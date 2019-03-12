/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: http://www.pfp.de/ppl/
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *******************************************************************************
 * Copyright (c) 2017, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include "prolog.h"
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include <time.h>
#ifdef _WIN32
    #include <winsock2.h>
	#include <Ws2tcpip.h>
	#include <windows.h>
#else
	#ifdef HAVE_UNISTD_H
    #include <unistd.h>
	#endif
	#ifdef HAVE_SYS_SOCKET_H
    #include <sys/socket.h>
	#endif
	#ifdef HAVE_NETINET_IN_H
    #include <netinet/in.h>
	#endif
	#ifdef HAVE_NETDB_H
    #include <netdb.h>
	#endif
	#ifdef HAVE_ARPA_INET_H
    #include <arpa/inet.h>
	#endif
	#include <netdb.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif


#include "ppl7.h"
#include "ppl7-inet.h"
//#include "socket.h"

namespace ppl7 {

static unsigned int bytemask[]={
		0x00,
		0x80,
		0xc0,
		0xe0,
		0xf0,
		0xf8,
		0xfc,
		0xfe,
		0xff
};

/*!\class IPNetwork
 * \ingroup PPLGroupInternet
 *
 *
 */


IPNetwork::IPNetwork()
{
	_prefixlen=-1;
}

IPNetwork::IPNetwork(const IPNetwork &other)
{
	_prefixlen=-1;
	set(other);
}

IPNetwork::IPNetwork(const String &other)
{
	_prefixlen=-1;
	set(other);
}

IPNetwork &IPNetwork::operator=(const IPNetwork &other)
{
	set(other);
	return *this;
}

IPNetwork &IPNetwork::operator=(const String &other)
{
	set(other);
	return *this;
}


void IPNetwork::set(const IPAddress &other, int prefixlen)
{
	_addr=other.mask(prefixlen);
	_prefixlen=prefixlen;
}

void IPNetwork::set(const IPNetwork &other)
{
	_prefixlen=other._prefixlen;
	_addr=other._addr;
}

static int validateNetmaskByte(unsigned char byte)
{
	switch(byte) {
		case 0xfe: return 7;
		case 0xfc: return 6;
		case 0xf8: return 5;
		case 0xf0: return 4;
		case 0xe0: return 3;
		case 0xc0: return 2;
		case 0x80: return 1;
		case 0x00: return 0;
		default: return -1;
	}
	return -1;
}

int IPNetwork::getPrefixlenFromNetmask(const String &netmask)
{
	unsigned char _addr[16];
	int len;
	int res=0;
	if (netmask.has(":")) {
		res=inet_pton(AF_INET6, netmask.getPtr(), &_addr);
		len=16;
	} else {
		res=inet_pton(AF_INET, netmask.getPtr(), &_addr);
		len=4;
	}
	if (res!=1) throw InvalidNetmaskOrPrefixlenException("%s",(const char*)netmask);
	int stage=0;
	int prefixlen=0;
	for (int i=0;i<len;i++) {
		unsigned char byte=_addr[i];
		if (stage==0 && byte==0xff) {
			prefixlen+=8;
		} else if (stage==1 && byte!=0) {
			throw InvalidNetmaskOrPrefixlenException("%s",(const char*)netmask);
		} else if (stage==0 && byte!=0xff) {
			stage=1;
			int bits=validateNetmaskByte(byte);
			if (bits<0) throw InvalidNetmaskOrPrefixlenException("%s",(const char*)netmask);
			prefixlen+=bits;
		}
	}
	return prefixlen;
}

void IPNetwork::set(const String &network)
{
	int t=network.instr("/");
	if (t<0) throw InvalidNetworkAddressException(network);
	IPAddress addr=network.left(t);
	String mask=network.mid(t+1);
	int prefixlen=0;
	if (mask.instr(".")>=0 || mask.instr(":")>=0) {
		prefixlen=getPrefixlenFromNetmask(mask);
		if (prefixlen<0) throw InvalidNetmaskOrPrefixlenException("%s",(const char*)mask);
	} else {
		prefixlen=mask.toInt();
	}
	_addr=addr.mask(prefixlen);
	_prefixlen=prefixlen;
}

IPAddress::IP_FAMILY IPNetwork::family() const
{
	return _addr.family();
}

IPAddress IPNetwork::addr() const
{
	if (_prefixlen<0) throw InvalidNetworkAddressException();
	return _addr;
}

IPAddress IPNetwork::first() const
{
	if (_prefixlen<0) throw InvalidNetworkAddressException();
	return _addr;
}

IPAddress IPNetwork::last() const
{
	if (_prefixlen<0) throw InvalidNetworkAddressException();
	IPAddress mask=netmask();
	IPAddress ret(_addr);
	unsigned char *adr=(unsigned char*)mask.addr();
	unsigned char *r=(unsigned char*)ret.addr();
	for (size_t i=0;i<mask.addr_len();i++)
		r[i]+=255-adr[i];
	return ret;
}

int IPNetwork::prefixlen() const
{
	if (_prefixlen<0) throw InvalidNetworkAddressException();
	return _prefixlen;
}

String IPNetwork::toString() const
{
	if (_prefixlen<0) throw InvalidNetworkAddressException();
	return ppl7::ToString("%s/%d",(const char*)_addr.toString(),_prefixlen);
}

IPNetwork::operator String() const
{
	return toString();
}


IPAddress IPNetwork::netmask() const
{
	int hbyte=0;
	if (_addr.family()==IPAddress::IPv4) {
		hbyte=3;
	} else if (_addr.family()==IPAddress::IPv6) {
		hbyte=15;
	} else {
		throw InvalidNetworkAddressException();
	}
	unsigned char tmp[16];
	memset(tmp,255,16);
	int prange=hbyte*8;
	for (int byte=hbyte;byte>=0;byte--) {
		if (_prefixlen>prange) {
			tmp[byte]=(unsigned char)bytemask[_prefixlen-prange];
			byte=0;
		} else {
			tmp[byte]=(unsigned char)0;
		}
		prange-=8;
	}
	return IPAddress(_addr.family(),&tmp,_addr.addr_len());
}

bool IPNetwork::contains(const IPAddress &addr) const
{
	IPAddress start=first();
	IPAddress end=last();
	if (addr>=start && addr<=end) return true;
	return false;
}


static int cmp_net(const IPNetwork &net1, const IPNetwork &net2)
{
	int ret=net1.addr().compare(net2.addr());
	if (ret==0) {
		if (net1.prefixlen()<net2.prefixlen()) ret=1;
		else if (net1.prefixlen()>net2.prefixlen()) ret=-1;
	}
	return ret;
}


int IPNetwork::compare(const IPNetwork &other) const
{
	return cmp_net(*this, other);
}

bool IPNetwork::operator<(const IPNetwork &other) const
{
	if (cmp_net(*this, other)<0) return true;
	return false;
}

bool IPNetwork::operator<=(const IPNetwork &other) const
{
	if (cmp_net(*this, other)<=0) return true;
	return false;
}

bool IPNetwork::operator==(const IPNetwork &other) const
{
	if (cmp_net(*this, other)==0) return true;
	return false;
}

bool IPNetwork::operator!=(const IPNetwork &other) const
{
	if (cmp_net(*this, other)!=0) return true;
	return false;
}

bool IPNetwork::operator>=(const IPNetwork &other) const
{
	if (cmp_net(*this, other)>=0) return true;
	return false;
}

bool IPNetwork::operator>(const IPNetwork &other) const
{
	if (cmp_net(*this, other)>0) return true;
	return false;
}


std::ostream& operator<<(std::ostream& s, const IPNetwork &net)
{
	String str=net.toString();
	return s.write((const char*)str,str.size());
}




}	// namespace ppl7

