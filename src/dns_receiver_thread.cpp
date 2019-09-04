/*
 * Copyright (c) 2019, OARC, Inc.
 * Copyright (c) 2019, DENIC eG
 * All rights reserved.
 *
 * This file is part of dnsmeter.
 *
 * dnsmeter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dnsmeter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dnsmeter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "dns_receiver_thread.h"

DNSReceiverThread::DNSReceiverThread()
{
}

DNSReceiverThread::~DNSReceiverThread()
{
}

void DNSReceiverThread::setInterface(const ppl7::String& Device)
{
    Socket.initInterface(Device);
}

void DNSReceiverThread::setSource(const ppl7::IPAddress& ip, int port)
{
    Socket.setSource(ip, port);
}

void DNSReceiverThread::run()
{
    counter.clear();
    while (1) {
        if (Socket.socketReady())
            Socket.receive(counter);
        if (this->threadShouldStop())
            break;
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
    if (counter.num_pkgs)
        return counter.rtt_total / counter.num_pkgs;
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

const RawSocketReceiver::Counter& DNSReceiverThread::getCounter() const
{
    return counter;
}
