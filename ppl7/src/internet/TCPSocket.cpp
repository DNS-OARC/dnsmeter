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

//#define DEBUGOUT

#include "ppl7.h"
#include "ppl7-inet.h"
#include "socket.h"

namespace ppl7 {

/*!\class TCPSocket
 * \ingroup PPLGroupInternet
 *
 * \brief Socket-Klasse für TCP-Verbindungen
 *
 * \header \#include <ppl7-inet.h>
 *
 * \desc
 * Mit dieser Klasse kann eine TCP-Verbindung (Transmission Control Protocol) zu einem über IP
 * erreichbaren Server aufgebaut oder ein eigener Server gestartet werden. Über eine
 * bestehende Verbindung können Daten gesenden und empfangen werden. Mit Hilfe der Klasse CSSL
 * und der OpenSSL-Bibliothek können die Daten auch verschlüsselt übertragen werden.
 *
 * \example TCP-Client
 * Das nachfolgende Beispiel zeigt einen einfachen Client, der eine Verbindung
 * zu einem Server öffnet, ein Datenpaket sendet und auf ein Datenpaket
 * wartet.
 * \dontinclude socket_examples.cpp
 * \skip Socket_Example1
 * \until EOF
 *
 * \example TCP-Server
 * Das zweite Beispiel zeigt, wie ein einfacher TCP-Server programmiert
 * wird. Zunächst wird ein Server gestartet, der auf eingehende
 * Verbindungen wartet. Nach Verbindungseingang wird auf Datenpakete
 * gewartet, die sofort wieder an den Client zurückgeschickt werden. Durch Abbruch
 * der Verbindung wird die Schleife wieder beendet.
 * \dontinclude socket_examples.cpp
 * \skip Socket_Example2
 * \until EOF
 * \par
 * Ein weiteres Beispiel mit einer verschlüsselten Datenübertragung ist in der Dokumentation
 * der Klasse CSSL zu finden.
 *
 */

// Compat-Funktionen
#ifdef _WIN32
#define socklen_t	int
#endif

void throwExceptionFromEaiError(int ecode, const String &msg)
{
#ifndef _WIN32
	if (ecode == EAI_SYSTEM)
		throwExceptionFromErrno(errno, msg);
#endif
	String m = msg;
	if (msg.notEmpty())
		m += " [";
	m += "ResolverException: ";
	m += gai_strerror(ecode);
	if (msg.notEmpty())
		m += "]";
	throw ResolverException(m);
}

/*!\brief Eingehende Verbindung verarbeiten
 *
 * \desc
 * Diese virtuelle Funktion wird innerhalb von CTCPSocket::Listen aufgerufen,
 * wenn eine neue Verbindung zum Server aufgebaut wurde. Sie muss daher von
 * der abgeleiteten Klasse reimplementiert werden.
 *
 * @param[in] socket Pointer auf eine neue Socket-Klasse, die die eingegangene
 * Verbindung repräsentiert. Die Anwendung muss die Klasse nach Verbindungsende
 * selbst mit "delete" löschen.
 * @param[in] host Hostname oder IP der Gegenstelle
 * @param[in] port TCP-Port der Gegenstelle
 * @return Wird die Verbindung angenommen und von der Anwendung weiter
 * verarbeitet (z.B. in einem eigenen Thread), muss die Funktion 1
 * zurückgeben. Soll die Verbindung wieder getrennt werden, muss die
 * Funktion 0 zurückgeben.
 */
int TCPSocket::receiveConnect(TCPSocket *socket, const String &host, int port)
{
	return 0;
}

/*!\brief Konstruktor der Klasse
 *
 *
 */
TCPSocket::TCPSocket()
{
#ifdef _WIN32
	InitSockets();
#endif
	PortNum=0;
	socket = NULL;
	connected = false;
	islisten = false;
	stoplisten = false;
	blocking=true;
	ssl = NULL;
	sslcontext = NULL;
	connect_timeout_sec = 0;
	connect_timeout_usec = 0;
	SourcePort = 0;
#ifndef _WIN32
	// signal SIGPIPE ignorieren
	signal(SIGPIPE, SIG_IGN);
#endif
}

/*!\brief Destruktor der Klasse
 *
 *
 */
TCPSocket::~TCPSocket()
{
	sslStop();
	if (connected)
		disconnect();
	if (islisten)
		disconnect();
	PPLSOCKET *s = (PPLSOCKET*) socket;
	if (!s)
		return;
	if (s->sd)
		disconnect();
	if (s->ipname)
		free(s->ipname);
	free(s);
	socket = NULL;
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
void TCPSocket::setSource(const String &interface, int port)
{
	SourceInterface = interface;
	SourcePort = port;
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
int TCPSocket::getDescriptor()
{
	if (!connected)
		throw NotConnectedException();
	PPLSOCKET *s = (PPLSOCKET*) socket;
#ifdef _WIN32
	return (int)s->sd;
#else
	return s->sd;
#endif
}

/*!\brief Verbindung trennen
 *
 * \desc
 * Durch Aufruf dieser Funktion wird eine bestehende Verbindung beendet. Unter Windows ist die Funktion
 * identisch mit CTCPSocket::shutdown, unter Unix unterscheiden sie sich.
 * \par
 * TCPSocket::disconnect schließt den Socket-Descriptor für den Prozess, die Verbindung bleibt
 * jedoch weiterhin bestehen, wenn ein anderer Prozess den gleichen Descriptor verwendet.
 * Die Verbindung bleibt dann sowohl zum Lesen als auch zum Schreiben geöffnet.
 *
 * \par
 * TCPSocket::shutdown trennt die Verbindung für alle Prozesse, die den gleichen Descriptor
 * verwenden. Falls ein anderer Prozess noch versucht auf den Socket zuzzugreifen, bekommt er eine
 * Fehlermeldung. Beim Lesen ist dies meist ein EOF, beim Schreiben ein SIGPIPE, der möglicherweise
 * Aufgrund von Puffern im Kernel nicht sofort auftreten muss.
 * @exception
 */
void TCPSocket::disconnect()
{
	PPLSOCKET *s = (PPLSOCKET*) socket;
	if (!s)
		return;
	if (islisten) {
		stopListen();
	}
	sslStop();
	//if (s->sd>0) shutdown(s->sd,2);
	connected = false;
	if (s->sd > 0) {
#ifdef _WIN32
		closesocket(s->sd);
#else
		close(s->sd);
#endif
		s->sd = 0;
	}
	HostName.clear();
	PortNum = 0;
	islisten = false;
}

/*!\brief Verbindung trennen
 *
 * \desc
 * Durch Aufruf dieser Funktion wird eine bestehende Verbindung beendet. Unter Windows ist die Funktion
 * identisch mit CTCPSocket::shutdown, unter Unix unterscheiden sie sich.
 * \par
 * TCPSocket::disconnect schließt den Socket-Descriptor für den Prozess, die Verbindung bleibt
 * jedoch weiterhin bestehen, wenn ein anderer Prozess den gleichen Descriptor verwendet.
 * Die Verbindung bleibt dann sowohl zum Lesen als auch zum Schreiben geöffnet.
 *
 * \par
 * TCPSocket::shutdown trennt die Verbindung für alle Prozesse, die den gleichen Descriptor
 * verwenden. Falls ein anderer Prozess noch versucht auf den Socket zuzzugreifen, bekommt er eine
 * Fehlermeldung. Beim Lesen ist dies meist ein EOF, beim Schreiben ein SIGPIPE, der möglicherweise
 * Aufgrund von Puffern im Kernel nicht sofort auftreten muss.
 */
void TCPSocket::shutdown()
{
	PPLSOCKET *s = (PPLSOCKET*) socket;
	if (!s)
		return;
	if (islisten) {
		stopListen();
	}
	sslStop();
	//if (s->sd>0) shutdown(s->sd,2);
	connected = false;
	if (s->sd > 0) {
#ifdef _WIN32
		closesocket(s->sd);
#else
		::shutdown(s->sd, SHUT_RDWR);
#endif
		s->sd = 0;
	}
	HostName.clear();
	PortNum = 0;
	islisten = false;
}

/*!\brief TCP-Server anhalten
 *
 * \desc
 * Durch Aufruf dieser Funktion innerhalb eines anderen Threads wird dem Server mitgeteilt, dass er nicht
 * weiter auf Verbindungseingänge warten soll. Die Funktion kehrt dabei erst zurück, wenn der Server sich
 * beendet hat. Falls man darauf nicht warten möchte, kann stattdessen auch CTCPSocket::SignalStopListen
 * aufgerufen werden.
 *
 */
void TCPSocket::stopListen()
{
	mutex.lock();
	stoplisten = true;
	while (islisten) {
		mutex.unlock();
		MSleep(1);
		mutex.lock();
	}
	stoplisten = false;
	mutex.unlock();
}

/*!\brief TCP-Server signalisieren, dass er stoppen soll
 *
 * \desc
 * Durch Aufruf dieser Funktion innerhalb eines anderen Threads wird dem Server mitgeteilt, dass er nicht
 * weiter auf Verbindungseingänge warten soll. Die Funktion kehrt dabei sofort zurück und es liegt an der
 * Anwendung später zu prüfen, ob der Server wirklich gestoppt wurde (z.B. mit CTCPSocket::StopListen).
 *
 */
void TCPSocket::signalStopListen()
{
	mutex.lock();
	stoplisten = true;
	mutex.unlock();
}

/*!\brief Timeout für Connect-Funktion definieren
 *
 * \desc
 * Mit dieser Funktion kann ein Timeout für die Connect-Funktionen definiert werden. Ist es nicht
 * möglich innerhalb der vorgegebenen Zeit eine Verbindung mit dem Ziel herzustellen, brechen die
 * Connect-Funktionen mit einer Timeout-Fehlermeldung ab.
 * \par
 * Um den Timeout wieder abzustellen, kann die Funktion mit 0 als Wert für seconds und useconds aufgerufen werden.
 *
 * @param[in] seconds Anzahl Sekunden
 * @param[in] useconds Anzahl Mikrosekunden (1000 Mikrosekunden=1 Millisekunde, 1000 Millisekunden = 1 Sekunde)
 */
void TCPSocket::setTimeoutConnect(int seconds, int useconds)
{
	connect_timeout_sec = seconds;
	connect_timeout_usec = useconds;
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
	hints.ai_socktype = SOCK_STREAM;
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
		throwExceptionFromEaiError(n, ToString("TCPSocket bind connect: %s:%i", host, port));
	}
	ressave = res;
	do {
		listenfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue; // Error, try next one

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
			freeaddrinfo(ressave);
			throwExceptionFromErrno(errno, ToString("TCPSocket bind connect: %s:%i", host, port));
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
 * \desc
 * Mit dieser Funktion wird eine Verbindung zur angegebenen Adresse aufgebaut.
 *
 * \param host Der Parameter \p host_and_port muss das Format "hostname:port" haben, wobei
 * "hostname" auch eine IP-Adresse sein kann. Der Port kann entweder als Zahl oder
 * als Servicename angegeben werden, z.B. "smtp" für Port 25.
 *
 * @exception IllegalArgumentException Wird geworfen, wenn der Parameter \p host_and_port
 * keinen Wert oder keinen oder mehr als einen Doppelpunkt enthält
 * @exception IllegalPortException wird geworfen, wenn der angegebene Port oder
 * Servicename ungültig ist.
 */
void TCPSocket::connect(const String &host_and_port)
{
	if (host_and_port.isEmpty())
		throw IllegalArgumentException("TCPSocket::connect(const String &host_and_port)");
	Array hostname = StrTok(host_and_port, ":");
	if (hostname.size() != 2)
		throw IllegalArgumentException("TCPSocket::connect(const String &host_and_port)");
	String portname = hostname.get(1);
	int port = portname.toInt();
	if (port <= 0 && portname.size() > 0) {
		// Vielleicht wurde ein Service-Namen angegeben?
		struct servent *s = getservbyname((const char*) portname, "tcp");
		if (s) {
			unsigned short int p = s->s_port;
			port = (int) ntohs(p);
		} else {
			throw IllegalPortException("TCPSocket::connect(const String &host_and_port=%s)", (const char*) host_and_port);
		}
	}
	if (port <= 0)
		throw IllegalPortException("TCPSocket::connect(const String &host_and_port=%s)", (const char*) host_and_port);
	return connect(hostname.get(0), port);
}

/*!\fn TCPSocket::connect(const String &host, int port)
 * \brief Verbindung aufbauen
 *
 * \desc
 * Mit dieser Funktion wird eine Verbindung zur angegebenen Adresse aufgebaut.
 *
 * \param[in] host Der Hostname oder die IP-Adresse des Zielrechners
 * \param[in] port Der gewünschte Zielport
 *
 * @exception IllegalArgumentException Wird geworfen, wenn einer der beiden Parameter
 * keinen oder einen ungültigen Wert enthält
 * @exception IllegalPortException wird geworfen, wenn der angegebene Port ungültig ist.
 */

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


void TCPSocket::connect(const String &host, int port)
{
	if (connected)
		disconnect();
	if (islisten)
		disconnect();
	//BytesWritten=0;
	//BytesRead=0;
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
		throw IllegalArgumentException("void TCPSocket::connect(const String &host, int port): host is empty");
	if (!port)
		throw IllegalArgumentException("void TCPSocket::connect(const String &host, int port): port is 0");
#ifdef _WIN32
	SOCKET sockfd;
#else
	int sockfd = 0;
#endif
	int n;
	struct addrinfo hints, *res, *ressave;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	char portstr[10];
	sprintf(portstr, "%i", port);
	if ((n = getaddrinfo((const char*) host, portstr, &hints, &res)) != 0)
		throwExceptionFromEaiError(n, ToString("TCPSocket::connect: host=%s, port=%i", (const char*) host, port));
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
	//s->addrlen=res->ai_addrlen;
	HostName = host;
	PortNum = port;
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
bool TCPSocket::isConnected() const
{
	return connected;
}

/*!\brief Prüfen, ob der Socket auf eingehende Verbindungen wartet
 *
 * \desc
 * Mit dieser Funktion kann überprüft werden, ob der Socket auf eingehende TCP-Verbindungen wartet.
 *
 * @return Liefert 1 zurück, wenn der Socket wartet, sonst 0. Es wird kein Fehlercode gesetzt.
 */
bool TCPSocket::isListening() const
{
	return islisten;
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
void TCPSocket::setTimeoutRead(int seconds, int useconds)
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
void TCPSocket::setTimeoutWrite(int seconds, int useconds)
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
size_t TCPSocket::write(const void *buffer, size_t bytes)
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
			if (ssl)
				b = SSL_Write(buffer, rest);
			else
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
					throwExceptionFromErrno(errno, "TCPSocket::write");
				}
			}
		}
	}
	return BytesWritten;
}

size_t TCPSocket::write(const String &str, size_t bytes)
{
	if (bytes>0 && bytes<=str.size()) {
		return write(str.getPtr(),bytes);
	}
	return write(str.getPtr(),str.size());
}

size_t TCPSocket::write(const WideString &str, size_t bytes)
{
	if (bytes>0 && bytes<=str.byteLength()) {
		return write(str.getPtr(),bytes);
	}
	return write(str.getPtr(),str.byteLength());
}

size_t TCPSocket::write(const ByteArrayPtr &bin, size_t bytes)
{
	if (bytes>0 && bytes<=bin.size()) {
		return write(bin.ptr(),bytes);
	}
	return write(bin.ptr(),bin.size());
}

size_t TCPSocket::writef(const char *fmt, ...)
{
	if (!fmt) throw IllegalArgumentException();
	String str;
	va_list args;
	va_start(args, fmt);
	str.vasprintf(fmt,args);
	va_end(args);
	return write(str);
}

/*!\brief Daten lesen
 *
 * \desc
 * Diese Funktion versucht \p bytes vom Socket in den von der Anwendung
 * angegebenen Puffer \p buffer zu lesen. Falls der Socket auf "blocking" eingestellt
 * ist (=Default) wartet die Funktion solange, bis die ersten Daten eingegangen sind
 * und liefert diese zurück. Ist der Socket auf "non blocking" eingestellt, kehrt die
 * Funktion sofort zurück, wenn keine Daten zur Verfügung stehen.
 * Die Anzahl tatsächlich gelesener Bytes wird als Rückgabewert zurückgegeben.
 *
 * @param[in] buffer Speicherbereich, in dem die gelesenen Daten abgelegt
 * werden sollen. Dieses muss gross genug sein, um die erwartete Anzahl Bytes aufnehmen
 * zu können.
 * @param[in] bytes Anzahl zu lesender Bytes
 * @return Die Funktion gibt die Anzahl tatsächlich gelesener Bytes zurück.
 * Im Fehlerfall wird eine Exception geworfen.
 * @see TCPSocket::setBlocking Socket auf "blocking" oder "non blocking" stellen
 * @see TCPSocket::isReadable
 * @see TCPSocket::waitForIncomingData
 */
size_t TCPSocket::read(void *buffer, size_t bytes)
{
	if (!connected)
		throw NotConnectedException();
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (!buffer) throw IllegalArgumentException();
	ssize_t BytesRead=0;
	if (ssl) {
		BytesRead=SSL_Read(buffer,bytes);
	} else {
		BytesRead=::recv(s->sd,(char*)buffer,bytes,0);
		if (BytesRead<0)
			throwExceptionFromErrno(errno, "TCPSocket::read");
	}
	return ((size_t)BytesRead);
}

size_t TCPSocket::read(String &buffer, size_t bytes)
{
	char * readbuffer=(char*)malloc(bytes);
	if (!readbuffer) throw OutOfMemoryException();
	try {
		size_t BytesRead=read((void*)readbuffer,bytes);
		buffer.set(readbuffer,BytesRead);
		free(readbuffer);
		return BytesRead;
	} catch (...) {
		free(readbuffer);
		throw;
	}
}

size_t TCPSocket::read(ByteArray &buffer, size_t bytes)
{
	char * readbuffer=(char*)malloc(bytes);
	if (!readbuffer) throw OutOfMemoryException();
	try {
		size_t BytesRead=read((void*)readbuffer,bytes);
		buffer.copy(readbuffer,BytesRead);
		free(readbuffer);
		return BytesRead;
	} catch (...) {
		free(readbuffer);
		throw;
	}
}


void TCPSocket::readLoop(void *buffer, size_t bytes, int timeout_seconds, Thread *watch_thread)
{
	size_t todo=bytes;
	unsigned char *ptr=(unsigned char*)buffer;
	ppluint64 timeout=ppl7::GetMilliSeconds()+timeout_seconds*1000;
	while (todo>0) {
		if (watch_thread) {
			if (watch_thread->threadShouldStop()) throw OperationAbortedException("TCPSocket::readLoop");
		}
		if (waitForIncomingData(0,200000)) {
			size_t recevied=read(ptr,todo);
			todo-=recevied;
			ptr+=recevied;
		}
		if (timeout_seconds>0 && ppl7::GetMilliSeconds()>timeout) throw ppl7::TimeoutException("TCPSocket::readLoop");
	}
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
void TCPSocket::setBlocking(bool value)
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
	if (ret==0) {
		blocking=value;
		return;
	}
	throwExceptionFromErrno(errno, "TCPSocket::setBlocking");
#else
	if (value)
	    ret=fcntl(s->sd,F_SETFL,fcntl(s->sd,F_GETFL,0)&(~O_NONBLOCK)); // Blocking
	else
		ret=fcntl(s->sd,F_SETFL,fcntl(s->sd,F_GETFL,0)|O_NONBLOCK);// NON-Blocking
	if (ret<0) throwExceptionFromErrno(errno, "TCPSocket::setBlocking");
	blocking=value;
#endif
}

/*!\brief Prüfen, ob der Socket im Blocking- oder Non-Blocking-Mode ist
 *
 * \desc
 * Prüfen, ob der Socket im Blocking- oder Non-Blocking-Mode ist
 *
 * @return Liefert \b true zurück, wenn der Socket im Blocking-Mode ist (was
 * der Default ist), \b false wenn nicht.
 */
bool TCPSocket::isBlocking() const
{
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if((!s) || (!s->sd)) throw NotConnectedException();
	return blocking;
}


/*!\brief Prüfen, ob Daten auf den Socket geschrieben werden können
 *
 * \desc
 * Diese Funktion prüft, ob der bereits verbundene Socket beschreibbar ist.
 *
 * @return Liefert \b true zurück, wenn Daten auf den Socket geschrieben werden können,
 * ansonsten \b false.
 * @exception Diverse Im Fehlerfall wird eine Exception geworfen
 */
bool TCPSocket::isWriteable()
{
	PPLSOCKET *s=(PPLSOCKET*)socket;
	if (!connected) throw NotConnectedException();
	fd_set rset, wset, eset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);
	FD_SET(s->sd,&wset);
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	int ret=select(s->sd+1,&rset,&wset,&eset,&timeout);
	if (ret<0) {
		throwExceptionFromErrno(errno, "TCPSocket::isWriteable");
	}
	if (FD_ISSET(s->sd,&eset)) {
		throw OutOfBandDataReceivedException("TCPSocket::isWriteable");
	}

