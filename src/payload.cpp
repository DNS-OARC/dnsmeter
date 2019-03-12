/*
 * This file is part of dnspecker by Patrick Fedick <fedick@denic.de>
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

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include <list>


#include "dnspecker.h"



PayloadFile::PayloadFile()
{
	validLinesInQueryFile=0;
}

void PayloadFile::openQueryFile(const ppl7::String &Filename)
{
	if (Filename.isEmpty()) throw InvalidQueryFile("File not given");
	ppl7::File QueryFile;
	QueryFile.open(Filename,ppl7::File::READ);
	if (QueryFile.size()==0) {
		throw InvalidQueryFile("File is empty [%s]", (const char*)Filename);
	}
	printf ("INFO: Loading and precompile payload. This could take some time...\n");
	loadAndCompile(QueryFile);
	printf ("INFO: %llu queries loaded\n",validLinesInQueryFile);
	it=querycache.begin();
}

void PayloadFile::loadAndCompile(ppl7::File &ff)
{
	ppl7::ByteArray buf(4096);
	ppl7::String buffer;
	validLinesInQueryFile=0;
	unsigned char *compiled_query=(unsigned char *)buf.ptr();
	while (1) {
		try {
			if (ff.eof()) throw ppl7::EndOfFileException();
			ff.gets(buffer,1024);
			buffer.trim();
			if (buffer.isEmpty()) continue;
			if (buffer.c_str()[0]=='#') continue;
			try {
				// Precompile Query
				int size=MakeQuery(buffer,compiled_query,4096,false);
				querycache.push_back(ppl7::ByteArray(compiled_query,size));
				validLinesInQueryFile++;
			} catch (...) {
				// ignore invalid queries
			}
		} catch (const ppl7::EndOfFileException &) {
			if (validLinesInQueryFile==0) {
				throw InvalidQueryFile("No valid Queries found in Queryfile");
			}
			return;
		}
	}
}


const ppl7::ByteArrayPtr PayloadFile::getQuery()
{
	ppl7::ByteArrayPtr bap;
	QueryMutex.lock();
	bap=*it;
	++it;
	if (it==querycache.end()) it=querycache.begin();
	QueryMutex.unlock();
	return bap;
}
