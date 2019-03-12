#include <string.h>
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
#include <math.h>
#include <errno.h>

#include "dnspecker.h"

/*!\brief Konstruktor
 *
 * Einige interne Variablen werden mit dem Default-Wert befüllt.
 */
DNSSenderThread::DNSSenderThread()
{
	buffer=(unsigned char*)malloc(4096);
	if (!buffer) throw ppl7::OutOfMemoryException();
	Zeitscheibe=0.0f;
	runtime=10;
	timeout=5;
	queryrate=0;
	counter_packets_send=0;
	counter_bytes_send=0;
	errors=0;
	counter_0bytes=0;
	duration=0.0;
	for (int i=0;i<255;i++) counter_errorcodes[i]=0;
	verbose=false;
	spoofingEnabled=false;
	DnssecRate=0;
	dnsseccounter=0;
	payload=NULL;
	spoofing_net_start=0;
	spoofing_net_size=0;
}

/*!\brief Destruktor
 *
 * Stoppt den ReceiverThread (sofern er noch läuft) und schließt den Socket.
 */
DNSSenderThread::~DNSSenderThread()
{
	free(buffer);
}


/*!\brief Zieladresse setzen
 *
 * @param destination String mit Zieladresse und Port
 */
void DNSSenderThread::setDestination(const ppl7::IPAddress &ip, int port)
{
	Socket.setDestination(ip, port);
	pkt.setDestination(ip, port);
}


void DNSSenderThread::setPayload(PayloadFile &payload)
{
	this->payload=&payload;
}

/*!\brief Laufzeit festlegen
 *
 * Legt die Laufzeit für den Testlauf fest.
 *
 * @param seconds Laufzeit
 */
void DNSSenderThread::setRuntime(int seconds)
{
	runtime=seconds;
}

/*!\brief Timeout setzen
 *
 * Setzt den Timeout für den Lasttest. Nachdem die Laufzeit für das Senden von Paketen abgelaufen ist,
 * wird noch \seconds Sekunden auf rückkehrende Pakete gewartet.
 *
 * @param seconds Timeout
 */
void DNSSenderThread::setTimeout(int seconds)
{
	timeout=seconds;
}

void DNSSenderThread::setDNSSECRate(int rate)
{
	DnssecRate=rate;
}

/*!\brief Gewünschte Query-Rate pro Sekunde einstellen
 *
 * Ein Wert > 0 aktiviert das Rate-Limiting. Der Sender versucht die gewünschte Anzahl Pakete
 * gleichmäßig auf die Sekunde zu verteilen. Dazu werden Zeitscheiben verwendet, deren Dauer
 * mit der Methode SenderThread::setZeitscheibe konfiguriert werden kann.
 *
 * @param qps Queries pro Sekunde
 */
void DNSSenderThread::setQueryRate(ppluint64 qps)
{
	queryrate=qps;
}

/*!\brief Dauer einer Zeitscheibe bei aktiviertem Rate-Limit einstellen
 *
 * Legt die Dauer einer Zeitscheibe bei aktiviertem Rate-Limit fest. Dabei werden die zu
 * sendenen Pakete pro Sekunde gleichmäßig auf alle Zeitscheiben in der Sekunde verteilt.
 *
 * Der Wert muss mindestens 1 sein (=Default), maximal 1000. Ferner muss der Wert von
 * 1000 teilbar sein.
 *
 * @param ms Dauer einer Zeitscheibe in Millisekunden
 *
 * @exception ppl7::InvalidArgumentsException Wird geworfen, wenn der Wert \p ms nicht den
 * genannten kriterien entspricht.
 */
void DNSSenderThread::setZeitscheibe(float ms)
{
	if (ms==0.0f || ms >1000.0f) throw ppl7::InvalidArgumentsException();
	//if ((1000 % ms)!=0) throw ppl7::InvalidArgumentsException();
	Zeitscheibe=(double)ms/1000;
}


void DNSSenderThread::setSourceIP(const ppl7::IPAddress &ip)
{
	sourceip=ip;
	spoofingEnabled=false;
}

void DNSSenderThread::setSourceNet(const ppl7::IPNetwork &net)
{
	sourcenet=net;
	spoofingEnabled=true;
	spoofing_net_start=ntohl(*(in_addr_t*)net.first().addr());
	spoofing_net_size=powl(2,32-net.prefixlen());
}

void DNSSenderThread::setVerbose(bool verbose)
{
	this->verbose=verbose;
}


