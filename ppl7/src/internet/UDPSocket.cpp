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
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

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


#include "ppl7.h"
#include "ppl7-inet.h"
#include "socket.h"

namespace ppl7 {


/*!\class UDPSocket
 * \ingroup PPLGroupInternet
 * \brief UDP-Socket-Klasse
 *
 * \header \#include <ppl7-inet.h>
 * \desc
 * Mit dieser Klasse können Pakete per UDP verschickt und empfangen werden.
 */

UDPSocket::UDPSocket()
/*! \brief Konstruktor der Klasse
 *
 * \header \#include <ppl7-inet.h>
 * \desc
 * Initialisiert interne Daten der Klasse
 */
{
#ifdef _WIN32
	InitSockets();
#endif
	timeout_sec=0;
	timeout_usec=0;
	socket=NULL;
	SourcePort=0;
	connect_timeout_sec=0;
	connect_timeout_usec=0;
	connected=false;
}

UDPSocket::~UDPSocket()
/*! \brief Destruktor der Klasse
 *
 * \header \#include <ppl6-inet.h>
 * \desc
 * De-Initialisiert interne Daten der Klasse
 *
 */
{
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (!s) {
		return;
	}
    if ((int)s->sd>-1) {
#ifdef _WIN32
		closesocket(s->sd);
#else
		close(s->sd);
#endif
    }
	free(s);
}

void UDPSocket::setTimeoutConnect(int seconds, int useconds)
{
	connect_timeout_sec = seconds;
	connect_timeout_usec = useconds;
}

/*!\brief Descriptor des Sockets auslesen
 *
 * \desc
 * Mit dieser Funktion kann der Betriebssystem-spezifische Socket-Descriptor ausgelesen werden.
 * Unter Unix ist dies ein File-Descriptor vom Typ Integer, unter Windows ein Socket-Descriptor
 * vom Typ SOCKET.
 *
 * @return Betriebsystem-spezifischer Descriptor.
 * @exception NotConnectedException Wird geworfen, wenn kein Socket geöffnet ist
 */
int UDPSocket::getDescriptor()
{
	if (socket==NULL)
		throw NotConnectedException();
	PPLSOCKET *s = (PPLSOCKET*) socket;
#ifdef _WIN32
	if ((int)s->sd<0) throw NotConnectedException();
	return (int)s->sd;
#else
	if (s->sd<0) throw NotConnectedException();
	return s->sd;
#endif
}

void UDPSocket::disconnect()
{
	PPLSOCKET *s = (PPLSOCKET*) socket;
	if (!s)
		return;
	/*
	if (islisten) {
		stopListen();
	}
	*/
	if (s->sd > 0) {
#ifdef _WIN32
		closesocket(s->sd);
#else
		close(s->sd);
#endif
		s->sd = 0;
	}
	connected=false;
}

/*!\brief Quell-Interface und Port festlegen
 *
 * \desc
 * Diese Funktion kann aufgerufen werden, wenn der Rechner über mehrere Netzwerkinterfaces verfügt.
 * Normalerweise entscheidet das Betriebssytem, welches Interface für eine ausgehende Verbindung
 * verwendet werden soll, aber manchmal kann es sinnvoll sein, dies manuell zu machen.
 *
 * @param[in] interface Hostname oder IP-Adresse des Quellinterfaces. Bleibt der Parameter leer,
 * wird nur der \p port beachtet
 * @param[in] port Port-Nummer des Quellinterfaces. Wird 0 angegeben, wird nur das \p interface
 * beachtet
 *
 * \attention
 * Diese Funktionalität wird derzeit nicht unter Windows unterstützt! Falls trotzdem ein
 * \p host oder \p port definiert wurden, wird die Connect-Funktion fehlschlagen!
 *
 * \remarks
 * Sind beide Parameter leer bzw. 0, wird das Quellinterface und Port vom Betriebssystem
 * festgelegt.
 *
 */
void UDPSocket::setSource(const String &interface, int port)
{
	SourceInterface = interface;
	SourcePort = port;
}

#ifdef WIN32
static int out_bind(const char *host, int port)
{
	throw UnsupportedFeatureException("TCPSocket.connect after TCPSocket.bind");
}

#else
/*!\brief Socket auf ausgehendes Interface legen
 *
 * \desc
 * Diese Funktion wird intern durch die Connect-Funktionen aufgerufen, um einen ausgehenden Socket auf
 * ein bestimmtes Netzwerk-Interface zu binden. Die Funktion wird nur dann verwendet, wenn
 * mit CTCPSocket::SetSource ein Quellinterface definiert wurde.
 *
 * @param[in] host Hostname oder IP des lokalen Interfaces
 * @param[in] port Port auf dem lokalen Interface
 * @return Bei Erfolg liefert die Funktion die File-Nummer des Sockets zurück,
 * im Fehlerfall wird eine Exception geworfen.
 *
 * @exception IllegalArgumentException Wird geworfen, wenn \p host auf NULL zeigt und \p port 0 enthält
 *
 * \relates ppl6::CTCPSocket
 */
static int out_bind(const char *host, int port)
{
	struct addrinfo hints, *res, *ressave;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	int listenfd;
	int n;
	int on = 1;
	// Prüfen, ob host ein Wildcard ist
	if (host != NULL && strlen(host) == 0)
		host = NULL;
	if (host != NULL && host[0] == '*')
		host = NULL;

	if (host != NULL && port > 0) {
		char portstr[10];
		sprintf(portstr, "%i", port);
		n = getaddrinfo(host, portstr, &hints, &res);
	} else if (host) {
		n = getaddrinfo(host, NULL, &hints, &res);
	} else if (port) {
		char portstr[10];
		sprintf(portstr, "%i", port);
		n = getaddrinfo(NULL, portstr, &hints, &res);
	} else {
		throw IllegalArgumentException();
	}
	if (n != 0) {
		throwExceptionFromEaiError(n, ToString("UDPSocket bind connect: %s:%i", host, port));
	}
	ressave = res;
	do {
		listenfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue; // Error, try next one

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
			freeaddrinfo(ressave);
			throwExceptionFromErrno(errno, ToString("UDPSocket bind connect: %s:%i", host, port));
		}

		//HexDump(res->ai_addr,res->ai_addrlen);
		if (::bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {

			break;
		}
		shutdown(listenfd, 2);
		close(listenfd);
		listenfd = 0;
	} while ((res = res->ai_next) != NULL);
	freeaddrinfo(ressave);
	if (listenfd <= 0)
		throw CouldNotOpenSocketException("Host: %s, Port: %d", host, port);
	if (res == NULL)
		throw CouldNotBindToInterfaceException("Host: %s, Port: %d", host, port);
	return (listenfd);
}
#endif

/*!\brief Verbindung aufbauen
 *
 */
void UDPSocket::connect(const String &host_and_port)
{
	if (host_and_port.isEmpty())
		throw IllegalArgumentException("UDPSocket::connect(const String &host_and_port)");
	Array hostname = StrTok(host_and_port, ":");
	if (hostname.size() != 2)
		throw IllegalArgumentException("UDPSocket::connect(const String &host_and_port)");
	String portname = hostname.get(1);
	int port = portname.toInt();
	if (port <= 0 && portname.size() > 0) {
		// Vielleicht wurde ein Service-Namen angegeben?
		struct servent *s = getservbyname((const char*) portname, "tcp");
		if (s) {
			unsigned short int p = s->s_port;
			port = (int) ntohs(p);
		} else {
			throw IllegalPortException("UDPSocket::connect(const String &host_and_port=%s)", (const char*) host_and_port);
		}
	}
	if (port <= 0)
		throw IllegalPortException("UDPSocket::connect(const String &host_and_port=%s)", (const char*) host_and_port);
	return connect(hostname.get(0), port);
}

#ifdef _WIN32
#else

/*!\brief Non-Blocking-Connect
 *
 * \desc
 * Diese Funktion wird intern durch die Connect-Funktionen aufgerufen, um einen nicht blockierenden
 * Connect auf die Zieladresse durchzuführen. Normalerweise würde ein Connect solange
 * blockieren, bis entweder ein Fehler festgestellt wird, die Verbindung zustande kommt oder
 * das Betriebssystem einen Timeout auslöst. Letzteres kann aber mehrere Minuten dauern.
 * Mit dieser Funktion wird ein non-blocking-connect durchgeführt, bei dem der Timeout
 * mikrosekundengenau angegeben werden kann.
 *
 * @param[in] sockfd File-ID des Sockets
 * @param[in] serv_addr IP-Adressenstruktur mit der Ziel-IP un dem Ziel-Port
 * @param[in] addrlen Die Länge der Adressenstruktur
 * @param[in] sec Timeout in Sekunden
 * @param[in] usec Timeout in Mikrosekunden. Der tatsächliche Timeout errechnet sich aus \p sec + \p usec
 * @return Bei Erfolg liefert die Funktion 1 zurück, im Fehlerfall 0.
 *
 * \relates ppl6::CTCPSocket
 * \note
 * Zur Zeit wird diese Funktion nur unter Unix unterstützt.
 */
static int ppl_connect_nb(int sockfd, struct sockaddr *serv_addr, int addrlen, int sec, int usec)
{
	int flags, n, error;
	struct timeval tval;
	socklen_t len;
	fd_set rset, wset;
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	error = 0;
	if ((n = ::connect(sockfd, serv_addr, addrlen)) < 0)
		if (errno != EINPROGRESS)
			return (-1);
	if (n == 0) // Connect completed immediately
		goto done;
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = sec;
	tval.tv_usec = usec;
	if ((n = select(sockfd + 1, &rset, &wset, NULL, &tval)) == 0) {
		errno = ETIMEDOUT;
		return -1;
	}
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			return -1; // Solaris pending error
		}
	}
	done: fcntl(sockfd, F_SETFL, flags);
	if (error) {
		errno = error;
		return (-1);
	}
	return 0;
}
#endif



