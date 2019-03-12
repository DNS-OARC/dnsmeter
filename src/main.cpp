#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <signal.h>
#include <list>

#include "../include/sensor.h"
#include "dnspecker.h"

static const char *rcode_names[] = {
		"OK", "FORMAT", "SRVFAIL", "NAME", "NOTIMPL", "REFUSED",
		"YXDOMAIN", "YXRRSET", "NXRRSET", "NOTAUTH", "NOTZONE",
		"11", "12", "13", "14", "15",
		NULL
};



bool stopFlag=false;

void sighandler(int sig)
{
	stopFlag=true;
	printf ("Stopping...\n");
}

unsigned short getQueryTimestamp()
{
	struct timeval tp;
	if (gettimeofday(&tp,NULL)==0) {
		return (tp.tv_sec%6)*10000+(tp.tv_usec/100);
	}
	return 0;
}

double getQueryRTT(unsigned short start)
{
	unsigned short now=getQueryTimestamp();
	unsigned short diff=now-start;
	if (now<start) diff=60000-start+now;
	return (double)(diff)/10000.0f;
}


int main(int argc, char**argv)
{
	res_init();
	// For unknown reason, res_mkquery is much slower (factor 3) when not
	// setting the following options:
	_res.options|=RES_USE_EDNS0;
	_res.options|=RES_USE_DNSSEC;
	DNSSender Sender;
	return Sender.main(argc,argv);
}


void DNSSender::help()
{
	printf ("Usage:\n"
			"  -h            Zeigt diese Hilfe an\n"
			"  -q HOST       Hostname oder IP der Quelle, sofern nicht gespooft\n"
			"                werden soll (siehe -s)\n"
			"  -s NETWORK    Spoofe den Absender. Random-IP aus dem gegebenen Netz\n"
			"                (Beispiel: 192.168.0.0/16). Erfordert root-Rechte!\n"
			"  -e ETH        Interface, auf dem der Receiver lauschen soll (nur FreeBSD)\n"
			"  -z HOST:PORT  Hostname oder IP und Port des Zielservers\n"
			"  -p FILE       Datei mit Queries/Payload\n"
			"  -l #          Laufzeit in Sekunden (Default=10 Sekunden)\n"
			"  -t #          Timeout in Sekunden (Default=2 Sekunden)\n"
			"  -n #          Anzahl Worker-Threads (Default=1)\n"
			"  -r #          Queryrate (Default=soviel wie geht)\n"
			"                Kann auch eine Kommaseparierte Liste sein (rate,rate,...) oder eine\n"
			"                Range (von rate - bis rate, Schrittweite)\n"
			"  -d #          DNSSEC-Anteil in Prozent von 0-100 (Default=0)\n"
			"  -c FILE       CSV-File fuer Ergebnisse\n"
			"  --ignore      Ignoriere die Antworten\n"
			"\n");
}


DNSSender::Results::Results()
{
	queryrate=0;
	counter_send=0;
	counter_received=0;
	bytes_send=0;
	bytes_received=0;
	counter_errors=0;
	packages_lost=0;
	counter_0bytes=0;
	for (int i=0;i<255;i++) counter_errorcodes[i]=0;
	rtt_avg=0.0f;
	rtt_total=0.0f;
	rtt_min=0.0f;
	rtt_max=0.0f;
	for (int i=0;i<16;i++) rcodes[i]=0;
	truncated=0;
}

void DNSSender::Results::clear()
{
	queryrate=0;
	counter_send=0;
	counter_received=0;
	bytes_send=0;
	bytes_received=0;
	counter_errors=0;
	packages_lost=0;
	counter_0bytes=0;
	for (int i=0;i<255;i++) counter_errorcodes[i]=0;
	rtt_avg=0.0f;
	rtt_total=0.0f;
	rtt_min=0.0f;
	rtt_max=0.0f;
	for (int i=0;i<16;i++) rcodes[i]=0;
	truncated=0;
}