/*!\brief Einzelnes Paket senden
 *
 * Generiert ein neues Paket. Die ersten 8 Byte enthalten dabei eine eindeutige fortlaufende
 * ID, die nächsten 8 Byte einen Wert in Double-Precision mit der aktuellen, mikrosekunden genauen
 * Uhrzeit des Servers. Anhand der Uhrzeit kann die Laufzeit eines rückkehrenden Pakets berechnet
 * werden.
 */
void DNSSenderThread::sendPacket()
{
	ppl7::ByteArrayPtr bap;
	size_t query_size;
	while (1) {
		try {
			bap=payload->getQuery();
			query_size=bap.size();
			memcpy(buffer,bap.ptr(),query_size);
			dnsseccounter+=DnssecRate;
			if (dnsseccounter>=100) {
				query_size=AddDnssecToQuery(buffer,4096,query_size);
				dnsseccounter-=100;
			}
			pkt.setPayload(buffer,query_size);
			if (spoofingEnabled) {
				pkt.randomSourceIP(spoofing_net_start, spoofing_net_size);
				pkt.randomSourcePort();
			}
			pkt.setDnsId(getQueryTimestamp());
			ssize_t n=Socket.send(pkt);
			if (n>0 && (size_t)n==pkt.size()) {
				counter_packets_send++;
				counter_bytes_send+=pkt.size();
			} else if (n<0) {
				if (errno<255) counter_errorcodes[errno]++;
				errors++;
			} else {
				counter_0bytes++;
			}
			return;
		} catch (const UnknownRRType &exp) {
		} catch (const InvalidDNSQuery &exp) {
		}
	}
}




/*!\brief Worker-Thread
 *
 * Diese Methode ist der Einstiegspunkt fuer den Workerthread. Hier wird der Socket initialisiert
 * und mit dem Ziel "connected". Sofern Antwortpakete nicht ignoriert werden sollen, wird noch
 * der ReceiverThread gestartet und dann die Last. Jenachdem, ob ein Ratelimit angegeben
 * wurde oder nicht, wird entweder die Methode SenderThread::runWithRateLimit oder SenderThread::runWithoutRateLimit
 * aufgerufen. Nach Ablauf der Laufzeit wird dann noch die Methode SenderThread::waitForTimeout
 * aufgerufen, bevor der Socket wieder geschlossen wird.
 */
void DNSSenderThread::run()
{
	if (!payload) throw ppl7::NullPointerException("payload nicht gesetzt!");
	if (!spoofingEnabled) {
		pkt.setSource(sourceip,0x4567);
	}
	dnsseccounter=0;
	counter_packets_send=0;
	counter_bytes_send=0;
	counter_0bytes=0;
	errors=0;
	duration=0.0;
	for (int i=0;i<255;i++) counter_errorcodes[i]=0;
	double start=ppl7::GetMicrotime();
	if (queryrate>0) {
		runWithRateLimit();
	} else {
		runWithoutRateLimit();
	}
	duration=ppl7::GetMicrotime()-start;
	waitForTimeout();
}


/*!\brief Generiert und empfängt soviele Pakete wie möglich
 *
 * In einer Endlosschleife werden permanent Pakete generiert und versenden.
 */
void DNSSenderThread::runWithoutRateLimit()
{
	double start=ppl7::GetMicrotime();
	double end=start+(double)runtime;
	double now;
	int pc=0;
	while (1) {
		sendPacket();
		pc++;
		if (pc>10000) {
			pc=0;
			if (this->threadShouldStop()) break;
			now=ppl7::GetMicrotime();
			if (now>end) break;
		}
	}
}

/*!\brief Nanosekundengenaue Uhrzeit auslesen
 *
 * Verwendet die Realtime-Uhr des Betriebssystems um die nanosekundengenaue
 * Uhrzeit auszulesen.
 * @return Uhrzeit als Gleitkommazahl. Die Vorkommastellen enthalten die Sekunden seit
 * Epoch, die Nachkommastellen die Nanosekunden.
 */
static inline double getNsec()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (double)ts.tv_sec+((double)ts.tv_nsec/1000000000.0);
}