void UDPSocket::connect(const String &host, int port)
{
	if (connected)
		disconnect();
	//if (islisten)
	//	disconnect();
	if (!socket) {
		socket = malloc(sizeof(PPLSOCKET));
		if (!socket)
			throw OutOfMemoryException();
		PPLSOCKET *s = (PPLSOCKET*) socket;
		s->sd = 0;
		//s->proto=6;
		s->proto = 0;
		s->ipname = NULL;
		s->port = 0;
		//s->addrlen=0;
	}
	PPLSOCKET *s = (PPLSOCKET*) socket;
	if (s->ipname)
		free(s->ipname);
	s->ipname = NULL;

	if (s->sd)
		disconnect();
	if (host.isEmpty())
		throw IllegalArgumentException("void UDPSocket::connect(const String &host, int port): host is empty");
	if (!port)
		throw IllegalArgumentException("void UDPSocket::connect(const String &host, int port): port is 0");
#ifdef _WIN32
	SOCKET sockfd;
#else
	int sockfd = 0;
#endif
	int n;
	struct addrinfo hints, *res, *ressave;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	char portstr[10];
	sprintf(portstr, "%i", port);
	if ((n = getaddrinfo((const char*) host, portstr, &hints, &res)) != 0)
		throwExceptionFromEaiError(n, ToString("UDPSocket::connect: host=%s, port=%i", (const char*) host, port));
	ressave = res;
	int e = 0, conres = 0;
	do {
		if (SourceInterface.size() > 0 || SourcePort > 0) {
			try {
				sockfd = out_bind((const char*) SourceInterface, SourcePort);
			} catch (...) {
				::shutdown(sockfd, 2);
				close(sockfd);
				freeaddrinfo(ressave);
				throw;
			}
		} else {
			sockfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (sockfd < 0)
				continue; // Error, try next one
		}
		if (connect_timeout_sec > 0 || connect_timeout_usec) {
#ifdef _WIN32
			conres=::connect(sockfd,res->ai_addr,res->ai_addrlen);
#else
			conres = ppl_connect_nb(sockfd, res->ai_addr, res->ai_addrlen, connect_timeout_sec, connect_timeout_usec);
			e = errno;
#endif
		} else {
			conres = ::connect(sockfd, res->ai_addr, res->ai_addrlen);
			e = errno;
		}
		if (conres == 0)
			break;
#ifdef _WIN32
		e=WSAGetLastError();
		::shutdown(sockfd,2);
		closesocket(sockfd);
#else
		::shutdown(sockfd, 2);
		close(sockfd);
#endif
		sockfd = 0;
	} while ((res = res->ai_next) != NULL);
	if (conres < 0)
		res = NULL;
	if (sockfd < 0) {
		freeaddrinfo(ressave);
		throw CouldNotOpenSocketException(ToString("Host: %s, Port: %d", (const char*) host, port));
	}
	if (res == NULL) {
		freeaddrinfo(ressave);
		throwExceptionFromErrno(e, ToString("Host: %s, Port: %d", (const char*) host, port));
	}
	s->sd = sockfd;
	//HostName = host;
	//PortNum = port;
	connected = true;
	freeaddrinfo(ressave);
}

