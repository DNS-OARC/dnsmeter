/*
 * Copyright (c) 2019-2021, OARC, Inc.
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

#include "packet.h"
#include "exceptions.h"
#include "query.h"

#define __FAVOR_BSD 1
#include <netinet/in.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <math.h>

#define USZ sizeof(struct udphdr)
#define ISZ sizeof(struct ip)
#define HDRSZ ISZ + USZ
#define MAXPACKETSIZE 4096

static unsigned short in_cksum(unsigned short* addr, int len)
{
    int             nleft  = len;
    unsigned short* w      = addr;
    int             sum    = 0;
    unsigned short  answer = 0;
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    if (nleft == 1) {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}

static unsigned short udp_cksum(const struct ip* iphdr, const struct udphdr* udp, const unsigned char* payload, size_t payload_size)
{
    unsigned char cbuf[MAXPACKETSIZE];
    memset(cbuf, 0, sizeof(cbuf));
    unsigned char* p  = (unsigned char*)cbuf;
    *(unsigned int*)p = iphdr->ip_src.s_addr;
    p += sizeof(unsigned int);
    *(unsigned int*)p = iphdr->ip_dst.s_addr;
    p += sizeof(unsigned int);
    *(unsigned char*)p++ = 0;
    *(unsigned char*)p++ = iphdr->ip_p;
    *(unsigned short*)p  = udp->uh_ulen;
    p += sizeof(unsigned short);
    memcpy(p, udp, USZ);
    p += USZ;
    memcpy(p, payload, payload_size);
    return in_cksum((unsigned short*)cbuf, sizeof(uint) * 3 + sizeof(struct udphdr) + payload_size + (payload_size % 2));
}

Packet::Packet()
{
    buffersize   = MAXPACKETSIZE;
    payload_size = 0;
    buffer       = (unsigned char*)calloc(1, buffersize);
    if (!buffer)
        throw ppl7::OutOfMemoryException();

    struct ip*     iphdr = (struct ip*)buffer;
    struct udphdr* udp   = (struct udphdr*)(buffer + ISZ);

    iphdr->ip_hl         = ISZ >> 2;
    iphdr->ip_v          = IPVERSION;
    iphdr->ip_tos        = 0;
    iphdr->ip_off        = 0;
    iphdr->ip_ttl        = 64;
    iphdr->ip_p          = IPPROTO_UDP;
    iphdr->ip_sum        = 0;
    iphdr->ip_id         = 0;
    iphdr->ip_src.s_addr = 0;
    iphdr->ip_dst.s_addr = 0;
    iphdr->ip_len        = htons(HDRSZ + payload_size);
    iphdr->ip_sum        = in_cksum((unsigned short*)iphdr, ISZ);

    udp->uh_ulen = htons(USZ + payload_size);
    chksum_valid = false;
}

Packet::~Packet()
{
    free(buffer);
}

void Packet::setSource(const ppl7::IPAddress& ip_addr, int port)
{
    struct ip*     iphdr = (struct ip*)buffer;
    struct udphdr* udp   = (struct udphdr*)(buffer + ISZ);
    iphdr->ip_src.s_addr = *(in_addr_t*)ip_addr.addr();
    udp->uh_sport        = htons(port);
    chksum_valid         = false;
}

void Packet::randomSourcePort()
{
    struct udphdr* udp = (struct udphdr*)(buffer + ISZ);
    udp->uh_sport      = htons(ppl7::rand(1024, 65535));
    chksum_valid       = false;
}

void Packet::randomSourceIP(const ppl7::IPNetwork& net)
{
    struct ip* iphdr     = (struct ip*)buffer;
    in_addr_t  start     = ntohl(*(in_addr_t*)net.first().addr());
    size_t     size      = powl(2, 32 - net.prefixlen());
    iphdr->ip_src.s_addr = htonl(ppl7::rand(start, start + size - 1));
    chksum_valid         = false;
}

void Packet::randomSourceIP(unsigned int start, unsigned int size)
{
    struct ip* iphdr     = (struct ip*)buffer;
    iphdr->ip_src.s_addr = htonl(ppl7::rand(start, start + size - 1));
    chksum_valid         = false;
}

void Packet::useSourceFromPcap(const char* pkt, size_t size)
{
    const struct ip*     s_iphdr = (const struct ip*)(pkt + 14);
    const struct udphdr* s_udp   = (const struct udphdr*)(pkt + 14 + sizeof(struct ip));
    struct ip*           iphdr   = (struct ip*)buffer;
    struct udphdr*       udp     = (struct udphdr*)(buffer + ISZ);
    iphdr->ip_src.s_addr         = s_iphdr->ip_src.s_addr;
    udp->uh_sport                = s_udp->uh_sport;
}

void Packet::setDestination(const ppl7::IPAddress& ip_addr, int port)
{
    struct ip*     iphdr = (struct ip*)buffer;
    struct udphdr* udp   = (struct udphdr*)(buffer + ISZ);
    iphdr->ip_dst.s_addr = *(in_addr_t*)ip_addr.addr();
    udp->uh_dport        = htons(port);
    chksum_valid         = false;
}

void Packet::setIpId(unsigned short id)
{
    struct ip* iphdr = (struct ip*)buffer;
    iphdr->ip_id     = htons(id);
    chksum_valid     = false;
}

void Packet::setDnsId(unsigned short id)
{
    *((unsigned short*)(buffer + HDRSZ)) = htons(id);
    chksum_valid                         = false;
}

void Packet::setPayload(const void* payload, size_t size)
{
    if (size + HDRSZ > MAXPACKETSIZE)
        throw BufferOverflow("%zd > %zd", size, MAXPACKETSIZE - HDRSZ);
    memcpy(buffer + HDRSZ, payload, size);
    payload_size         = size;
    struct ip*     iphdr = (struct ip*)buffer;
    struct udphdr* udp   = (struct udphdr*)(buffer + ISZ);
    iphdr->ip_len        = htons(HDRSZ + payload_size);
    udp->uh_ulen         = htons(USZ + payload_size);
    chksum_valid         = false;
}

void Packet::setPayloadDNSQuery(const ppl7::String& query, bool dnssec)
{
    payload_size         = MakeQuery(query, buffer + HDRSZ, buffersize - HDRSZ, dnssec);
    struct ip*     iphdr = (struct ip*)buffer;
    struct udphdr* udp   = (struct udphdr*)(buffer + ISZ);
    iphdr->ip_len        = htons(HDRSZ + payload_size);
    udp->uh_ulen         = htons(USZ + payload_size);
    chksum_valid         = false;
}

void Packet::updateChecksums()
{
    struct ip*     iphdr = (struct ip*)buffer;
    struct udphdr* udp   = (struct udphdr*)(buffer + ISZ);
    iphdr->ip_sum        = 0;
    iphdr->ip_sum        = in_cksum((unsigned short*)iphdr, ISZ);
    udp->uh_sum          = 0;
    udp->uh_sum          = udp_cksum(iphdr, udp, buffer + HDRSZ, payload_size);
    chksum_valid         = true;
}

size_t Packet::size() const
{
    return HDRSZ + payload_size;
}

unsigned char* Packet::ptr()
{
    if (!chksum_valid)
        updateChecksums();
    return buffer;
}
