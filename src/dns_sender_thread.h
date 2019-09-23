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

#include "raw_socket_sender.h"
#include "payload_file.h"

#include <ppl7.h>

#ifndef __dnsmeter_dns_sender_thread_h
#define __dnsmeter_dns_sender_thread_h

class DNSSenderThread : public ppl7::Thread {
private:
    RawSocketSender Socket;
    Packet          pkt;

    ppl7::IPAddress destination;
    ppl7::IPAddress sourceip;
    ppl7::IPNetwork sourcenet;

    PayloadFile*   payload;
    unsigned char* buffer;
    ppluint64      queryrate;
    ppluint64      counter_packets_send, errors, counter_0bytes;
    ppluint64      counter_bytes_send;
    ppluint64      counter_errorcodes[255];

    unsigned int spoofing_net_start;
    unsigned int spoofing_net_size;

    int    runtime;
    int    timeout;
    int    DnssecRate;
    int    dnsseccounter;
    double Timeslice;

    double duration;
    bool   spoofingEnabled;
    bool   verbose;
    bool   payloadIsPcap;
    bool   spoofingFromPcap;

    void sendPacket();
    void waitForTimeout();
    bool socketReady();

    void runWithoutRateLimit();
    void runWithRateLimit();

public:
    DNSSenderThread();
    ~DNSSenderThread();
    void setDestination(const ppl7::IPAddress& ip, int port);
    void setSourceIP(const ppl7::IPAddress& ip);
    void setSourceNet(const ppl7::IPNetwork& net);
    void setSourcePcap();
    void setRandomSource(const ppl7::IPNetwork& net);
    void setRuntime(int seconds);
    void setTimeout(int seconds);
    void setDNSSECRate(int rate);
    void setQueryRate(ppluint64 qps);
    void setTimeslice(float ms);
    void setVerbose(bool verbose);
    void setPayload(PayloadFile& payload);
    void      run();
    ppluint64 getPacketsSend() const;
    ppluint64 getBytesSend() const;
    ppluint64 getErrors() const;
    ppluint64 getCounter0Bytes() const;
    ppluint64 getCounterErrorCode(int err) const;
};

#endif