/*!\brief Prüfen, ob eine Verbindung besteht
 *
 * \desc
 * Mit dieser Funktion kann überprüft werden, ob eine TCP-Verbindung besteht.
 *
 * @return Liefert 1 zurück, wenn eine Verbindung besteht, sonst 0. Es wird kein Fehlercode gesetzt.
 */
bool UDPSocket::isConnected() const
{
	return connected;
}

/*!\brief Lese-Timeout festlegen
 *
 * Mit dieser Funktion kann ein Timeout für Lesezugriffe gesetzt werden. Normalerweise
 * blockiert eine Leseoperation mit "Read" solange, bis die angeforderten Daten
 * eingegangen sind (ausser der Socket wurde mit TCPSocket::setBlocking auf "Non-Blocking"
 * gesetzt). Mit dieser Funktion kann jedoch ein beliebiger mikrosekunden genauer
 * Timeout festgelegt werden. Der Timeout errechnet sich dabei aus
 * \p seconds + \p useconds.
 * \par
 * Um den Timeout wieder abzustellen, kann die Funktion mit 0 als
 * Wert für \p seconds und \p useconds aufgerufen werden.
 *
 * @param[in] seconds Anzahl Sekunden
 * @param[in] useconds Anzahl Mikrosekunden (1000000 Mikrosekunden = 1 Sekunde)
 * @exception NotConnectedException
 * @exception InvalidSocketException
 * @exception BadFiledescriptorException
 * @exception UnknownOptionException
 * @exception BadAddressException
 * @exception InvalidArgumentsException
 */
void UDPSocket::setTimeoutRead(int seconds, int useconds)
{
	if (!connected)
		throw NotConnectedException();
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = useconds;
	PPLSOCKET *s = (PPLSOCKET*) socket;
#ifdef WIN32
	if (setsockopt(s->sd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv))!=0) {
#else
	if (setsockopt(s->sd, SOL_SOCKET, SO_RCVTIMEO, (void*) &tv, sizeof(tv)) != 0) {
#endif
		throwExceptionFromErrno(errno, "setTimeoutRead");
	}
}

/*!\brief Schreib-Timeout festlegen
 *
 * Mit dieser Funktion kann ein Timeout für Schreibzugriffe gesetzt werden. Normalerweise
 * blockiert eine Schreiboperation mit "Write" solange, bis alle Daten gesendet wurden.
 * Mit dieser Funktion kann jedoch ein beliebiger mikrosekunden genauer
 * Timeout festgelegt werden. Der Timeout errechnet sich dabei aus
 * \p seconds + \p useconds.
 * \par
 * Um den Timeout wieder abzustellen, kann die Funktion mit 0 als
 * Wert für \p seconds und \p useconds aufgerufen werden.
 *
 * @param[in] seconds Anzahl Sekunden
 * @param[in] useconds Anzahl Mikrosekunden (1000000 Mikrosekunden = 1 Sekunde)
 * @exception NotConnectedException
 * @exception InvalidSocketException
 * @exception BadFiledescriptorException
 * @exception UnknownOptionException
 * @exception BadAddressException
 * @exception InvalidArgumentsException
 */