/*!\brief Generiert eine gewünschte Anzahl Pakete pro Sekunde
 *
 * In einer Endlosschleife werden Pakete generiert und versendet.
 * Um die gewünschte Queryrate zu erreichen, wird die Laufzeit in Zeitscheiben unterteilt und berechnet,
 * wieviele Pakete pro Zeitscheibe versendet werden müssen.
 *
 * \see SenderThread::setRuntime Setzen der Laufzeit
 * \see SenderThread::setQueryRate Setzen der Queryrate
 * \see SenderThread::setZeitscheibe Setzen der Länge einer Zeitscheibe
 *
 * \note Falls das Senden der Pakete länger als die Zeitscheibe dauert, kann die gewünschte Queryrate
 * nicht erreichtwerden. Es liegt ein Bottleneck auf dem Lastgenerator vor. Dies dürfte in der Regel die
 * CPU sein.
 */
void DNSSenderThread::runWithRateLimit()
{
	struct timespec ts;
	ppluint64 total_zeitscheiben=runtime*1000/(Zeitscheibe*1000.0);
	ppluint64 queries_rest=runtime*queryrate;
	ppl7::SockAddr addr=Socket.getSockAddr();
	verbose=true;
	if (verbose) {
		//printf ("qps=%d, runtime=%d\n",queryrate, runtime);
		printf ("Laufzeit: %d s, Dauer Zeitscheibe: %0.6f s, Zeitscheiben total: %llu, Qpzs: %llu, Source: %s:%d\n",
				runtime,Zeitscheibe,total_zeitscheiben,
				queries_rest/total_zeitscheiben,
				(const char*)addr.toIPAddress().toString(), addr.port());
	}
	double now=getNsec();
	double naechste_zeitscheibe=now;
	double next_checktime=now+0.1;

	double start=ppl7::GetMicrotime();
	double end=start+(double)runtime;
	double total_idle=0.0;

	for (ppluint64 z=0;z<total_zeitscheiben;z++) {
		naechste_zeitscheibe+=Zeitscheibe;
		ppluint64 restscheiben=total_zeitscheiben-z;
		ppluint64 queries_pro_zeitscheibe=queries_rest/restscheiben;
		if (restscheiben==1)
			queries_pro_zeitscheibe=queries_rest;
		for (ppluint64 i=0;i<queries_pro_zeitscheibe;i++) {
			sendPacket();
		}

		queries_rest-=queries_pro_zeitscheibe;
		while ((now=getNsec())<naechste_zeitscheibe) {
			if (now<naechste_zeitscheibe) {
				total_idle+=naechste_zeitscheibe-now;
				ts.tv_sec=0;
				ts.tv_nsec=(naechste_zeitscheibe-now)*1000000000;
				nanosleep(&ts,NULL);
			}
		}
		if (now>next_checktime) {
			next_checktime=now+0.1;
			if (this->threadShouldStop()) break;
			if (ppl7::GetMicrotime()>=end) break;
			//printf ("Zeitscheiben rest: %llu\n", z);
		}
	}
	if (verbose) {
		//printf ("total idle: %0.6f\n",total_idle);
	}
}


/*!\brief Wartet solange auf rückkehrende Pakete, bis der Timeout erreicht ist
 *
 * Diese Methode wird aufgerufen, nachdem das Senden der Pakete beendet wurde. Sie wartet
 * noch solange auf rückkehrende Pakete, bis der mittels SenderThread::setTimeout
 * eingestellte Timeout erreicht ist
 */
void DNSSenderThread::waitForTimeout()
{
	double start=ppl7::GetMicrotime();
	double end=start+(double)timeout;
	double now, next_checktime=start+0.1;
	while ((now=ppl7::GetMicrotime())<end) {
		if (now>next_checktime) {
			next_checktime=now+0.1;
			if (this->threadShouldStop()) break;
		}
		ppl7::MSleep(10);
	}
}

/*!\brief Anzahl gesendeter Pakete auslesen
 *
 * @return Anzahl Pakete
 */
ppluint64 DNSSenderThread::getPacketsSend() const
{
	return counter_packets_send;
}

/*!\brief Anzahl gesendeter Bytes auslesen
 *
 * @return Anzahl Bytes
 */
ppluint64 DNSSenderThread::getBytesSend() const
{
	return counter_bytes_send;
}

/*!\brief Anzahl beim Senden aufgetretener Fehler auslesen
 *
 * @return Anzahl Fehler
 */
ppluint64 DNSSenderThread::getErrors() const
{
	return errors;
}

ppluint64 DNSSenderThread::getCounter0Bytes() const
{
	return counter_0bytes;
}

ppluint64 DNSSenderThread::getCounterErrorCode(int err) const
{
	if (err < 255) return counter_errorcodes[err];
	return 0;}


