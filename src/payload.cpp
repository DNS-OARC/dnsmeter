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

/*!\brief Query-File öffnen
 *
 * Öffnet eine Datei, in der die zu sendenden Queries stehen. Diese
 * müssen folgendes Format aufweisen:
 * * jeder Zeile enthält eine Abfrage
 * * Query beginnt mit den abzufragenden Daten, gefolgt von 1 x Space, gefolgt
 * vom Record-Typ
 *
 * Beispiel: denic.de NS
 *
 * @param Filename String mit dem Dateinamen
 * @exception Diverse Es können diverse Exceptions georfen werden, falls die Datei
 * nicht geöffnet werden kann.
 */
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