void UDPSocket::setTimeoutWrite(int seconds, int useconds)
{
	if (!connected)
		throw NotConnectedException();
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = useconds;
	PPLSOCKET *s = (PPLSOCKET*) socket;
#ifdef WIN32
	if (setsockopt(s->sd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&tv,sizeof(tv))!=0) {
#else
	if (setsockopt(s->sd, SOL_SOCKET, SO_SNDTIMEO, (void*) &tv, sizeof(tv)) != 0) {
#endif
		throwExceptionFromErrno(errno, "setTimeoutRead");
	}
}

/*!\brief Auf eingehende Daten warten
 *
 * \desc
 * Diese Funktion prüft, ob Daten eingegangen sind. Ist dies der Fall,
 * kehrt sie sofort wieder zurück. Andernfalls wartet sie solange, bis
 * Daten eingehen, maximal aber die mit \p seconds und \p useconds
 * angegebene Zeitspanne. Falls \p seconds und \p useconds Null sind, und
 * keine Daten bereitstehen, kehrt die Funktion sofort zurück.
 *
 * @param[in] seconds Anzahl Sekunden, die gewartet werden soll
 * @param[in] useconds Anzahl Mikrosekunden, die gewartet werden soll
 * @return Die Funktion gibt \b true zurück, wenn Daten zum Lesen bereitstehen,
 * sonst \b false. Im Fehlerfall wird eine Exception geworfen.
 * @exception Diverse
 */
bool UDPSocket::waitForIncomingData(int seconds, int useconds)
{
	if (!connected) throw NotConnectedException();
	PPLSOCKET *s=(PPLSOCKET*)socket;
	fd_set rset, wset, eset;
	struct timeval timeout;

	timeout.tv_sec=seconds;
	timeout.tv_usec=useconds;

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);
	FD_SET(s->sd,&rset); // Wir wollen nur prüfen, ob was zu lesen da ist
	int ret=select(s->sd+1,&rset,&wset,&eset,&timeout);
	if (ret<0) {
		throwExceptionFromErrno(errno, "UDPSocket::waitForIncomingData");
	}
	if (FD_ISSET(s->sd,&eset)) {
		throw OutOfBandDataReceivedException("UDPSocket::waitForIncomingData");
	}
	// Falls Daten zum Lesen bereitstehen, könnte dies auch eine Verbindungstrennung anzeigen
	if (FD_ISSET(s->sd,&rset)) {
		char buf[2];
		ret=recv(s->sd, buf,1, MSG_PEEK|MSG_DONTWAIT);
		// Kommt hier ein Fehler zurück?
		if (ret<0) {
			throwExceptionFromErrno(errno, "UDPSocket::isReadable");
		}
		// Ein Wert von 0 zeigt an, dass die Verbindung getrennt wurde
		if (ret==0) {
			throw BrokenPipeException();
		}
		return true;
	}
	return false;
}

/*!\brief Warten, bis der Socket beschreibbar ist
 *
 * \desc
 * Diese Funktion prüft, ob Daten auf den Socket geschrieben werden können.
 * Ist dies der Fall, kehrt sie sofort wieder zurück. Andernfalls wartet
 * sie solange, bis der Socket beschrieben werden kann, maximal aber die
 * mit \p seconds und \p useconds angegebene Zeitspanne.
 * Falls \p seconds und \p useconds Null sind, und
 * keine Daten gesendet werden können, kehrt die Funktion sofort zurück.
 *
 * @param[in] seconds Anzahl Sekunden, die gewartet werden soll
 * @param[in] useconds Anzahl Mikrosekunden, die gewartet werden soll
 * @return Die Funktion gibt \b true zurück, wenn Daten geschrieben werden können,
 * sonst \b false. Im Fehlerfall wird eine Exception geworfen.
 * @exception Diverse
 *
 */
bool UDPSocket::waitForOutgoingData(int seconds, int useconds)
{
	if (!connected) throw NotConnectedException();
	PPLSOCKET *s=(PPLSOCKET*)socket;
	fd_set rset, wset, eset;
	struct timeval timeout;
	timeout.tv_sec=seconds;
	timeout.tv_usec=useconds;

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);
	FD_SET(s->sd,&wset); // Wir wollen nur prüfen, ob wir schreiben können
	int ret=select(s->sd+1,&rset,&wset,&eset,&timeout);
	if (ret<0) {
		throwExceptionFromErrno(errno, "UDPSocket::waitForOutgoingData");
	}
	if (FD_ISSET(s->sd,&eset)) {
		throw OutOfBandDataReceivedException("UDPSocket::waitForIncomingData");
	}
	if (FD_ISSET(s->sd,&wset)) {
		return true;
	}
	return false;
}

/*!\brief Bei Lesezugriffen blockieren
 *
 * \desc
 * Durch Aufruf dieser Funktion kann festgelegt werden, ob der Socket bei Lesezugriffen
 * blockieren soll. Nach dem Öffnen des Sockets ist dieser defaultmäßig so
 * eingestellt, dass er bei Lesezugriffen solange blockiert (wartet), bis Daten zur
 * Verfügung stehen. Wird er auf "Non-Blocking" gestellt, kehren die Lese-Funktionen
 * sofort mit einer Fehlermeldung zurück, wenn noch keine Daten bereitstehen.
 *
 * @param[in] value Der Wert "true" setzt den Socket in den Blocking-Modus, was auch der
 * Default ist. Durch den Wert "false" wird er in den Non-Blocking-Modus gesetzt.
 * @exception Diverse
 */