DNSSender::Results operator-(const DNSSender::Results &second, const DNSSender::Results &first)
{
	DNSSender::Results r;
	r.queryrate=second.queryrate-first.queryrate;
	r.counter_send=second.counter_send-first.counter_send;
	r.counter_received=second.counter_received-first.counter_received;
	r.bytes_send=second.bytes_send-first.bytes_send;
	r.bytes_received=second.bytes_received-first.bytes_received;
	r.counter_errors=second.counter_errors-first.counter_errors;
	r.packages_lost=second.packages_lost-first.packages_lost;
	r.counter_0bytes=second.counter_0bytes-first.counter_0bytes;
	for (int i=0;i<255;i++) r.counter_errorcodes[i]=second.counter_errorcodes[i]-first.counter_errorcodes[i];
	r.rtt_total=second.rtt_total-first.rtt_total;
	if (r.counter_received) r.rtt_avg=r.rtt_total/r.counter_received;
	else r.rtt_avg=0.0;
	r.rtt_min=second.rtt_min-first.rtt_min;
	r.rtt_max=second.rtt_max-first.rtt_max;

	for (int i=0;i<16;i++) r.rcodes[i]=second.rcodes[i]-first.rcodes[i];
	r.truncated=second.truncated-first.truncated;
	return r;
}




DNSSender::DNSSender()
{
	ppl7::InitSockets();
	Laufzeit=10;
	Timeout=2;
	ThreadCount=1;
	Zeitscheibe=1.0f;
	ignoreResponses=false;
	DnssecRate=0;
	TargetPort=53;
	spoofingEnabled=false;
	Receiver=NULL;
}

DNSSender::~DNSSender()
{
	if (Receiver) delete Receiver;
}


ppl7::Array DNSSender::getQueryRates(const ppl7::String &QueryRates)
{
	ppl7::Array rates;
	if (QueryRates.isEmpty()) {
		rates.add("0");
	} else {
		ppl7::Array matches;
		if (QueryRates.pregMatch("/^([0-9]+)-([0-9]+),([0-9]+)$", matches)) {
			for (ppluint64 i = matches[1].toUnsignedInt64(); i <= matches[2].toUnsignedInt64(); i += matches[3].toUnsignedInt64()) {
				rates.addf("%llu", i);
			}

		} else {
			rates.explode(QueryRates, ",");
		}
	}
	return rates;
}

void DNSSender::getTarget(int argc, char**argv)
{
	if (!ppl7::HaveArgv(argc,argv,"-z")) {
		throw MissingCommandlineParameter("Ziel-IP/Hostname oder Port nicht angegeben (-z IP:PORT)");
	}
	ppl7::String Tmp=ppl7::GetArgv(argc,argv,"-z");
	ppl7::Array Tok(Tmp,":");
	if (Tok.size()!=2) {
		if (Tok.size()!=1) throw InvalidCommandlineParameter("-z IP:PORT");
		TargetPort=53;
	} else {
		TargetPort=Tok[1].toInt();
	}
	if (TargetPort<1 || TargetPort>65535) throw InvalidCommandlineParameter("-z IP:PORT, Invalid Port");
	std::list<ppl7::IPAddress> Result;
	size_t num=ppl7::GetHostByName(Tok[0], Result,ppl7::af_inet);
	if (!num) throw InvalidCommandlineParameter("-z IP:PORT, Invalid IP or could not resolve Hostname");
	TargetIP=Result.front();
	//printf ("num=%d, %s\n",num, (const char*)TargetIP.toString());
}

void DNSSender::getSource(int argc, char**argv)
{
	if (ppl7::HaveArgv(argc,argv,"-s")) {
		SourceNet.set(ppl7::GetArgv(argc,argv,"-s"));
		if (SourceNet.family()!=ppl7::IPAddress::IPv4) throw UnsupportedIPFamily("only IPv4 works");
		spoofingEnabled=true;
	} else {
		ppl7::String Tmp=ppl7::GetArgv(argc,argv,"-q");
		std::list<ppl7::IPAddress> Result;
		size_t num=ppl7::GetHostByName(Tmp, Result,ppl7::af_inet);
		if (!num) throw InvalidCommandlineParameter("-q HOST, Invalid IP or could not resolve Hostname");
		SourceIP=Result.front();
		if (SourceIP.family()!=ppl7::IPAddress::IPv4) throw UnsupportedIPFamily("only IPv4 works");
		spoofingEnabled=false;
	}
}

