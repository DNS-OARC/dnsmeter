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

#include "packet.h"

#include <ppl7.h>

#ifndef __dnsmeter_raw_socket_sender_h
#define __dnsmeter_raw_socket_sender_h

class RawSocketSender {
private:
    void* buffer;
    int   sd;

public:
    RawSocketSender();
    ~RawSocketSender();
    void setDestination(const ppl7::IPAddress& ip_addr, int port);
    ssize_t send(Packet& pkt);
    ppl7::SockAddr getSockAddr() const;
    bool           socketReady();
};

#endif