void UDPSocket::setBlocking(bool value)
{
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if((!s) || (!s->sd)) throw NotConnectedException();
	int ret=0;
#ifdef _WIN32
	u_long v;
	if (value) {
		v=0;
		ret=ioctlsocket(s->sd,FIONBIO,NULL);
	} else {
		v=1;
		ret=ioctlsocket(s->sd,FIONBIO,&v);
	}
	if (ret==0) return;
	throwExceptionFromErrno(errno, "UDPSocket::setBlocking");
#else
	if (value)
	    ret=fcntl(s->sd,F_SETFL,fcntl(s->sd,F_GETFL,0)&(~O_NONBLOCK)); // Blocking
	else
		ret=fcntl(s->sd,F_SETFL,fcntl(s->sd,F_GETFL,0)|O_NONBLOCK);// NON-Blocking
	if (ret<0) throwExceptionFromErrno(errno, "UDPSocket::setBlocking");
#endif
}



/*!\brief Socket auf eine IP-Adresse und Port binden
 *
 * \desc
 * Diese Funktion muss aufgerufen werden, bevor man mit CTCPSocket::Listen einen TCP-Server starten kann. Dabei wird mit \p host
 * die IP-Adresse festgelegt, auf die sich der Server binden soll und mit \p port der TCP-Port.
 * Es ist nicht möglich einen Socket auf mehrere Adressen zu binden.
 *
 * @param[in] host IP-Adresse, Hostname oder "*". Bei Angabe von "*" bindet sich der Socket auf alle
 * Interfaces des Servers.
 * @param[in] port Der gewünschte TCP-Port
 * @exception OutOfMemoryException
 * @exception ResolverException
 * @exception SettingSocketOptionException
 * @exception CouldNotBindToInterfaceException
 * @exception CouldNotOpenSocketException
 */
void UDPSocket::bind(const String &host, int port)
{
	//int addrlen=0;
	if (!socket) {
		socket=malloc(sizeof(PPLSOCKET));
		if (!socket) throw OutOfMemoryException();
		PPLSOCKET *s=(PPLSOCKET*)socket;
		s->sd=0;
		s->proto=6;
		s->ipname=NULL;
		s->port=port;
		//s->addrlen=0;
	}
#ifdef _WIN32
	SOCKET listenfd=0;
#else
	int listenfd=0;
#endif

	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (s->sd) disconnect();
	if (s->ipname) free(s->ipname);
	s->ipname=NULL;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	int on=1;
	// Prüfen, ob host ein Wildcard ist
	struct addrinfo *res;
	if (host.notEmpty()==true && host!="*") {
		char portstr[10];
		sprintf(portstr,"%i",port);
		int n;
		if ((n=getaddrinfo(host,portstr,&hints,&res))!=0) {
			throw ResolverException("%s, %s",(const char*)host,gai_strerror(n));
		}
		struct addrinfo *ressave=res;
		do {
			listenfd=::socket(res->ai_family,res->ai_socktype,res->ai_protocol);
			if (listenfd<0) continue; // Error, try next one
#ifdef _WIN32
			if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on))!=0) {
#else
			if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))!=0) {
#endif
				freeaddrinfo(ressave);
				throw SettingSocketOptionException();
			}
			if (::bind(listenfd,res->ai_addr,res->ai_addrlen)==0) {
				//addrlen=res->ai_addrlen;
				break;
			}
			::shutdown(listenfd,2);
#ifdef _WIN32
			closesocket(listenfd);
#else
			close(listenfd);
#endif
			listenfd=0;

		} while ((res=res->ai_next)!=NULL);
		freeaddrinfo(ressave);
	} else {
		// Auf alle Interfaces binden
		listenfd=::socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd>=0) {
			struct sockaddr_in addr;
			memset(&addr,0,sizeof(addr));
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(port);
			addr.sin_family = AF_INET;
			/* bind server port */
			if(::bind(listenfd, (struct sockaddr *) &addr, sizeof(addr))!=0) {
				::shutdown(listenfd,2);
#ifdef _WIN32
				closesocket(listenfd);
#else
				close(listenfd);
#endif
				throw CouldNotBindToInterfaceException("Host: *, Port: %d",port);
			}
			s->sd=listenfd;
			connected=1;
			return;
		}
	}
	if (listenfd<0) {
		throw CouldNotOpenSocketException("Host: %s, Port: %d",(const char*)host,port);
	}
	if (res==NULL) {
		throw CouldNotBindToInterfaceException("Host: %s, Port: %d",(const char*)host,port);
	}
	s->sd=listenfd;
	connected=1;
}

/*\brief lokale Socket-Adresse auslesen
 *
 * @desc
 * Gibt ein SockAddr-Objekt zurueck, dass die Socket-Adresse der lokalen
 * Seite des Sockets enthaelt.
 *
 * @return SockAddr
 *
 * \note Kann nur verwendet werden, wenn der Socket verbunden ist
 */
SockAddr UDPSocket::getSockAddr() const
{
	if (!connected)
		throw NotConnectedException();
	PPLSOCKET *s = (PPLSOCKET*) socket;
	struct sockaddr addr;
	socklen_t len=sizeof(addr);
	int ret=getsockname(s->sd, &addr, &len);
	if (ret<0) throwExceptionFromErrno(errno, "UDPSocket::getSockAddr");
	return ppl7::SockAddr((const void*)&addr,(size_t)len);
}

/*\brief Socket-Adresse der Gegenstelle auslesen
 *
 * @desc
 * Gibt ein SockAddr-Objekt zurueck, dass die Socket-Adresse der
 * Gegenstelle des Sockets enthaelt.
 *
 * @return SockAddr
 *
 * \note Kann nur verwendet werden, wenn der Socket verbunden ist
 */
