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
 * Copyright (c) 2013, Patrick Fedick <patrick@pfp.de>
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
	#ifdef HAVE_SYS_POLL_H
    #include <sys/poll.h>
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
#ifdef HAVE_SIGNAL_H
    #include <signal.h>
#endif

#ifdef HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
#ifdef HAVE_RESOLV_H
#include <resolv.h>
#endif


#include <list>
#include <set>

#include "ppl7.h"
#include "ppl7-inet.h"
//#include "socket.h"

namespace ppl7 {


String GetHostname()
/*!\brief Liefert den Hostnamen des Systems zurück
 * \ingroup PPLGroupInternet
 *
 * Diese Funktion liefert den Hostnamen des Systems als String zurück.
 *
 * @return Bei Erfolg befindet sich der Hostname im String, bei Misserfolg ist der
 * String leer. Es wird kein Fehlercode gesetzt.
 */
{
	String s;
#ifdef HAVE_GETHOSTNAME
	char h[256];
	if (gethostname(h,255)==0) {
		s=h;
		return s;
	}
	return s;
#else
	s=getenv("HOSTNAME");
	return s;
#endif

}

static size_t GetHostByNameInternal(const String &name, std::list<IPAddress> &result, int flags)
{
	int n;
	struct addrinfo hints, *res, *ressave;
	memset(&hints,0,sizeof(struct addrinfo));
	int family=flags&3;
	switch (family) {
		case af_inet: hints.ai_family=AF_INET; break;
		case af_inet6: hints.ai_family=AF_INET6; break;
		default: hints.ai_family=AF_UNSPEC; break;
	}
	hints.ai_socktype=SOCK_STREAM;
	if ((n=getaddrinfo((const char*)name,NULL,&hints,&res))!=0) {
#ifdef EAI_NODATA
		if (n==EAI_NODATA) return 0;
#endif
		if (n==EAI_NONAME) return 0;
		throw NetworkException("getaddrinfo(%s) returned %i: %s",(const char*)name,n,gai_strerror(n));
	}
	ressave=res;
	//char hbuf[INET6_ADDRSTRLEN];
	do {
		//if (getnameinfo(res->ai_addr,res->ai_addrlen, hbuf, sizeof(hbuf), NULL, 0, NI_NUMERICHOST) == 0) {
			IPAddress ip;
			//ip.set(String(hbuf));

			if (res->ai_family==AF_INET) {
				//HexDump(&((struct sockaddr_in*)res->ai_addr)->sin_addr,4);
				ip.set(IPAddress::IPv4, &((struct sockaddr_in*)res->ai_addr)->sin_addr,4);
			} else if (res->ai_family==AF_INET6) {
				//HexDump(&((struct sockaddr_in6*)res->ai_addr)->sin6_addr,16);
				ip.set(IPAddress::IPv6, &((struct sockaddr_in6*)res->ai_addr)->sin6_addr,16);
			}

			result.push_back(ip);
		//}
	} while ((res=res->ai_next)!=NULL);
	freeaddrinfo(ressave);
	return result.size();
}


/*!\brief Hostauflösung anhand des Namens
 * \ingroup PPLGroupInternet
 *
 * \header \#include <ppl7-inet.h>
 * \desc
 * Diese Funktion führt eine Namensauflösung durch. Dabei werden alle IPs zurückgegeben, die
 * auf den angegebenen Namen passen, einschließlich IPv6.
 *
 * \param name Der gesuchte Hostname oder die IP-Adresse, wobei sowohl IPv4- als auch IPv6-Adressen
 * unterstützt werden
 * \param result Liste vom Typ IPAddress, in der die gefundenen IP-Adressen gespeichert werden.
 * \param flags Bitmaske mit folgender Bedeutung:
 * - af_unspec: Das Betriebssystem entscheidet. Hier ist es häufig so, dass IPv6-Adressen nur
 *   dann zurückgegeben werden, wenn das System auch über eine IPv6-Anbindung verfügt.
 * - af_inet: nur nach IPv4 Adressen suchen
 * - af_inet6: nur nach IPv6 Adressen suchen
 * - af_all: IPv4 und IPv6 Adressen suchen
 *
 * \result Im Erfolgsfall, das heisst der angegebene Name konnte aufgelöst werden, liefert die
 * Funktion die Anzahl gefundener IP-Adressen zurück. Ausserdem werden die Adressen in die
 * Liste \p result kopiert. Wurde der Name \p name nicht gefunden, liefert die Funktion 0
 * zurück. Ist ein anderer Fehler aufgetreten (z.B. Netzwerkprobleme) wird eine Exception
 * geworfern
 *
 * \exception NetworkException Wird geworfen, wenn ein Netzwerkproblem aufgetreten ist.
 *
 * \note Es ist zu beachten, dass die Liste \p result erst gelöscht und dann mit den
 * gefundenen Daten gefüllt wird. Vorher vorhandene Daten gehen also verloren.
 *
 */
size_t GetHostByName(const String &name, std::list<IPAddress> &result, ResolverFlags flags)
{
	#ifdef _WIN32
		InitSockets();
	#endif
	result.clear();
	if (flags!=af_all) return GetHostByNameInternal(name,result,flags);
	int ret=GetHostByNameInternal(name,result,af_inet);
	std::list<IPAddress> additional;
	int ret2=GetHostByNameInternal(name,additional,af_inet6);
	// Hier könnten Duplikate entstanden sein, die wir nicht wollen (FreeBSD)
	if (ret2>0 && ret>0) {
		// Wir bauen uns erst ein Set aus den vorhandenen Adresen auf
		std::set<ppl7::IPAddress> have;
		std::set<ppl7::IPAddress>::iterator haveIt;
		std::list<ppl7::IPAddress>::iterator it;
		for (it=result.begin();it!=result.end();++it) have.insert(*it);
		// Dann gleichen wir die zusätzlichen Adressen mit den vorhandenen ab
		// und fügen nur das ins result hinzu, was noch nicht da ist
		for (it=additional.begin();it!=additional.end();++it) {
			haveIt=have.find(*it);
			if (haveIt==have.end()) {
				result.push_back(*it);
				ret++;
			}
		}
	} else if (ret2>0 && ret==0) {
		result=additional;
		return ret2;
	}
	return ret;
}


String GetHostByAddr(const IPAddress &addr)
/*!\brief Reverse-Lookup anhand einer IP-Adresse
 * \ingroup PPLGroupInternet
 *
 * \header \#include <ppl7-inet.h>
 * \desc
 * Diese Funktion führt eine Reverse-Abfrage einer IP-Adresse durch.
 *
 * \param addr Die gesuchte IP-Adresse, wobei sowohl IPv4- als auch IPv6-Adressen
 * unterstützt werden
 * \return String mit dem gefundenen Hostnamen
 *
 * \exception
 */
{
#ifdef _WIN32
	InitSockets();
#endif
	struct sockaddr saddr;
	addr.toSockAddr(&saddr, sizeof(saddr));

	char hbuf[NI_MAXHOST];
	unsigned int sa_len=4;
	if (saddr.sa_family==AF_INET6) sa_len=16;
	if (getnameinfo(&saddr,sa_len, hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD) == 0) {
		return String(hbuf);
	}
	throw UnknownHostException(addr.toString());
}


/*!\class Resolver
 * \ingroup PPLGroupInternet
 *
 *
 */

String Resolver::typeName(Type t)
{
        switch (t)  {
                case A: return "A";
                case NS: return "NS";
                case CNAME: return "CNAME";
                case MX: return "MX";
                case SOA: return "SOA";
                case PTR: return "PTR";
                case TXT: return "TXT";
                case AAAA: return "AAAA";
                case NAPTR: return "NAPTR";
                case SRV: return "SRV";
                case DS: return "DS";
                case DNSKEY: return "DNSKEY";
                case NSEC: return "NSEC";
                case NSEC3: return "NSEC3";
                case RRSIG: return "RRSIG";
                case OPT: return "OPT";
                case TSIG: return "TSIG";
                default: return "UNKNOWN";
        }
}

String Resolver::className(Class c)
{
        switch (c)  {
                case CLASS_IN: return "IN";
                case CLASS_CH: return "CH";
                case CLASS_HS: return "HS";
                case CLASS_NONE: return "NONE";
                case CLASS_ANY: return "ANY";
                default: return "UNKNOWN";
        }
}


String shortenIpv6(const String &s)
/*!\brief Reverse-Lookup anhand einer IP-Adresse
 * \ingroup PPLGroupInternet
 *
 * \header \#include <ppl7-inet.h>
 * \desc
 * Diese Funktion führt eine Reverse-Abfrage einer IP-Adresse durch.
 *
 * \param s String mit IPv6-Adresse
 * \return String abgekürzter IPv6-Adresse
 *
 */
{
	ppl7::String r=s;
	if (r.instr(":0:0:0:0:0:0:0:")>=0) r.replace(":0:0:0:0:0:0:0:","::");
	else if (r.instr(":0:0:0:0:0:0:")>=0) r.replace(":0:0:0:0:0:0:","::");
	else if (r.instr(":0:0:0:0:0:")>=0) r.replace(":0:0:0:0:0:","::");
	else if (r.instr(":0:0:0:0:")>=0) r.replace(":0:0:0:0:","::");
	else if (r.instr(":0:0:0:")>=0) r.replace(":0:0:0:","::");
	else if (r.instr(":0:0:")>=0) r.replace(":0:0:","::");
	if (r.right(3)=="::0") r.chop(1);
	return r;
}


void Resolver::query(Array &r, const String &label, Type t, Class c)
{
#ifndef HAVE_RES_SEARCH
	throw UnsupportedFeatureException("libbind res_search");
#else
	ppl7::ByteArray buf(4096);

	int ret=res_search((const char*)label,c,t,(u_char*)buf.adr(),buf.size());
	if (ret<0) {
		switch (h_errno) {
			case HOST_NOT_FOUND: throw HostNotFoundException(label);
			case TRY_AGAIN: throw TryAgainException();
			case NO_RECOVERY: throw QueryFailedException(label);
			case NO_DATA: throw NoResultException(label);
			case NETDB_INTERNAL: throwExceptionFromErrno(h_errno,ToString("Resolver::query NETDB_INTERNAL Error"));
			default: throw QueryFailedException(label);
		}
	}
	//buf.hexDump(ret);

	ns_msg handle;
	ret=ns_initparse((const u_char *)buf.adr(),ret,&handle);
	/*
	printf ("Msg-Id: %d\n",(int)ns_msg_id(handle));
	//printf ("Flags: %d\n",(int)ns_msg_get_flag(handle,ns_f_qr));
	printf ("Frage: %d\n",(int)ns_msg_count(handle,ns_s_qd));
	printf ("Answers: %d\n",(int)ns_msg_count(handle,ns_s_an));
	printf ("Zone: %d\n",(int)ns_msg_count(handle,ns_s_zn));
	printf ("Vorbedingung: %d\n",(int)ns_msg_count(handle,ns_s_pr));
	printf ("ns: %d\n",(int)ns_msg_count(handle,ns_s_ns));
	printf ("ud: %d\n",(int)ns_msg_count(handle,ns_s_ud));
	printf ("ar: %d\n",(int)ns_msg_count(handle,ns_s_ar));
	*/

	if (ns_msg_count(handle,ns_s_an)==0) throw QueryFailedException(ToString("Empty resultset"));
	for (u_int16_t i=0;i<ns_msg_count(handle,ns_s_an);i++) {
		ns_rr rr;
		if (ns_parserr(&handle,ns_s_an,i,&rr)==0) {
			//printf ("Record: %i: name: %s\n",i,ns_rr_name(rr));
			u_int16_t type=ns_rr_type(rr);
			if (type==NS) {
				char buf[MAXDNAME];
				if(ns_name_uncompress(
						ns_msg_base(handle),
						ns_msg_end(handle),
						ns_rr_rdata(rr),
						buf,
						MAXDNAME)) {
					//printf ("rdata: %s\n",buf);
					r.add(buf);
				}
			} else if (type==A) {
				unsigned char *adr=(unsigned char*)ns_rr_rdata(rr);
				r.addf("%i.%i.%i.%i",(int)adr[0],(int)adr[1],(int)adr[2],(int)adr[3]);
			} else if (type==AAAA) {
				//printf ("AAAA, len=%i\n",(int)ns_rr_rdlen(rr));
				//ppl7::HexDump(ns_rr_rdata(rr),ns_rr_rdlen(rr));
				ppluint16 *adr=(ppluint16*)ns_rr_rdata(rr);

				r.add(shortenIpv6(ToString("%x:%x:%x:%x:%x:%x:%x:%x",
						(int)ntohs(adr[0]),(int)ntohs(adr[1]),(int)ntohs(adr[2]),(int)ntohs(adr[3]),
						(int)ntohs(adr[4]),(int)ntohs(adr[5]),(int)ntohs(adr[6]),(int)ntohs(adr[7]))));
			} else if (type==SOA) {
				ppl7::HexDump(ns_rr_rdata(rr),ns_rr_rdlen(rr));
				char buf[MAXDNAME];
								if(ns_name_uncompress(
										ns_msg_base(handle),
										ns_msg_end(handle),
										ns_rr_rdata(rr),
										buf,
										MAXDNAME)) {
									//printf ("rdata: %s\n",buf);
									r.add(buf);
								}
			}


		}
	}
#endif
}



}	// namespace ppl7