int DNSSender::getParameter(int argc, char**argv)
{
	if (ppl7::HaveArgv(argc,argv,"-q") && ppl7::HaveArgv(argc,argv,"-s")) {
		printf ("ERROR: Parameter koennen nicht gleichzeitig verwendet werden: -q -s\n\n");
		help();
		return 1;
	}
	if ((!ppl7::HaveArgv(argc,argv,"-q")) && (!ppl7::HaveArgv(argc,argv,"-s"))) {
		printf ("ERROR: Quell IP/Host oder gespooftes Netz muss angegeben werden (-q IP | -s NETZ)\n\n");
		help();
		return 1;
	}
	ignoreResponses=ppl7::HaveArgv(argc,argv,"--ignore");

	if (ppl7::HaveArgv(argc,argv,"-e")) {
		InterfaceName=ppl7::GetArgv(argc,argv,"-e");
	}

	try {
		getTarget(argc, argv);
		getSource(argc, argv);
	} catch (const ppl7::Exception &e) {
		printf ("ERROR: Fehlender oder fehlerhafter Parameter\n");
		e.print();
		printf ("\n");
		help();
		return 1;
	}

	Laufzeit = ppl7::GetArgv(argc,argv,"-l").toInt();
	Timeout = ppl7::GetArgv(argc,argv,"-t").toInt();
	ThreadCount = ppl7::GetArgv(argc,argv,"-n").toInt();
	ppl7::String QueryRates = ppl7::GetArgv(argc,argv,"-r");
	CSVFileName = ppl7::GetArgv(argc,argv,"-c");
	QueryFilename = ppl7::GetArgv(argc,argv,"-p");
	if (ppl7::HaveArgv(argc,argv,"-d")) {
		DnssecRate=ppl7::GetArgv(argc,argv,"-d").toInt();
		if (DnssecRate<0 || DnssecRate>100) {
			printf ("ERROR: DNSSEC-Rate muss zwischen 0 und 100 liegen (-d #)\n\n");
			help();
			return 1;
		}
	}
	if (!ThreadCount) ThreadCount=1;
	if (!Laufzeit) Laufzeit=10;
	if (!Timeout) Timeout=2;
	if (QueryFilename.isEmpty()) {
		printf ("ERROR: Payload-File ist nicht angegeben (-p FILENAME)\n\n");
		help();
		return 1;
	}
	rates = getQueryRates(QueryRates);
	return 0;
}


int DNSSender::openFiles()
{
	if (CSVFileName.notEmpty()) {
		try {
			openCSVFile(CSVFileName);
		} catch (const ppl7::Exception &e) {
			printf ("ERROR: CSV-File konnte nicht geoeffnet werden\n");
			e.print();
			return 1;
		}
	}
	try {
		payload.openQueryFile(QueryFilename);
	} catch (const ppl7::Exception &e) {
		printf ("ERROR: Payload-File konnte nicht geoeffnet werden oder enthaelt keine Queries\n");
		e.print();
		return 1;
	}
	return 0;
}


int DNSSender::main(int argc, char**argv)
{
	if (ppl7::HaveArgv(argc,argv,"-h") || ppl7::HaveArgv(argc,argv,"--help")) {
		help();
		return 0;
	}
	if (getParameter(argc,argv)!=0) return 1;
	if (openFiles()!=0) return 1;

	signal(SIGINT,sighandler);
	signal(SIGKILL,sighandler);

	DNSSender::Results results;
	try {
		if (!ignoreResponses) {
			Receiver=new DNSReceiverThread();
			Receiver->setSource(TargetIP,TargetPort);
			try {
				Receiver->setInterface(InterfaceName);
			} catch (const ppl7::Exception &e) {
				printf ("ERROR: Konnte nicht an Device binden [%s]\n",(const char*)InterfaceName);
				e.print();
				printf ("\n");
				help();
				return 1;
			}

		}
		prepareThreads();
		for (size_t i=0;i<rates.size();i++) {
			results.queryrate=rates[i].toInt();
			run(rates[i].toInt());
			getResults(results);
			presentResults(results);
			saveResultsToCsv(results);
		}
		threadpool.destroyAllThreads();
	} catch (const ppl7::OperationInterruptedException &) {
		getResults(results);
		presentResults(results);
		saveResultsToCsv(results);
	} catch (const ppl7::Exception &e) {
		e.print();
		return 1;
	}
	return 0;
}