SockAddr UDPSocket::getPeerAddr() const
{
	if (!connected)
		throw NotConnectedException();
	PPLSOCKET *s = (PPLSOCKET*) socket;
	struct sockaddr addr;
	socklen_t len=sizeof(addr);
	int ret=getpeername(s->sd, &addr, &len);
	if (ret<0) throwExceptionFromErrno(errno, "UDPSocket::getSockAddr");
	return ppl7::SockAddr((const void*)&addr,(size_t)len);
}


/*!\brief Daten schreiben
 *
 * \desc
 * Mit dieser Funktionen werden \p bytes Bytes aus dem Speicherbereich \p buffer an die Gegenstelle
 * gesendet.
 *
 * @param[in] buffer Pointer auf die zu sendenden Daten
 * @param[in] bytes Anzahl zu sendender Bytes
 * @return Wenn die Daten erfolgreich geschrieben wurden, gibt die Funktion die Anzahl geschriebener
 * Bytes zurück. Im Fehlerfall wird eine Exception geworfen.
 */
size_t UDPSocket::write(const void *buffer, size_t bytes)
{
	if (!connected)
		throw NotConnectedException();
	PPLSOCKET *s = (PPLSOCKET*) socket;
	if (!buffer)
		throw InvalidArgumentsException();
	size_t BytesWritten = 0;
	if (bytes) {
		size_t rest = bytes;
		ssize_t b = 0;
		while (rest) {
			b = ::send(s->sd, (char *) buffer, rest, 0);
			if (b > 0) {
				BytesWritten += b;
				rest -= b;
				buffer = ((const char*) buffer) + b;
			}
#ifdef WIN32
			if (b==SOCKET_ERROR) {
#else
			if (b < 0) {
#endif
				if (errno == EAGAIN) {
					waitForOutgoingData(0, 100000);
				} else {
					throwExceptionFromErrno(errno, "UDPSocket::write");
				}
			}
		}
	}
	return BytesWritten;
}

size_t UDPSocket::write(const String &str, size_t bytes)
{
	if (bytes>0 && bytes<=str.size()) {
		return write(str.getPtr(),bytes);
	}
	return write(str.getPtr(),str.size());
}

size_t UDPSocket::write(const WideString &str, size_t bytes)
{
	if (bytes>0 && bytes<=str.byteLength()) {
		return write(str.getPtr(),bytes);
	}
	return write(str.getPtr(),str.byteLength());
}

size_t UDPSocket::write(const ByteArrayPtr &bin, size_t bytes)
{
	if (bytes>0 && bytes<=bin.size()) {
		return write(bin.ptr(),bytes);
	}
	return write(bin.ptr(),bin.size());
}

size_t UDPSocket::writef(const char *fmt, ...)
{
	if (!fmt) throw IllegalArgumentException();
	String str;
	va_list args;
	va_start(args, fmt);
	str.vasprintf(fmt,args);
	va_end(args);
	return write(str);
}

#ifdef TODO

	void UDPSocket::setTimeoutRead(int seconds, int useconds)
	/*! \brief Timeout setzen
	 *
	 * \header \#include <ppl6.h>
	 * \desc
	 * Mit dieser Funktion wird der Timeout für das Empfangen von Daten gesetzt.
	 *
 * \param seconds Anzahl Sekunden
 * \param useconds Anzahl Millisekunden
 * \note Diese Funktion hat zur Zeit noch keine Auswirkungen
 * \since Wurde mit Version 6.0.19 eingeführt
 */
{
	timeout_sec=seconds;
	timeout_usec=useconds;
}
#endif

#ifdef TODO
size_t UDPSocket::sendTo(const String &host, int port, const String &buffer)
/*! \brief UDP-Packet verschicken
 *
 * \header \#include <ppl6.h>
 * \desc
 * Mit dieser Funktion wird ein UDP-Paket an den angegebenen Host verschickt.
 *
 * \param host Der Name oder die IP-Adresse des Zielrechners
 * \param port Der Port des Zielrechners
 * \param buffer Ein Pointer auf eine String-Klasse, die die zu sendenden Daten enthält
 * \returns Im Erfolgsfall liefert die Funktion die Anzahl gesendeter Bytes zurück, im Fehlerfall -1.
 * Der Fehlercode kann über die üblichen Fehler-Funktionen ausgelesen werden
 *
 * \since Wurde mit Version 6.0.19 eingeführt
 */
{
	return sendTo(host,port,(const void *)buffer.getPtr(),buffer.len());
}




size_t UDPSocket::sendTo(const String &host, int port, const void *buffer, size_t bytes)
/*! \brief UDP-Packet verschicken
 *
 * \header \#include <ppl6.h>
 * \desc
 * Mit dieser Funktion wird ein UDP-Paket an den angegebenen Host verschickt.
 *
 * \param host Der Name oder die IP-Adresse des Zielrechners
 * \param port Der Port des Zielrechners
 * \param buffer Ein Pointer auf den Puffer, der die zu sendenden Daten enthält
 * \param bytes Die Anzahl Bytes im Puffer, die gesendet werden sollen
 * \returns Im Erfolgsfall liefert die Funktion die Anzahl gesendeter Bytes zurück, im Fehlerfall -1.
 * Der Fehlercode kann über die üblichen Fehler-Funktionen ausgelesen werden
 *
 * \since Wurde mit Version 6.0.19 eingeführt
 */
{
	if (!host) {
		ppl6::SetError(194,"int UDPSocket::SendTo(==> char *host <== , int port, void *buffer, int bytes)");
		return -1;
	}
	if (!port) {
		ppl6::SetError(194,"int UDPSocket::SendTo(char *host, ==> int port <== , void *buffer, int bytes)");
		return -1;
	}
	if (!buffer) {
		ppl6::SetError(194,"int UDPSocket::SendTo(char *host, int port, ==> void *buffer <== , int bytes)");
		return -1;
	}
	if (bytes<0) {
		ppl6::SetError(194,"int UDPSocket::SendTo(char *host, int port, void *buffer, ==> int bytes <== )");
		return -1;
	}
	ppl6::CAssocArray res, *a;
	ppl6::CBinary *bin;
	if (!ppl6::GetHostByName(host,&res)) return -1;
	a=res.GetFirstArray();
	//a->List();


	int domain, type, protocol;
	domain=ppl6::atoi(a->Get("ai_family"));
	type=ppl6::atoi(a->Get("ai_socktype"));
	//protocol=ppl6::atoi(a->Get("ai_protocol"));
	struct protoent *proto=getprotobyname("UDP");
	if (!proto) {
		ppl6::SetError(395);
		return -1;
	}
	protocol=proto->p_proto;
	bin=a->GetBinary("ai_addr");
	//a->List();
	const struct sockaddr *to=(const struct sockaddr *)bin->GetPtr();
	((sockaddr_in*)to)->sin_port=htons(port);
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (!s) {
		socket=malloc(sizeof(PPLSOCKET));
		s=(PPLSOCKET*)socket;
		s->sd=-1;
	} else if ((int)s->sd>-1) {
		ppl_closesocket(s->sd);
	}
	s->sd=::socket(domain,SOCK_DGRAM,protocol);
	//sockfd=::socket(AF_INET,SOCK_DGRAM,0);
	if (s->sd<0) {
		SetSocketError();
		return -1;
	}
#ifdef _WIN32
	int ret=::sendto(s->sd,(const char*)buffer,bytes,0,to,bin->GetSize());
#else
	ssize_t ret=::sendto(s->sd,(const void*)buffer,(size_t)bytes,0,to,(socklen_t) bin->GetSize());
#endif
	if (ret<0) {
		printf ("ret: %i\n",(int)ret);
		SetSocketError();
		return -1;
	}
	//close(sockfd);
	return ret;
}



int UDPSocket::RecvFrom(CString &buffer, int maxlen)
/*! \brief UDP-Packet empfangen
 *
 * \header \#include <ppl6.h>
 * \desc
 * Diese Funktion wartet auf ein UDP-Packet
 *
 * \param buffer Ein Pointer auf eine String-Klasse, in die die Daten geschrieben werden sollen
 * \param maxlen Die maximale Anzahl Bytes, die in den Puffer geschrieben werden können
 * \returns Im Erfolgsfall liefert die Funktion die Anzahl gelesener Bytes zurück, im Fehlerfall -1.
 * Der Fehlercode kann über die üblichen Fehler-Funktionen ausgelesen werden
 *
 * \since Wurde mit Version 6.0.19 eingeführt
 */
{
	char *b=(char*)malloc(maxlen+1);
	if (!b) {
		SetError(2);
		return 0;
	}
	int ret=RecvFrom((void*)b,maxlen);
	buffer.ImportBuffer(b,maxlen);
	return ret;
}

int UDPSocket::SetReadTimeout(int seconds, int useconds)
{
	struct timeval tv;
	tv.tv_sec=seconds;
	tv.tv_usec=useconds;
	PPLSOCKET *s=(PPLSOCKET*)socket;
    if (!s) {
        SetError(275);
        return 0;
    }
#ifdef WIN32
	if (setsockopt(s->sd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv))!=0) {
#else
	if (setsockopt(s->sd,SOL_SOCKET,SO_RCVTIMEO,(void*)&tv,sizeof(tv))!=0) {
#endif
		SetError(350);
		return 0;
	}
	return 1;
}


int UDPSocket::RecvFrom(void *buffer, int maxlen)
/*! \brief UDP-Packet empfangen
 *
 * \header \#include <ppl6.h>
 * \desc
 * Diese Funktion wartet auf ein UDP-Packet
 *
 * \param buffer Ein Pointer auf den Puffer, in den die Daten geschrieben werden sollen
 * \param maxlen Die maximale Anzahl Bytes, die in den Puffer geschrieben werden können
 * \returns Im Erfolgsfall liefert die Funktion die Anzahl gelesener Bytes zurück, im Fehlerfall -1.
 * Der Fehlercode kann über die üblichen Fehler-Funktionen ausgelesen werden
 *
 * \since Wurde mit Version 6.0.19 eingeführt
 */
{
	struct sockaddr from;
#ifdef _WIN32
	int len;
#else
	socklen_t len=0;
#endif
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (!s) {
		socket=malloc(sizeof(PPLSOCKET));
		s=(PPLSOCKET*)socket;
		s->sd=-1;
	}
	if (s->sd<0) s->sd=::socket(AF_INET,SOCK_DGRAM,0);

#ifdef _WIN32
	len=sizeof(struct sockaddr);
	int ret=::recvfrom(s->sd,(char*)buffer,maxlen,0,&from,&len);
	//int ret=::recvfrom(s->sd,(char*)buffer,maxlen,0,NULL,0);
#else
	int ret=::recvfrom(s->sd,buffer,maxlen,0,&from,&len);
#endif
	if (ret<0) {
		SetSocketError();
		return -1;
	}

	return ret;
}

int UDPSocket::GetDescriptor()
{
	if (!socket) return 0;
	PPLSOCKET *s=(PPLSOCKET*)socket;
#ifdef _WIN32
	return (int)s->sd;
#else
	return s->sd;
#endif
}


int UDPSocket::RecvFrom(void *buffer, int maxlen, CString &host, int *port)
/*! \brief UDP-Packet empfangen
 *
 * \header \#include <ppl6.h>
 * \desc
 * Diese Funktion wartet auf ein UDP-Packet
 *
 * \param buffer Ein Pointer auf den Puffer, in den die Daten geschrieben werden sollen
 * \param maxlen Die maximale Anzahl Bytes, die in den Puffer geschrieben werden können
 * \param host Pointer auf einen String, in dem die Absender-IP des Pakets gespeichert wird
 * \param port Port des Absenders
 * \returns Im Erfolgsfall liefert die Funktion die Anzahl gelesener Bytes zurück, im Fehlerfall -1.
 * Der Fehlercode kann über die üblichen Fehler-Funktionen ausgelesen werden
 *
 * \since Wurde mit Version 6.2.3 eingeführt
 */
{
	struct sockaddr_in from;
#ifdef _WIN32
	int len;
#else
	socklen_t len=0;
#endif
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (!s) {
		socket=malloc(sizeof(PPLSOCKET));
		s=(PPLSOCKET*)socket;
		s->sd=-1;
	}
	if (s->sd<0) s->sd=::socket(AF_INET,SOCK_DGRAM,0);

#ifdef _WIN32
	len=sizeof(struct sockaddr);
	int ret=::recvfrom(s->sd,(char*)buffer,maxlen,0,(sockaddr*)&from,&len);
	//int ret=::recvfrom(s->sd,(char*)buffer,maxlen,0,NULL,0);
#else
	int ret=::recvfrom(s->sd,buffer,maxlen,0,(struct sockaddr *)&from,&len);
#endif
	if (ret<0) {
		SetSocketError();
		return -1;
	}
	char hostname[1024];
	char servname[32];
	bzero(hostname,1024);
	bzero(servname,32);
	ppl6::HexDump(&from,sizeof(struct sockaddr));
	if (getnameinfo((const sockaddr*)&from,len,
		hostname,1023, servname,31,NI_NUMERICHOST|NI_NUMERICSERV)!=0) {
		host.Set("");
		*port=0;
	} else {
		host.Set(hostname);
		*port=atoi(servname);
	}

	return ret;
}


int UDPSocket::Bind(const char *host, int port)
{
	int addrlen=0;
	if (!socket) {
		socket=malloc(sizeof(PPLSOCKET));
		if (!socket) {
			SetError(2);
			return 0;
		}
		PPLSOCKET *s=(PPLSOCKET*)socket;
    	s->sd=0;
		s->proto=6;
		s->ipname=NULL;
		s->port=port;
		//s->addrlen=0;
	}
	PPLSOCKET *s=(PPLSOCKET*)socket;
	//if (s->sd) Disconnect();
	if (s->ipname) free(s->ipname);
	s->ipname=NULL;

	struct addrinfo hints, *res, *ressave;
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	int n;
	int on=1;
	#ifdef _WIN32
		SOCKET	listenfd;
	#else
		int listenfd;
	#endif
	// Prüfen, ob host ein Wildcard ist
	if (host!=NULL && strlen(host)==0) host=NULL;
	if (host!=NULL && host[0]=='*') host=NULL;

	if (host) {
		char portstr[10];
		sprintf(portstr,"%i",port);
		if ((n=getaddrinfo(host,portstr,&hints,&res))!=0) {
			SetError(273,"%s, %s",host,gai_strerror(n));
			return 0;
		}
		ressave=res;
		do {
			listenfd=::socket(res->ai_family,res->ai_socktype,res->ai_protocol);
			if (listenfd<0) continue;		// Error, try next one
			if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&on,sizeof(on))!=0) {
				freeaddrinfo(ressave);
				SetError(334);
				return 0;
			}
			if (::bind(listenfd,res->ai_addr,res->ai_addrlen)==0) {
				addrlen=res->ai_addrlen;
				break;
			}
			shutdown(listenfd,2);
			#ifdef _WIN32
				closesocket(listenfd);
			#else
				close(listenfd);
			#endif
			listenfd=0;

		} while ((res=res->ai_next)!=NULL);
		freeaddrinfo(ressave);
	} else {
		listenfd=::socket(AF_INET, SOCK_DGRAM, 0);
		if (listenfd>=0) {
			struct sockaddr_in addr;
			bzero(&addr,sizeof(addr));
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(port);
			addr.sin_family = AF_INET;
		    /* bind server port */
    		if(::bind(listenfd, (struct sockaddr *) &addr, sizeof(addr))!=0) {
				shutdown(listenfd,2);
				#ifdef _WIN32
					closesocket(listenfd);
				#else
					close(listenfd);
				#endif
				SetError(266);
				return 0;
    		}
    		s->sd=listenfd;
    		//s->addrlen=0;
    		//connected=1;
    		return 1;
		}
	}
	if (listenfd<0) {
		SetError(265,"Host: %s, Port: %d",host,port);
		return 0;
	}
	if (res==NULL) {
		SetError(266,"Host: %s, Port: %d",host,port);
		return 0;
	}
	s->sd=listenfd;
	//s->addrlen=addrlen;
	//connected=1;
	return 1;
}

#endif


int UDPSocket::receiveConnect(UDPSocket *socket, const String &host, int port)
{
	return 0;
}

}	// EOF ppl7
