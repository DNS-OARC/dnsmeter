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

#include <ppl7.h>
#include <ppl7-inet.h>

#ifndef __dnsmeter_raw_socket_receiver_h
#define __dnsmeter_raw_socket_receiver_h

class RawSocketReceiver {
public:
    class Counter {
    public:
        Counter();
        void      clear();
        ppluint64 num_pkgs;
        ppluint64 bytes_rcv;
        ppluint64 rcodes[16];
        ppluint64 truncated;
        double    rtt_total, rtt_min, rtt_max;
    };

private:
    ppl7::IPAddress SourceIP;
    unsigned char*  buffer;
    int             buflen;
    int             sd;
    unsigned short  SourcePort;
#ifdef __FreeBSD__
    bool useZeroCopyBuffer;
#endif

public:
    RawSocketReceiver();
    ~RawSocketReceiver();
    void initInterface(const ppl7::String& Device);
    bool socketReady();
    void setSource(const ppl7::IPAddress& ip_addr, int port);
    void receive(Counter& counter);
};

#endif
