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

static int cmp_addr(IPAddress::IP_FAMILY f1, const void * a1, IPAddress::IP_FAMILY f2, const void *a2)
{
	if (f1<f2) return -1;
	if (f1>f2) return 1;
	if (f1==IPAddress::IPv4) {
		return memcmp(a1,a2,4);
	}
	return memcmp(a1,a2,16);
}

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

/*!\class IPAddress
 * \ingroup PPLGroupInternet
 *
 *
 */

IPAddress::IPAddress()
{
	_family=UNKNOWN;
}

IPAddress::IPAddress(const IPAddress &other)
{
	_family=UNKNOWN;
	set(other);
}

IPAddress::IPAddress(const String &other)
{
	_family=UNKNOWN;
	set(other);
}

IPAddress::IPAddress(IP_FAMILY family, void *addr, size_t addr_len)
{
	_family=UNKNOWN;
	set(family, addr, addr_len);
}


void IPAddress::toSockAddr(void *sockaddr, size_t sockaddr_len) const
{
	if (_family==UNKNOWN) throw InvalidIpAddressException();
	memset(sockaddr,0,sockaddr_len);
	if (_family==IPv4) {
		if (sockaddr_len<sizeof(struct sockaddr_in)) throw BufferTooSmallException();
		struct sockaddr_in *s=(struct sockaddr_in *)sockaddr;
		s->sin_family=AF_INET;
		memcpy(&s->sin_addr,_addr,4);
#ifdef HAVE_SOCKADDR_SA_LEN
		s->sin_len=sizeof(struct sockaddr_in);
#endif
	} else {
		if (sockaddr_len<sizeof(struct sockaddr_in6)) throw BufferTooSmallException();
		struct sockaddr_in6 *s=(struct sockaddr_in6 *)sockaddr;
		s->sin6_family=AF_INET6;
		memcpy(&s->sin6_addr,_addr,16);
#ifdef HAVE_SOCKADDR_SA_LEN
		s->sin6_len=sizeof(struct sockaddr_in6);
#endif
	}
}

IPAddress &IPAddress::operator=(const IPAddress &other)
{
	set(other);
	return *this;
}

IPAddress &IPAddress::operator=(const String &other)
{
	set(other);
	return *this;
}


void IPAddress::set(const IPAddress &other)
{
	_family=other._family;
	memcpy(&_addr, &other._addr, sizeof(_addr));
}

void IPAddress::set(const String &address)
{
	int res=0;
	if (address.has(":")) {
		res=inet_pton(AF_INET6, address.getPtr(), &_addr);
		_family=IPv6;
	} else {
		res=inet_pton(AF_INET, address.getPtr(), &_addr);
		_family=IPv4;
	}
	if (res!=1) {
		_family=UNKNOWN;
		throw InvalidIpAddressException("%s",(const char*)address);
	}
}

void IPAddress::set(IP_FAMILY family, void *addr, size_t addr_len)
{
	if (family==UNKNOWN) throw IllegalArgumentException();
	if (addr==NULL) throw IllegalArgumentException();
	if (family==IPv4 && addr_len!=4) throw InvalidIpAddressException();
	else if (family==IPv6 && addr_len!=16) throw InvalidIpAddressException();
	_family=family;
	if (addr_len>sizeof(_addr)) {
		throw InvalidIpAddressException();
	}
	memcpy(&_addr, addr, addr_len);
}

IPAddress::IP_FAMILY IPAddress::family() const
{
	return _family;
}

const void *IPAddress::addr() const
{
	if (_family==UNKNOWN) throw InvalidIpAddressException();
	return &_addr;
}

size_t IPAddress::addr_len() const
{
	if (_family==UNKNOWN) throw InvalidIpAddressException();
	if (_family==IPv4) return 4;
	return 16;
}


String IPAddress::toString() const
{
	if (_family==UNKNOWN) throw InvalidIpAddressException();
	char hbuf[INET6_ADDRSTRLEN];
	memset(hbuf,0,INET_ADDRSTRLEN);
	if (_family==IPv4) {
		if (NULL==inet_ntop(AF_INET, &_addr, hbuf,INET6_ADDRSTRLEN)) {
			throw InvalidIpAddressException();
		}
	} else {
		if (NULL==inet_ntop(AF_INET6, &_addr, hbuf,INET6_ADDRSTRLEN)) {
			throw InvalidIpAddressException();
		}
	}
	return String(hbuf);
}

IPAddress::operator String() const
{
	return toString();
}

IPAddress IPAddress::mask(int prefixlen) const
{
	int hbyte=0;
	if (_family==IPAddress::IPv4) {
		if (prefixlen<0 || prefixlen>32) throw InvalidNetmaskOrPrefixlenException("%d",prefixlen);
		hbyte=3;
	} else if (_family==IPAddress::IPv6) {
		if (prefixlen<0 || prefixlen>128) throw InvalidNetmaskOrPrefixlenException("%d",prefixlen);
		hbyte=15;
	} else {
		throw InvalidIpAddressException();
	}
	IPAddress tmp(*this);
	int prange=hbyte*8;
	for (int byte=hbyte;byte>=0;byte--) {
		if (prefixlen>prange) {
			tmp._addr[byte]&=(unsigned char)bytemask[prefixlen-prange];
			byte=0;
		} else {
			tmp._addr[byte]=(unsigned char)0;
		}
		prange-=8;
	}
	return tmp;
}

int IPAddress::compare(const IPAddress &other) const
{
	return cmp_addr(_family,_addr, other._family, other._addr);
}

bool IPAddress::operator<(const IPAddress &other) const
{
	if (cmp_addr(_family,_addr, other._family, other._addr)<0) return true;
	return false;
}

bool IPAddress::operator<=(const IPAddress &other) const
{
	if (cmp_addr(_family,_addr, other._family, other._addr)<=0) return true;
	return false;
}

bool IPAddress::operator==(const IPAddress &other) const
{
	if (cmp_addr(_family,_addr, other._family, other._addr)==0) return true;
	return false;
}

bool IPAddress::operator!=(const IPAddress &other) const
{
	if (cmp_addr(_family,_addr, other._family, other._addr)!=0) return true;
	return false;
}

bool IPAddress::operator>=(const IPAddress &other) const
{
	if (cmp_addr(_family,_addr, other._family, other._addr)>=0) return true;
	return false;
}

bool IPAddress::operator>(const IPAddress &other) const
{
	if (cmp_addr(_family,_addr, other._family, other._addr)>0) return true;
	return false;
}


std::ostream& operator<<(std::ostream& s, const IPAddress &addr)
{
	String str=addr.toString();
	return s.write((const char*)str,str.size());
}

}	// namespace ppl7

