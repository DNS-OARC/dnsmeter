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

#include "dnspecker.h"


DNSReceiverThread::DNSReceiverThread()
{

}

DNSReceiverThread::~DNSReceiverThread()
{

}

void DNSReceiverThread::setInterface(const ppl7::String &Device)
{
	Socket.initInterface(Device);
}

void DNSReceiverThread::setSource(const ppl7::IPAddress &ip, int port)
{
	Socket.setSource(ip, port);
}

void DNSReceiverThread::run()
{
	counter.clear();
	while (1) {
		if (Socket.socketReady()) Socket.receive(counter);
		if (this->threadShouldStop()) break;
	}
}

ppluint64 DNSReceiverThread::getPacketsReceived() const
{
	return counter.num_pkgs;
}

ppluint64 DNSReceiverThread::getBytesReceived() const
{
	return counter.bytes_rcv;
}

double DNSReceiverThread::getDuration() const
{
	return counter.rtt_total;
}

double DNSReceiverThread::getRoundTripTimeAverage() const
{
	if (counter.num_pkgs) return counter.rtt_total/counter.num_pkgs;
	return 0.0f;
}

double DNSReceiverThread::getRoundTripTimeMin() const
{
	return counter.rtt_min;
}

double DNSReceiverThread::getRoundTripTimeMax() const
{
	return counter.rtt_max;
}

const RawSocketReceiver::Counter &DNSReceiverThread::getCounter() const
{
	return counter;
}



