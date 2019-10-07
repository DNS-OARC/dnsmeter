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

#include "payload_file.h"
#include "exceptions.h"
#include "query.h"

#define __FAVOR_BSD 1
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pcap/pcap.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#pragma pack(push) /* push current alignment to stack */
#pragma pack(1) /* set alignment to 1 byte boundary */
struct ETHER {
    unsigned char  destination[6];
    unsigned char  source[6];
    unsigned short type;
};
#pragma pack(pop) /* restore original alignment from stack */

PayloadFile::PayloadFile()
{
    validLinesInQueryFile = 0;
    payloadIsPcap         = false;
}

bool PayloadFile::detectPcap(ppl7::File& ff)
{
    unsigned char buffer[8];
    if (ff.read(buffer, 8) != 8) {
        ff.seek(0);
        return false;
    }
    ff.seek(0);
    unsigned int magic = ppl7::Peek32(buffer + 0);
    if (magic == 0xa1b2c3d4 || magic == 0xa1b23c4d)
        return true;
    if (magic == 0xd4c3b2a1 || magic == 0x4d3cb2a1)
        return true;
    return false;
}

void PayloadFile::openQueryFile(const ppl7::String& Filename)
{
    if (Filename.isEmpty())
        throw InvalidQueryFile("File not given");
    ppl7::File QueryFile;
    QueryFile.open(Filename, ppl7::File::READ);
    if (QueryFile.size() == 0) {
        throw InvalidQueryFile("File is empty [%s]", (const char*)Filename);
    }
    printf("INFO: Loading and precompile payload. This could take some time...\n");
    if (detectPcap(QueryFile)) {
        loadAndCompilePcapFile(Filename);
    } else {
        loadAndCompile(QueryFile);
    }
    printf("INFO: %llu queries loaded\n", validLinesInQueryFile);
    it = querycache.begin();
}

void PayloadFile::loadAndCompile(ppl7::File& ff)
{
    ppl7::ByteArray buf(4096);
    ppl7::String    buffer;
    validLinesInQueryFile         = 0;
    unsigned char* compiled_query = (unsigned char*)buf.ptr();
    while (1) {
        try {
            if (ff.eof())
                throw ppl7::EndOfFileException();
            ff.gets(buffer, 1024);
            buffer.trim();
            if (buffer.isEmpty())
                continue;
            if (buffer.c_str()[0] == '#')
                continue;
            try {
                // Precompile Query
                int size = MakeQuery(buffer, compiled_query, 4096, false);
                querycache.push_back(ppl7::ByteArray(compiled_query, size));
                validLinesInQueryFile++;
            } catch (...) {
                // ignore invalid queries
            }
        } catch (const ppl7::EndOfFileException&) {
            if (validLinesInQueryFile == 0) {
                throw InvalidQueryFile("No valid Queries found in Queryfile");
            }
            return;
        }
    }
}

void PayloadFile::loadAndCompilePcapFile(const ppl7::String& Filename)
{
    char               errorbuffer[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr hdr;
    payloadIsPcap         = true;
    validLinesInQueryFile = 0;
    pcap_t* pp            = pcap_open_offline((const char*)Filename, errorbuffer);
    if (!pp)
        throw InvalidQueryFile("%s", errorbuffer);
    ppluint64     pkts_total = 0;
    const u_char* pkt;
    while ((pkt = pcap_next(pp, &hdr)) != NULL) {
        pkts_total++;
        //printf ("len=%d, caplen=%d\n",hdr.len,hdr.caplen);
        const struct ETHER* eth = (const struct ETHER*)pkt;
        if (hdr.caplen > 4096)
            continue;
        if (eth->type != htons(0x0800))
            continue;
        const struct ip* iphdr = (const struct ip*)(pkt + 14);
        if (iphdr->ip_v != 4)
            continue;
        const struct udphdr* udp = (const struct udphdr*)(pkt + 14 + sizeof(struct ip));
        if (udp->uh_dport != htons(53))
            continue;
        const struct DNS_HEADER* dns = (const struct DNS_HEADER*)(pkt + 14 + sizeof(struct ip) + sizeof(struct udphdr));
        if (dns->qr != 0 || dns->opcode != 0)
            continue;
        querycache.push_back(ppl7::ByteArray(pkt, hdr.caplen));
        validLinesInQueryFile++;
    }
    printf("Packets read from pcap file: %llu, valid UDP DNS queries: %llu\n",
        pkts_total, validLinesInQueryFile);
    pcap_close(pp);
    if (validLinesInQueryFile == 0) {
        throw InvalidQueryFile("No valid Queries found in pcap file [%s]", (const char*)Filename);
    }
}

const ppl7::ByteArrayPtr& PayloadFile::getQuery()
{
    QueryMutex.lock();
    const ppl7::ByteArrayPtr& bap = *it;
    ++it;
    if (it == querycache.end())
        it = querycache.begin();
    QueryMutex.unlock();
    return bap;
}

bool PayloadFile::isPcap()
{
    return payloadIsPcap;
}
