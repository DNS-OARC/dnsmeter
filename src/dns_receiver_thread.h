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

#include "raw_socket_receiver.h"

#include <ppl7.h>
#include <ppl7-inet.h>

#ifndef __dnsmeter_dns_receiver_thread_h
#define __dnsmeter_dns_receiver_thread_h

class DNSReceiverThread : public ppl7::Thread {
private:
    RawSocketReceiver          Socket;
    RawSocketReceiver::Counter counter;

public:
    DNSReceiverThread();
    ~DNSReceiverThread();
    void setInterface(const ppl7::String& Device);
    void setSource(const ppl7::IPAddress& ip, int port);
    void run();

    ppluint64 getPacketsReceived() const;
    ppluint64 getBytesReceived() const;

    double                            getDuration() const;
    double                            getRoundTripTimeAverage() const;
    double                            getRoundTripTimeMin() const;
    double                            getRoundTripTimeMax() const;
    const RawSocketReceiver::Counter& getCounter() const;
};

#endif
