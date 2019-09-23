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

#ifndef __dnsmeter_packet_h
#define __dnsmeter_packet_h

class Packet {
private:
    unsigned char* buffer;
    int            buffersize;
    int            payload_size;
    bool           chksum_valid;

    void updateChecksums();

public:
    Packet();
    ~Packet();
    void setSource(const ppl7::IPAddress& ip_addr, int port);
    void setDestination(const ppl7::IPAddress& ip_addr, int port);
    void setPayload(const void* payload, size_t size);
    void setPayloadDNSQuery(const ppl7::String& query, bool dnssec = false);
    void setDnsId(unsigned short id);
    void setIpId(unsigned short id);

    void randomSourceIP(const ppl7::IPNetwork& net);
    void randomSourceIP(unsigned int start, unsigned int size);
    void randomSourcePort();
    void useSourceFromPcap(const char* pkt, size_t size);

    size_t         size() const;
    unsigned char* ptr();
};

#endif
