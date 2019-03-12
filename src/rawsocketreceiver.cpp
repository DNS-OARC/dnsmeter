#include <ppl7.h>
#include <ppl7-inet.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#ifndef __FreeBSD__
#define __FAVOR_BSD
#endif
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#ifdef __FreeBSD__
#include <netinet/in_systm.h>
#include <net/if.h>
#include <net/bpf.h>
#include <net/ethernet.h>
#include <machine/atomic.h>
#endif

#include "dnspecker.h"

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct ETHER
{
	unsigned char destination[6];
	unsigned char source[6];
	unsigned short type;
};
#pragma pack(pop)   /* restore original alignment from stack */


#ifdef __FreeBSD__
static int open_bpf()
{
	int sd;
	for (int i=0;i<255;i++) {
		ppl7::String Device;
		Device.setf("/dev/bpf%d", i);
		sd=open((const char*)Device, O_RDWR);
		if (sd>=0) {
			return sd;
		}
	}
	ppl7::throwExceptionFromErrno(errno,"Could not create RawReceiverSocket");
	return -1;
}

bool tryAllocZeroCopyBuffer(int sd, struct bpf_zbuf *zbuf, size_t size)
{
	zbuf->bz_buflen=size;
	zbuf->bz_bufa=malloc(zbuf->bz_buflen);
	if (!zbuf->bz_bufa) {
		throw ppl7::OutOfMemoryException();
	}
	zbuf->bz_bufb=malloc(zbuf->bz_buflen);
	if (!zbuf->bz_bufb) {
		free(zbuf->bz_bufa);
		throw ppl7::OutOfMemoryException();
	}
	memset(zbuf->bz_bufa,0,zbuf->bz_buflen);
	memset(zbuf->bz_bufb,0,zbuf->bz_buflen);

	if (ioctl(sd, BIOCSETZBUF, zbuf) < 0) {
		free(zbuf->bz_bufa);
		free(zbuf->bz_bufb);
		return false;
	}
	return true;
}

void initZeroCopyBuffer(int sd, struct bpf_zbuf *zbuf)
{
	unsigned int bufmode=BPF_BUFMODE_ZBUF;
	if (ioctl(sd, BIOCSETBUFMODE, &bufmode) < 0) {
		ppl7::throwExceptionFromErrno(errno,"BIOCSETBUFMODE with BPF_BUFMODE_ZBUF failed");
	}
	unsigned int tstype=BPF_T_MICROTIME;
	if (ioctl(sd, BIOCSTSTAMP, &tstype) < 0) {
		ppl7::throwExceptionFromErrno(errno,"BIOCSTSTAMP");
	}
	if (tryAllocZeroCopyBuffer(sd, zbuf, 8192)) return;
	if (tryAllocZeroCopyBuffer(sd, zbuf, 4096)) return;
	throw FailedToInitializePacketfilter("Could not configure ZeroCopy-Buffer (BIOCSETZBUF)");
}

void initBufferedMode(int sd, unsigned int buflen)
{
	unsigned int bufmode=BPF_BUFMODE_BUFFER;
	if (ioctl(sd, BIOCSETBUFMODE, &bufmode) < 0) {
		ppl7::throwExceptionFromErrno(errno,"BIOCSETBUFMODE with BPF_BUFMODE_BUFFER failed");
	}
	if (ioctl(sd, BIOCSBLEN, &buflen) < 0) {
		ppl7::throwExceptionFromErrno(errno,"BIOCSBLEN failed");
	}
}

#endif

RawSocketReceiver::Counter::Counter()
{
	num_pkgs=0;
	bytes_rcv=0;
	truncated=0;
	for (int i=0;i<15;i++) rcodes[i]=0;
	rtt_total=0.0f;
	rtt_min=0.0f;
	rtt_max=0.0f;
}

void RawSocketReceiver::Counter::clear()
{
	num_pkgs=0;
	bytes_rcv=0;
	truncated=0;
	for (int i=0;i<15;i++) rcodes[i]=0;
	rtt_total=0.0f;
	rtt_min=0.0f;
	rtt_max=0.0f;
}