void DNSSender::prepareThreads()
{
	for (int i=0;i<ThreadCount;i++) {
		DNSSenderThread *thread=new DNSSenderThread();
		thread->setDestination(TargetIP,TargetPort);
		thread->setRuntime(Laufzeit);
		thread->setTimeout(Timeout);
		thread->setZeitscheibe(Zeitscheibe);
		thread->setDNSSECRate(DnssecRate);
		thread->setVerbose(false);
		thread->setPayload(payload);
		if (spoofingEnabled) {
			thread->setSourceNet(SourceNet);
		} else {
			thread->setSourceIP(SourceIP);
		}
		threadpool.addThread(thread);
	}
}

void DNSSender::openCSVFile(const ppl7::String &Filename)
{
	CSVFile.open(Filename,ppl7::File::APPEND);
	if (CSVFile.size()==0) {
		CSVFile.putsf ("#QPS Send; QPS Received; QPS Errors; Lostrate; "
				"rtt_avg; rtt_min; rtt_max;"
				"\n");
		CSVFile.flush();
	}
}

void DNSSender::showCurrentStats(ppl7::ppl_time_t start_time)
{
	DNSSender::Results result, diff;
	ppl7::ppl_time_t runtime=ppl7::GetTime()-start_time;
	getResults(result);
	diff=result-vis_prev_results;
	vis_prev_results=result;

	int h=(int)(runtime/3600);
	runtime-=h*3600;
	int m=(int)(runtime/60);
	int s=runtime-(m*60);


	printf ("%02d:%02d:%02d Queries send: %7llu, rcv: %7llu, ", h,m,s,
			diff.counter_send, diff.counter_received
			);
	printf ("Data send: %6llu KB, rcv: %6llu KB", diff.bytes_send/1024, diff.bytes_received/1024);
	printf ("\n");
}


void DNSSender::calcZeitscheibe(int queryrate)
{
	Zeitscheibe=(1000.0f/queryrate)*ThreadCount;
	//if (Zeitscheibe<1.0f) Zeitscheibe=1.0f;
	if (Zeitscheibe<0.1f) Zeitscheibe=0.1f;
}


void DNSSender::run(int queryrate)
{
	printf ("###############################################################################\n");
	if (queryrate) {
		calcZeitscheibe(queryrate);
		printf ("# Start Session with Threads: %d, Queryrate: %d, Timeslot: %0.6f ms\n",
				ThreadCount,queryrate, Zeitscheibe);
	} else {
		printf ("# Start Session with Threads: %d, Queryrate: unlimited\n",
				ThreadCount);
	}

	ppl7::ThreadPool::iterator it;
	for (it=threadpool.begin();it!=threadpool.end();++it) {
		((DNSSenderThread*)(*it))->setQueryRate(queryrate/ThreadCount);
		((DNSSenderThread*)(*it))->setZeitscheibe(Zeitscheibe);
	}
	vis_prev_results.clear();
	sampleSensorData(sys1);
	if (Receiver) Receiver->threadStart();
	threadpool.startThreads();
	ppl7::ppl_time_t start=ppl7::GetTime();
	ppl7::ppl_time_t report=start+1;
	ppl7::MSleep(500);
	while (threadpool.running()==true && stopFlag==false) {
		ppl7::MSleep(100);
		ppl7::ppl_time_t now=ppl7::GetTime();
		if (now>=report) {
			report=now+1;
			showCurrentStats(start);
		}
	}
	if (Receiver) Receiver->threadStop();
	sampleSensorData(sys2);
	if (stopFlag==true) {
		threadpool.stopThreads();
		throw ppl7::OperationInterruptedException("Lasttest wurde abgebrochen");
	}
}


void DNSSender::getResults(DNSSender::Results &result)
{
	ppl7::ThreadPool::iterator it;
	result.clear();

	for (it=threadpool.begin();it!=threadpool.end();++it) {
		result.counter_send+=((DNSSenderThread*)(*it))->getPacketsSend();
		result.bytes_send+=((DNSSenderThread*)(*it))->getBytesSend();
		result.counter_errors+=((DNSSenderThread*)(*it))->getErrors();
		result.counter_0bytes+=((DNSSenderThread*)(*it))->getCounter0Bytes();
		for (int i=0;i<255;i++) result.counter_errorcodes[i]+=((DNSSenderThread*)(*it))->getCounterErrorCode(i);
	}
	if (Receiver) {
		const RawSocketReceiver::Counter &counter=Receiver->getCounter();
		result.counter_received=counter.num_pkgs;
		result.bytes_received=counter.bytes_rcv;
		result.rtt_total=counter.rtt_total;
		if (counter.num_pkgs) result.rtt_avg=counter.rtt_total/counter.num_pkgs;
		else result.rtt_avg=0.0;
		result.rtt_min=counter.rtt_min;
		result.rtt_max=counter.rtt_max;
		for (int i=0;i<16;i++) result.rcodes[i]=counter.rcodes[i];
		result.truncated=counter.truncated;
	}

	result.packages_lost=result.counter_send-result.counter_received;
	if (result.counter_received>result.counter_send) result.packages_lost=0;
}


