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
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <errno.h>

#include "dnspecker.h"

RawSocketSender::RawSocketSender()
{
	buffer=calloc(1,sizeof(struct sockaddr_in));
	if (!buffer) throw ppl7::OutOfMemoryException();
	struct sockaddr_in *dest=(struct sockaddr_in *)buffer;
	dest->sin_addr.s_addr=-1;
	if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		free(buffer);
		ppl7::throwExceptionFromErrno(errno,"Could not create RawSocket");
	}
	unsigned int set =1;
	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &set, sizeof(set)) < 0) {
		close(sd);
		free(buffer);
		ppl7::throwExceptionFromErrno(errno,"Could not set socket option IP_HDRINCL");
	}
}

RawSocketSender::~RawSocketSender()
{
	close(sd);
	free(buffer);
}

void RawSocketSender::setDestination(const ppl7::IPAddress &ip_addr, int port)
{
	if (ip_addr.family()!=ppl7::IPAddress::IPv4)
		throw UnsupportedIPFamily("Only IPv4 is supported");
	ip_addr.toSockAddr(buffer,sizeof(struct sockaddr_in));
	((struct sockaddr_in *)buffer)->sin_port=htons(port);
}

ssize_t RawSocketSender::send(Packet &pkt)
{
	struct sockaddr_in *dest=(struct sockaddr_in *)buffer;
	if (dest->sin_addr.s_addr==(unsigned int)-1) throw UnknownDestination();
	return sendto(sd, pkt.ptr(),pkt.size(),0,
			(const struct sockaddr *)dest, sizeof(struct sockaddr_in));
}

ppl7::SockAddr RawSocketSender::getSockAddr() const
{
	return ppl7::SockAddr(buffer, sizeof(struct sockaddr_in));
}

bool RawSocketSender::socketReady()
{
	fd_set wset;
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=100;
	FD_ZERO(&wset);
	FD_SET(sd,&wset); // Wir wollen nur prüfen, ob wir schreiben können
	int ret=select(sd+1,NULL,&wset,NULL,&timeout);
	if (ret<0) return false;
	if (FD_ISSET(sd,&wset)) {
		return true;
	}
	return false;
}