RawSocketReceiver::RawSocketReceiver()
{
	SourceIP.set("0.0.0.0");
	SourcePort=0;
	buflen=4096;
	sd=-1;
	buffer=NULL;
#ifdef __FreeBSD__
	useZeroCopyBuffer=false;
	sd=open_bpf();
	buffer=(unsigned char*)malloc(sizeof(struct bpf_zbuf));
	if (!buffer) { close(sd); throw ppl7::OutOfMemoryException();}
	struct bpf_zbuf *zbuf=(struct bpf_zbuf*)buffer;

	try {
		initZeroCopyBuffer(sd,zbuf);
		useZeroCopyBuffer=true;
		buflen=zbuf->bz_buflen;
		printf("INFO: using fast bpf zero copy buffer for packet capturing\n");
		return;
	} catch (const ppl7::Exception &ex) {
		useZeroCopyBuffer=false;
		free(buffer);
	}
	buflen=8192;
	buffer=(unsigned char*)malloc(buflen);
	if (!buffer) { close(sd); throw ppl7::OutOfMemoryException();}
	try {
		initBufferedMode(sd, buflen);
		printf("INFO: using normal bpf buffered mode for packet capturing\n");
		int ret=fcntl(sd,F_SETFL,fcntl(sd,F_GETFL,0)|O_NONBLOCK);// NON-Blocking
		if (ret<0) ppl7::throwExceptionFromErrno(errno, "Could not set bpf into non blocking mode");

	} catch (const ppl7::Exception &ex) {
		free(buffer);
		close(sd);
		throw;
	}
	
#else
	buffer=(unsigned char*)malloc(buflen);
	if (!buffer) throw ppl7::OutOfMemoryException();
	if ((sd = socket(AF_PACKET, SOCK_RAW, htons(0x0800))) == -1) {
		int e=errno;
		free(buffer);
		ppl7::throwExceptionFromErrno(e,"Could not create RawReceiverSocket");
	}
	int ret=fcntl(sd,F_SETFL,fcntl(sd,F_GETFL,0)|O_NONBLOCK);// NON-Blocking
	if (ret<0) {
		int e=errno;
		close(sd);
		free(buffer);
		ppl7::throwExceptionFromErrno(e, "Could not set bpf into non blocking mode");
	}

#endif
}

RawSocketReceiver::~RawSocketReceiver()
{
	close(sd);
#ifdef __FreeBSD__
	if (useZeroCopyBuffer) {
		struct bpf_zbuf *zbuf=(struct bpf_zbuf*)buffer;
		free(zbuf->bz_bufa);
		free(zbuf->bz_bufb);
	}
#endif
	free(buffer);
}

void RawSocketReceiver::initInterface(const ppl7::String &Device)
{
#ifdef __FreeBSD__
	struct ifreq ifreq;
	strcpy((char *) ifreq.ifr_name, (const char*)Device);
	if (ioctl(sd, BIOCSETIF, &ifreq) < 0) {
		ppl7::throwExceptionFromErrno(errno,"Could not bind RawReceiverSocket on interface (BIOCSETIF)");
	}
	unsigned int promiscuous_mode=1;
	if (ioctl(sd, BIOCPROMISC, &promiscuous_mode) < 0) {
		ppl7::throwExceptionFromErrno(errno,"Could not set Interface into promiscuous mode (BIOCPROMISC)");
	}
#endif
}

void RawSocketReceiver::setSource(const ppl7::IPAddress &ip_addr, int port)
{
	SourceIP=ip_addr;
	SourcePort=htons(port);
#ifdef __FreeBSD__
	// Install packet filter in bpf
	int sip=htonl(*(int*)SourceIP.addr());
	struct bpf_insn insns[] = {
		// load halfword at position 12 from packet into register
		BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),
		// is it 0x800? if no, jump over 5 instructions, else jump over 0
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x0800, 0, 7),
		// source ip
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 26),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, (unsigned int)sip, 0, 5),

		// udp?
		BPF_STMT(BPF_LD+BPF_B+BPF_ABS, 23),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 17, 0, 3),

		// source port
		BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 34),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, (unsigned int)port, 0, 1),

		/* if we reach here, return -1 which will allow the packet to be read */
		BPF_STMT(BPF_RET+BPF_K, (u_int)-1),
		/* if we reach here, return 0 which will ignore the packet */
		BPF_STMT(BPF_RET+BPF_K, 0),
	};
	struct bpf_program bpf_program = {
		10,
		(struct bpf_insn *) &insns
	};
	if (ioctl(sd, BIOCSETF, (struct bpf_program *) &bpf_program) < 0) {
		throw FailedToInitializePacketfilter();
	}
#endif
}