	if (FD_ISSET(s->sd,&wset)) {
		return true;
	}
	return false;
}

/*!\brief Prüfen, ob Daten vom Socket gelesen werden können
 *
 * \desc
 * Diese Funktion prüft, ob der bereits geöffnete Socket bereit ist,
 * um Daten zu lesen.
 *
 * @return Liefert \b true zurück, wenn vom Socket gelesen werden kann.
 * Ist dies nicht der Fall, liegt ein Fehler vor und es wird eine entsprechende
 * Exception geworfen.
 * @exception Diverse Im Fehlerfall wird eine Exception geworfen
 */
bool TCPSocket::isReadable()
{
	if (!connected) throw NotConnectedException();
	PPLSOCKET *s=(PPLSOCKET*)socket;
	fd_set rset, wset, eset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&eset);
	FD_SET(s->sd,&rset);
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	int ret=select(s->sd+1,&rset,&wset,&eset,&timeout);
	if (ret<0) {
		throwExceptionFromErrno(errno, "TCPSocket::isReadable");
	}
	if (FD_ISSET(s->sd,&eset)) {
		throw OutOfBandDataReceivedException("TCPSocket::isReadable");
	}
	// Falls Daten zum Lesen bereitstehen, könnte dies auch eine Verbindungstrennung anzeigen
	if (FD_ISSET(s->sd,&rset)) {
		char buf[2];
		ret=recv(s->sd, buf,1, MSG_PEEK|MSG_DONTWAIT);
		// Kommt hier ein Fehler zurück?
		if (ret<0) {
			throwExceptionFromErrno(errno, "TCPSocket::isReadable");
		}
		// Ein Wert von 0 zeigt an, dass die Verbindung getrennt wurde
		if (ret==0) {
			throw BrokenPipeException();
		}
	}
	return true;
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
bool TCPSocket::waitForIncomingData(int seconds, int useconds)
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
		throwExceptionFromErrno(errno, "TCPSocket::waitForIncomingData");
	}
	if (FD_ISSET(s->sd,&eset)) {
		throw OutOfBandDataReceivedException("TCPSocket::waitForIncomingData");
	}
	// Falls Daten zum Lesen bereitstehen, könnte dies auch eine Verbindungstrennung anzeigen
	if (FD_ISSET(s->sd,&rset)) {
		char buf[2];
		ret=recv(s->sd, buf,1, MSG_PEEK|MSG_DONTWAIT);
		// Kommt hier ein Fehler zurück?
		if (ret<0) {
			throwExceptionFromErrno(errno, "TCPSocket::isReadable");
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
bool TCPSocket::waitForOutgoingData(int seconds, int useconds)
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
		throwExceptionFromErrno(errno, "TCPSocket::waitForOutgoingData");
	}
	if (FD_ISSET(s->sd,&eset)) {
		throw OutOfBandDataReceivedException("TCPSocket::waitForIncomingData");
	}
	if (FD_ISSET(s->sd,&wset)) {
		return true;
	}
	return false;
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
void TCPSocket::bind(const String &host, int port)
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
	hints.ai_socktype=SOCK_STREAM;
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


/*!\brief Server starten und auf eingehende Verbindung warten
 *
 * \desc
 * Der Aufruf dieser Funktion führt dazu, dass auf eingehende TCP-Verbindungen gewartet wird.
 * Es wird also ein TCP-Server gestartet. Wird eine Verbindung geöffnet, ruft sie die virtuelle
 * Funktion CTCPSocket::ReceiveConnect auf. Diese muss entsprechend reagieren, indem Sie bei
 * Annahme der Verbindung einen neuen Thread startet und 1 zurückgibt, bei Ablehnung 0.
 * \par
 * Vor Aufruf der Funktion muss der Klasse über die Funktion CTCPSocket::Bind zuerst mitgeteilt
 * werden, auf welche IP-Adresse und Port sich der Server binden soll. Sofern kein kritischer
 * Fehler auftritt, läuft die Funktion in einer Endlosschleife. Alle \p timeout Millisekunden
 * wird jedoch geprüft, ob die Funktion sich beenden soll. Dies wird durch Aufruf der Funktion
 * CTCPSocket::SignalStopListen oder CTCPSocket::StopListen signalisiert.
 *
 * \param[in] backlog The backlog argument defines the maximum length to which the queue of pending connections for sockfd may grow.  If a connection
 *      request  arrives when the queue is full, the client may receive an error with an indication of ECONNREFUSED or, if the underly‐
 *      ing protocol supports retransmission, the request may be ignored so that a later reattempt at connection succeeds.
 *
 * \param[in] timeout Intervall in Millisekunden, nachdem geprüpft werden soll, ob
 * weiter auf eingehende Verbindungen gewartet werden soll. Je niedriger der Wert, desto
 * schneller wird auf einen Stop-Befehl reagiert, jedoch steigt dadurch auch die
 * CPU-Auslastung. Der Default ist 100 Millisekunden (1000 Millisekunden = 1 Sekunde).
 *
 * @return Konnte der Server erfolgreich gestartet werden, kehrt die Funktion erst dann wieder zurück, wenn er wieder gestoppt wird.
 * In diesem Fall liefert sie den Wert 1 zurück. Konnte der Server nicht gestartet werden, wird 0 zurückgegeben.
 *
 */
void TCPSocket::listen(int backlog, int timeout)
{
#ifdef _WIN32
	struct sockaddr_in cliAddr;
#else
	struct sockaddr cliAddr;
#endif
    fd_set rset;
    struct timeval tv;

	PPLSOCKET *s=(PPLSOCKET*)socket;
	if((!s) || (!s->sd)) throw NotConnectedException();
	stopListen();
	mutex.lock();
	stoplisten=false;
	islisten=true;
	mutex.unlock();

	// Listen
	if (::listen(s->sd,backlog)!=0) {
		int e=errno;
		mutex.lock();
		islisten=false;
		mutex.unlock();
		throwExceptionFromErrno(e, "TCPSocket::listen");
	}
	setBlocking(true);

	while (1) {
		mutex.lock();
		if (stoplisten) {
			mutex.unlock();
			break;
		}
		mutex.unlock();
		socklen_t cliLen=sizeof(cliAddr);
#ifdef TODO
		// WIN32
		SOCKET newSd;
		newSd = accept(s->sd, (struct sockaddr *) &cliAddr, &cliLen);
		if (newSd!=INVALID_SOCKET) {
			CTCPSocket *newsocket=new CTCPSocket();
			if (newsocket) newsocket->socket=malloc(sizeof(PPLSOCKET));
			if (newsocket==NULL || newsocket->socket==NULL) {
				if (newsocket) delete newsocket;
				if (log) log->Printf (LOG::ERROR,1,__FILE__,__LINE__,"Out of memory, could not handle connect from: %s:%d\n",inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port));
				SetError(2);
				continue;
			}
			newsocket->connected=1;
			PPLSOCKET *ns=(PPLSOCKET*)newsocket->socket;
			ns->proto=6;
			ns->sd=newSd;
			ns->ipname=strdup(inet_ntoa(cliAddr.sin_addr));
			ns->port=ntohs(cliAddr.sin_port);
			if (!ReceiveConnect(newsocket,ns->ipname,ns->port)) {
				delete newsocket;
			}
		} else {
			MSleep(100);
		}
#endif
		int newSd;
		// Prüfen, ob der Socket lesbar ist
		FD_ZERO(&rset);
		FD_SET(s->sd,&rset);
		tv.tv_sec=0;
		tv.tv_usec=timeout*1000;// Timeout für select setzen
		if (select(s->sd+1,&rset,NULL,NULL,&tv)<=0) continue;
		newSd = accept(s->sd, (struct sockaddr *) &cliAddr, &cliLen);
		if(newSd>0) {
			char hostname[1024];
			char servname[32];
			bzero(hostname,1024);
			bzero(servname,32);
			if (getnameinfo((const sockaddr*)&cliAddr,sizeof(cliAddr),
							hostname,1023, servname,31,NI_NUMERICHOST|NI_NUMERICSERV)!=0) {
#ifdef _WIN32
				closesocket(newSd);
#else
				close (newSd);
#endif
				continue;
			}
			TCPSocket *newsocket=new TCPSocket();
			if (newsocket) newsocket->socket=malloc(sizeof(PPLSOCKET));
			if (newsocket==NULL || newsocket->socket==NULL) {
				if (newsocket) delete newsocket;
				throw OutOfMemoryException();
			}
			newsocket->connected=true;
			PPLSOCKET *ns=(PPLSOCKET*)newsocket->socket;
			ns->proto=6;
			ns->sd=newSd;
			ns->ipname=strdup(hostname);
			ns->port=atoi(servname);

			if (!receiveConnect(newsocket,ns->ipname,ns->port)) {
				delete newsocket;
			}

		}
	}
	mutex.lock();
	islisten=false;
	mutex.unlock();
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
SockAddr TCPSocket::getSockAddr() const
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
SockAddr TCPSocket::getPeerAddr() const
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



int	TCPSocket::port() const
{
	return PortNum;
}

const String& TCPSocket::hostname() const
{
	return HostName;
}


}
 // EOF namespace ppl
