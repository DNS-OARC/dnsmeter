/*
 * This file is part of dnsmeter by Patrick Fedick <fedick@denic.de>
 *
 * Copyright (c) 2019 DENIC eG
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <ppl7.h>
#include <ppl7-inet.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#include "../include/dnsmeter.h"

static const char *rr_types[] = {
		"A", "AAAA", "MX", "NS", "DS", "DNSKEY", "TXT", "SOA", "NAPTR", "RRSIG",
		"NSEC", "NSEC3", "NSEC3PARAM", "PTR", "SRV",
		"CNAME","TSIG","*","ANY","AXFR","IXFR",
		"SPF", "A6", "HINFO", "WKS", "NULL",
		NULL
};

static int rr_code[] = {
		1,28,15,2,43,48,16,6,35,46,
		47,50,51,12,33,
		5,250,255,255,252,251,
		99,38,13,11,10,
		0
};




#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct DNS_OPT
{
	unsigned char name;
	unsigned short type;
	unsigned short udp_payload_size;
	unsigned char extended_rcode;
	unsigned char edns0_version;
	unsigned short z;
	unsigned short data_length;
};
#pragma pack(pop)   /* restore original alignment from stack */


int MakeQuery(const ppl7::String &query, unsigned char *buffer, size_t buffersize, bool dnssec, int udp_payload_size)
{
	ppl7::Array tok(query," ");
	if (tok.size()!=2) throw InvalidDNSQuery(query);
	ppl7::String Type=tok[1].toUpperCase();

	int t=0;
	const char *str=Type.c_str();
	while (rr_types[t]!=NULL) {
		if(!strcmp(str,rr_types[t])) {
			int bytes=res_mkquery(QUERY,
					(const char*) tok[0],
					C_IN,
					rr_code[t],
					NULL,0,NULL,buffer,(int)buffersize);
			if (bytes<0) throw InvalidDNSQuery("%s", hstrerror(h_errno));
			if (!dnssec) return bytes;
			return AddDnssecToQuery(buffer,buffersize,bytes,udp_payload_size);
		}
		t++;
	}
	throw UnknownRRType(tok[1]);
}

int AddDnssecToQuery(unsigned char *buffer, size_t buffersize, int querysize, int udp_payload_size)
{
	DNS_HEADER *dns=(DNS_HEADER*)buffer;
	dns->ad=1;
	dns->add_count=htons(1);
	DNS_OPT *opt=(DNS_OPT*)(buffer+querysize);
	memset(opt,0,11);
	opt->type=htons(41);
	opt->udp_payload_size=htons(udp_payload_size);
	opt->z=htons(0x8000);	// DO-bit
	return querysize+11;
}