void DNSSender::saveResultsToCsv(const DNSSender::Results &result)
{

	if (CSVFile.isOpen()) {
		CSVFile.putsf ("%llu;%llu;%llu;%0.3f;%0.4f;%0.4f;%0.4f;\n",
				(ppluint64)((double)result.counter_send/(double)Laufzeit),
				(ppluint64)((double)result.counter_received/(double)Laufzeit),
				(ppluint64)((double)result.counter_errors/(double)Laufzeit),
				(double)result.packages_lost*100.0/(double)result.counter_send,
				result.rtt_total*1000.0/(double)ThreadCount,
				result.rtt_min*1000.0,
				result.rtt_max*1000.0
		);
		CSVFile.flush();
	}
}


void DNSSender::presentResults(const DNSSender::Results &result)
{
	printf ("===============================================================================\n");
	const SystemStat::Interface &net1=sys1.interfaces[InterfaceName];
	const SystemStat::Interface &net2=sys2.interfaces[InterfaceName];
	SystemStat::Network transmit=SystemStat::Network::getDelta(net1.transmit, net2.transmit);
	SystemStat::Network received=SystemStat::Network::getDelta(net1.receive, net2.receive);
	printf ("Netzwerk If %s Pkt send: %lu, rcv: %lu, Data send: %lu KB, rcv: %lu KB\n",
			(const char*)InterfaceName,
			transmit.packets, received.packets, transmit.bytes/1024, received.bytes/1024);

	ppluint64 qps_send=(ppluint64)((double)result.counter_send/(double)Laufzeit);
	ppluint64 bps_send=(ppluint64)((double)result.bytes_send/(double)Laufzeit);
	ppluint64 qps_received=(ppluint64)((double)result.counter_received/(double)Laufzeit);
	ppluint64 bps_received=(ppluint64)((double)result.bytes_received/(double)Laufzeit);

	printf ("DNS Queries send: %10llu, Qps: %7llu, Data send: %7llu KB = %6llu MBit\n",
			result.counter_send, qps_send, result.bytes_send/1024, bps_send/(1024*1024));

	printf ("DNS Queries rcv:  %10llu, Qps: %7llu, Data rcv:  %7llu KB = %6llu MBit\n",
			result.counter_received, qps_received, result.bytes_received/1024, bps_received/(1024*1024));

	printf ("DNS Queries lost: %10llu = %0.3f %%\n",result.packages_lost,
			(double)result.packages_lost*100.0/(double)result.counter_send);

	printf ("DNS rtt average: %0.4f ms, "
			"min: %0.4f ms, "
			"max: %0.4f ms\n",
			result.rtt_total*1000.0/(double)ThreadCount,
			result.rtt_min*1000.0,
			result.rtt_max*1000.0);
	printf ("DNS truncated: %llu\nDNS RCODES: ", result.truncated);
	for (int i=0;i<15;i++) {
		if (result.rcodes[i]) {
			printf ("%s: %llu, ",rcode_names[i],result.rcodes[i]);
		}
	}
	printf ("\n");



	if (result.counter_errors) {
		printf ("Errors:           %10llu, Qps: %10llu\n",result.counter_errors,
				(ppluint64)((double)result.counter_errors/(double)Laufzeit));
	}
	if (result.counter_0bytes) {
		printf ("Errors 0Byte:     %10llu, Qps: %10llu\n",result.counter_0bytes,
				(ppluint64)((double)result.counter_0bytes/(double)Laufzeit));
	}
	for (int i=0;i<255;i++) {
		if (result.counter_errorcodes[i]>0) {
			printf ("Errors %3d:       %10llu, Qps: %10llu [%s]\n",i, result.counter_errorcodes[i],
					(ppluint64)((double)result.counter_errorcodes[i]/(double)Laufzeit),
					strerror(i));

		}
	}

}