bool RawSocketReceiver::socketReady()
{
#ifdef __FreeBSD__
	//if (useZeroCopyBuffer) return true;
#endif
	fd_set rset;
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=100;
	FD_ZERO(&rset);
	FD_SET(sd,&rset); // Wir wollen nur prüfen, ob wir lesen können
	int ret=select(sd+1,&rset,NULL,NULL,&timeout);
	if (ret<0) return false;
	if (FD_ISSET(sd,&rset)) {
		return true;
	}
	return false;
}


static void count_packet(RawSocketReceiver::Counter &counter, unsigned char *buffer, size_t size)
{
	counter.num_pkgs++;
	counter.bytes_rcv+=size;
	struct DNS_HEADER *dns=(struct DNS_HEADER *)(buffer+14+sizeof(struct ip)+sizeof(struct udphdr));
	double rd=getQueryRTT(ntohs(dns->id));
	counter.rtt_total+=rd;
	if (rd<counter.rtt_min || counter.rtt_min==0) counter.rtt_min=rd;
	if (rd>counter.rtt_max) counter.rtt_max=rd;
	if (dns->rcode<16) counter.rcodes[dns->rcode]++;
	if (dns->tc) counter.truncated++;
}

#ifdef __FreeBSD__
/*
 *	Return ownership of a buffer to	the kernel for reuse.
 */
static void buffer_acknowledge(struct bpf_zbuf_header *bzh)
{
	atomic_store_rel_int(&bzh->bzh_user_gen, bzh->bzh_kernel_gen);
}

static int buffer_check(struct bpf_zbuf_header *bzh)
{
	return (bzh->bzh_user_gen !=
			atomic_load_acq_int(&bzh->bzh_kernel_gen));
}

static void read_buffer(unsigned char *ptr, size_t size, RawSocketReceiver::Counter &counter)
{
	size_t done=0;
	while (done<size) {
		struct bpf_hdr* bpfh=(struct bpf_hdr*)ptr;
		if (bpfh->bh_caplen==0 || bpfh->bh_hdrlen==0) break;
		size_t chunk_size=BPF_WORDALIGN(bpfh->bh_caplen+bpfh->bh_hdrlen);
		count_packet(counter,ptr+bpfh->bh_hdrlen,chunk_size-bpfh->bh_datalen);
		ptr+=chunk_size;
		done+=chunk_size;
	}
}

static void read_zbuffer(struct bpf_zbuf_header *zhdr, RawSocketReceiver::Counter &counter)
{
	size_t size=zhdr->bzh_kernel_len-sizeof(struct bpf_zbuf_header);
	unsigned char *ptr=(unsigned char *)zhdr+sizeof(struct bpf_zbuf_header);
	read_buffer(ptr,size,counter);
	buffer_acknowledge(zhdr);
}
void RawSocketReceiver::receive(RawSocketReceiver::Counter &counter)
{
	if (useZeroCopyBuffer) {
		struct bpf_zbuf *zbuf=(struct bpf_zbuf*)buffer;
		struct bpf_zbuf_header *zhdr=NULL;
		if (buffer_check((struct bpf_zbuf_header *)zbuf->bz_bufa)) {
			zhdr=((struct bpf_zbuf_header *)zbuf->bz_bufa);
			read_zbuffer(zhdr, counter);
		}
		if (buffer_check((struct bpf_zbuf_header *)zbuf->bz_bufb)) {
			zhdr=((struct bpf_zbuf_header *)zbuf->bz_bufb);
			read_zbuffer(zhdr, counter);
		}
	} else {
		ssize_t bufused=read(sd,buffer,buflen);
		if (bufused<34) return;
		read_buffer(buffer,bufused,counter);
	}
}

#else
void RawSocketReceiver::receive(Counter &counter)
{
	unsigned char *ptr=buffer;
	ssize_t bufused=recvfrom(sd,buffer,buflen,0,NULL,NULL);
	if (bufused<34) return;
	struct ETHER *eth=(struct ETHER*)ptr;
	//ppl7::HexDump(ptr,bufused);
	//printf ("sizeof ETHER=%d, type=%X\n",sizeof(struct ETHER),eth->type);
	if (eth->type!=htons(0x0800)) return;
	struct ip *iphdr = (struct ip *)(ptr+14);
	if (iphdr->ip_v!=4) return;
	if (iphdr->ip_src.s_addr!=*(in_addr_t*)SourceIP.addr()) return;

	struct udphdr *udp = (struct udphdr *)(ptr+14+sizeof(struct ip));
	if (udp->uh_sport!=SourcePort) return;
	count_packet(counter,ptr,bufused);
}
#endif
