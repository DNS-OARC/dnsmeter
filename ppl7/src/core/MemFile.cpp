/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: http://www.pfp.de/ppl/
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *******************************************************************************
 * Copyright (c) 2013, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/


#include "prolog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
	#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
	#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_FILE_H
	#include <sys/file.h>
#endif
#ifdef HAVE_STDARG_H
	#include <stdarg.h>
#endif
#ifdef HAVE_ERRNO_H
	#include <errno.h>
#endif
#ifdef _WIN32
#include <io.h>
#define WIN32_LEAN_AND_MEAN		// Keine MFCs
#define popen _popen
#define pclose _pclose

#include <windows.h>
#ifdef DeleteFile
#undef DeleteFile
#endif
#endif
#include "ppl7.h"

namespace ppl7 {

#define COPYBYTES_BUFFERSIZE 1024*1024

/*!\class MemFile
 * \ingroup PPLGroupFileIO
 * \brief Simulation von Dateizugriffen im Hauptspeicher
 *
 * \desc
 * Mit dieser von FileObject abgeleiteten Klasse werden Dateizugriffe im Hauptspeicher simuliert.
 * Sie kann immer dann verwendet werden, wenn sich die zu lesende Datei bereits im Hauptspeicher
 * befindet, oder Daten temporär im Hauptspeicher abgelegt werden sollen.
 * \par
 * Der zu verwendende Speicherbereich kann entweder über den Konstruktor abgegeben werden (siehe
 * MemFile::MemFile (void * adresse, size_t size) ) oder über die Funktion MemFile::open. Soll
 * der Speicherbereich auch beschrieben werden, muss als dritter Parameter "true" angegeben
 * oder die Funktion MemFile::openReadWrite verwendet werden.
 */

/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Durch Verwendung dieses Konstruktors wird die Klasse zum Lesen und Schreiben geöffnet, wobei
 * der Speicherbereich initial 0 Byte gross ist. Beim ersten Schreibzugriff wird der notwendige
 * Speicher allokiert.
 */
MemFile::MemFile ()
{
	buffer=NULL;
	mysize=0;
	pos=0;
	MemBase=NULL;
	readonly=false;
	maxsize=0;
	buffersize=0;
}


MemFile::MemFile (void * adresse, size_t size, bool writeable)
/*!\brief Konstruktor der Klasse mit Angabe eines Speicherbereichs
 *
 * \desc
 * Mit diesem Konstruktor wird gleichzeitig ein Pointer auf den Speicherbereich \p adresse mit einer
 * Größe von \p size Bytes übergeben. Sämtliche Dateizugriffe werden in diesem Speicherbereich
 * simuliert.
 *
 * @param adresse Pointer auf den zu verwendenden Speicherbereich
 * @param size Größe des Speicherbereichs
 * @param writeable Gibt an, ob der Speicherbereich auch beschreibbar sein soll.
 * @attention Wird der Parameter \p writeable auf "true" gesetzt, geht die Verwaltung des
 * Speichers an die MemFile-Klasse über. Der Speicher darf nicht mehr von der Applikation verändert
 * oder freigegeben werden!
 */
{
	mysize=0;
	pos=0;
	MemBase=NULL;
	readonly=false;
	maxsize=0;
	buffersize=0;
	open(adresse,size,writeable);
}

/*!\brief Konstruktor der Klasse mit Angabe eines Speicherbereichs
 *
 * Mit diesem Konstruktor wird gleichzeitig ein Pointer auf den Speicherbereich \p adresse mit einer
 * Größe von \p size Bytes übergeben. Sämtliche Dateizugriffe werden in diesem Speicherbereich
 * simuliert. Ein Schreibzugriff auf diesen Speicherbereich ist nicht möglich.
 *
 * @param adresse Pointer auf den zu verwendenden Speicherbereich
 * @param size Größe des Speicherbereichs
 */
MemFile::MemFile (const ByteArrayPtr &memory)
{
	if (memory.isEmpty()) {
		throw IllegalArgumentException();
	}
	MemBase=(char*)memory.adr();
	mysize=memory.size();
	pos=0;
	readonly=true;
	maxsize=0;
	buffersize=0;
}

MemFile::~MemFile()
{
}

/*!\brief Speicherbereich zum Lesen öffnen
 *
 * Mit dieser Funktion wird die simulierte Datei im Hauptspeicher geöffnet. Dazu muss mit
 * \p adresse ein Pointer auf den Beginn des zu verwendenden Hauptspeichers angegeben werden,
 * sowie mit \p size seine Größe. Sämtliche nachfolgenden Dateizugriffe werden dann in diesem
 * Speicherbereich simuliert. Ein Schreibender Zugriff ist nicht möglich.
 *
 * @param adresse Pointer auf den zu verwendenden Speicherbereich
 * @param size Größe des Speicherbereichs
 * @attention Wird der Parameter \p writeable auf "true" gesetzt, geht die Verwaltung des
 * Speichers an die MemFile-Klasse über. Der Speicher darf nicht mehr von der Applikation verändert
 * oder freigegeben werden!
 */
void MemFile::open (void * adresse, size_t size, bool writeable)
{
	//if (adresse==NULL || size==0) throw IllegalArgumentException();
	if (buffer) {
		free(buffer);
		buffer=NULL;
		buffersize=0;
	}
	MemBase=(char*)adresse;
	mysize=size;
	pos=0;

	if (writeable==true) {
		buffer=MemBase;
		readonly=false;
		buffersize=size;
	} else {
		buffersize=0;
		readonly=true;
	}
}

/*!\brief Speicherbereich zum Lesen öffnen
 *
 * Mit dieser Funktion wird die simulierte Datei im Hauptspeicher zum Lesen geöffnet. Dazu muss
 * mit \p adresse ein Pointer auf den Beginn des zu verwendenden Hauptspeichers angegeben werden,
 * sowie mit \p size seine Größe. Sämtliche nachfolgenden Dateizugriffe werden dann in diesem
 * Speicherbereich simuliert. Ein Schreibender Zugriff ist nicht möglich.
 *
 * @param memory Referenz auf eine ByteArrayPtr-Klasse, die den zu verwendenden Speicherbereich enthält
 * @see openReadWrite: Datei zum Lesen und Schreiben öffnen
 */
void MemFile::open(const ByteArrayPtr &memory)
{
	if (memory.isEmpty()) throw IllegalArgumentException();
	if (buffer) {
		free(buffer);
		buffer=NULL;
	}
	MemBase=(char*)memory.adr();
	mysize=memory.size();
	pos=0;
	readonly=true;
}

/*!\brief Speicherbereich zum Schreiben und Lesen öffnen
 *
 * Mit dieser Funktion wird die simulierte Datei im Hauptspeicher zum Lesen und Schreiben
 * geöffnet. Dazu muss mit \p adresse ein Pointer auf den Beginn des zu verwendenden
 * Hauptspeichers angegeben werden,
 * sowie mit \p size seine initiale Größe. Sämtliche nachfolgenden Dateizugriffe werden
 * dann in diesem Speicherbereich simuliert. Erfolgt ein schreibender Zugriff über dessen
 * Ende hinaus, wird der Speicherbereich automatisch vergrößert.
 *
 * @param adresse Pointer auf den zu verwendenden Speicherbereich
 * @param size Größe des Speicherbereichs
 * @see open: Datei wird nur zum Lesen geöffnet
 * @see setMaxSize: Legt die maximale Größe der Datei im Speicher fest (Default=unlimitiert)
 */
void MemFile::openReadWrite(void * adresse, size_t size)
{
	if (adresse==NULL || size==0) throw IllegalArgumentException();
	if (buffer) free(buffer);
	MemBase=(char*)adresse;
	buffer=MemBase;
	mysize=size;
	pos=0;
	readonly=false;
	buffersize=size;
}

/*!\brief Maximale Dateigröße festlegen
 *
 * \desc
 * Mit dieser Funktion wird die maximale Größe einer Datei im Hauptspeicher festgelegt.
 * Damit werden alle Schreibenden Zugriffe begrenzt, die Datei kann nicht größer werden
 * als \p size. Standardmäßig gibt es keine Limitierung, die Datei kann somit so groß werden,
 * wie Hauptspeicher zur Verfügung steht.
 *
 * \param[in] size Maximale Größe in Bytes. Der Wert "0" hebt die Limitierung auf.
 * \see Mit der Funktion MemFile::maxSize kann das derzeitige Limit ausgelesen werden.
 */
void MemFile::setMaxSize(size_t size)
{
	maxsize=size;
}

size_t MemFile::maxSize() const
{
	return maxsize;
}

void MemFile::resizeBuffer(size_t size)
{
	if (readonly) throw ReadOnlyException();
	if (maxsize>0 && size>maxsize) throw BufferExceedsLimitException();
	size=(size&8191)+8192;	// Round on 8KB
	if (size>buffersize) {
		char *buf=(char*)realloc(buffer,size);
		if (!buf) throw OutOfMemoryException();
		buffer=buf;
		MemBase=buf;
		buffersize=size;
	}
	mysize=size;
	if (pos>mysize) pos=mysize;
}

bool MemFile::isOpen() const
{
	if (MemBase!=NULL) return true;
	return false;
}

void MemFile::close()
{
	MemBase=NULL;
	mysize=0;
	pos=0;
	if (buffer!=0) {
		free (buffer);
		buffer=0;
	}
}

ppluint64 MemFile::size () const
{
	return (pplint64)mysize;
}

void MemFile::rewind ()
{
	pos=0;
}


void MemFile::seek(ppluint64 position)
{
	if (MemBase!=NULL || readonly==false) {
		if (position<mysize) {
			pos=position;
		} else if (mysize==0 && position==0) {
			return;
		} else {
			throw OverflowException();
		}
		return;
	}
	throw FileNotOpenException();
}

ppluint64 MemFile::seek (pplint64 offset, SeekOrigin origin )
{
	if (MemBase!=NULL || readonly==false) {
		ppluint64 oldpos=pos;
		switch (origin) {
			case SEEKCUR:
				pos+=offset;
				if (pos<mysize) return pos;
				if ((pplint64)pos<0) {pos=0;return pos; }
				break;
			case SEEKEND:
				pos=mysize-offset;
				if (pos>mysize) return pos;
				if ((pplint64)pos<0) {pos=0; return pos;}
				break;
			case SEEKSET:
				pos=offset;
				if ((pplint64)pos<0) {pos=0; return pos;}
				if (pos>mysize) return pos;
				break;
		}
		pos=oldpos;
		throw FileSeekException("pos=%lld, offset=%lld, origin=%d",pos, offset, origin);
	}
	throw FileNotOpenException();
}

ppluint64 MemFile::tell()
{
	if (MemBase!=NULL || readonly==false) {
		return pos;
	}
	throw FileNotOpenException();
}


size_t MemFile::fread(void *ptr, size_t size, size_t nmemb)
{
	if (MemBase==NULL) throw FileNotOpenException();
	size_t by=nmemb;
	if (pos+(by*size)>mysize) by=(size_t)(mysize-pos)/size;
	memmove(ptr,MemBase+pos,by*size);
	pos+=(by*size);
	if (by<nmemb) throw ReadException();
	return by;
}

size_t MemFile::fwrite(const void *ptr, size_t size, size_t nmemb)
{
	if (MemBase==NULL && readonly==true) throw FileNotOpenException();
	if (readonly) throw ReadOnlyException();
	size_t bytes=nmemb*size;
	if (pos+bytes>mysize) resizeBuffer(pos+bytes);
	memmove(MemBase+pos,ptr,bytes);
	pos+=bytes;
	return bytes;
}

char *MemFile::fgets (char *buffer1, size_t num)
{
	if (MemBase!=NULL) {
		if (pos>=mysize) throw EndOfFileException();
		ppluint64 by;
		by=num-1;
		if (pos+by>mysize) by=(ppluint64)(mysize-pos);
		char *ptr=MemBase+pos;
		ppluint64 i;
		for (i=0;i<by;i++) {
			if ((buffer1[i]=ptr[i])=='\n') {
				i++;
				break;
			}
		}
		buffer1[i]=0;
		pos+=i;
		if (pos>=mysize) throw EndOfFileException();
		return buffer1;
	}
	throw FileNotOpenException();
}

wchar_t *MemFile::fgetws (wchar_t *buffer1, size_t num)
{
	if (MemBase!=NULL) {
		if (pos>=mysize) throw EndOfFileException();
		//ppluint64 by;
		//by=(num-1)*sizeof(wchar_t);
		//if (pos+by>mysize) by=(ppluint64)(mysize-pos);
		wchar_t *ptr=(wchar_t*)(MemBase+pos);
		ppluint64 i;
		for (i=0;i<(num-1);i++) {
			if ((buffer1[i]=ptr[i])==L'\n') {
				i++;
				break;
			}
		}
		buffer1[i]=0;
		pos+=(i*sizeof(wchar_t));
		if (pos>=mysize) throw EndOfFileException();
		return buffer1;
	}
	throw FileNotOpenException();
}

void MemFile::fputs (const char *str)
{
	if (MemBase!=NULL || readonly==false) {
		fwrite ((void*)str,1,(ppluint32)strlen(str));
		return;
	}
	throw FileNotOpenException();
}

void MemFile::fputws (const wchar_t *str)
{
	if (MemBase!=NULL || readonly==false) {
		fwrite (str,1,(ppluint32)wcslen(str)*sizeof(wchar_t));
		return;
	}
	throw FileNotOpenException();
}

void MemFile::fputc(int c)
{
	char buf[1];
	buf[0]=c;
	fwrite(buf,1,1);
}

void MemFile::fputwc(wchar_t c)
{
	wchar_t buf[1];
	buf[0]=c;
	fwrite(buf,sizeof(wchar_t),1);
}

int MemFile::fgetc()
{
	if (MemBase==NULL) throw FileNotOpenException();
	if (pos>mysize) throw OverflowException();
	return MemBase[pos++];
}

wchar_t MemFile::fgetwc()
{
	wchar_t buf[1];
	fread(buf,sizeof(wchar_t),1);
	return buf[0];
}


bool MemFile::eof() const
{
	if (MemBase!=NULL || readonly==false) {
		if (pos>=mysize) return true;
		return false;

	}
	throw FileNotOpenException();
}

char *MemFile::adr(size_t adresse)
{
	if (MemBase!=NULL) {
		return (MemBase+adresse);
	}
	throw FileNotOpenException();
}

void MemFile::setMapReadAhead(size_t bytes)
{

}


const char *MemFile::map(ppluint64 position, size_t bytes)
{
	if (MemBase==NULL) throw FileNotOpenException();
	if (position+bytes<=mysize) {
		return (MemBase+position);
	}
	return NULL;
}

char *MemFile::mapRW(ppluint64 position, size_t bytes)
{
	if (MemBase==NULL) throw FileNotOpenException();
	if (position+bytes<=mysize) {
		return (MemBase+position);
	}
	return NULL;
}

void MemFile::unmap()
{
	return;
}


void MemFile::flush()
{
	return;
}

void MemFile::sync()
{
	return;
}

/*!\copybrief FileObject::getFileNo
 *
 * \desc
 * Diese Funktion steht bei bei dieser Speicherklasse nicht zur Verfügung. Bei
 * Aufruf der Funktion wird eine OperationUnavailableException geworfen.
 *
 */
int MemFile::getFileNo() const
{
	throw OperationUnavailableException();
}


void MemFile::truncate(ppluint64 length)
{
	if (readonly) throw ReadOnlyException();
	if (length<mysize) {
		resizeBuffer(length);
		return;
	} else if (length==mysize) return;
	size_t oldsize=mysize;
	size_t increase=length-mysize;
	resizeBuffer(length);
	memset(MemBase+oldsize,0,increase);
}

/*!\copybrief FileObject::lockShared
 *
 * \desc
 * Diese Funktion steht bei bei dieser Speicherklasse nicht zur Verfügung. Bei
 * Aufruf der Funktion wird eine OperationUnavailableException geworfen.
 *
 */
void MemFile::lockShared(bool block)
{
	throw OperationUnavailableException();
}

/*!\copybrief FileObject::lockExclusive
 *
 * \desc
 * Diese Funktion steht bei bei dieser Speicherklasse nicht zur Verfügung. Bei
 * Aufruf der Funktion wird eine OperationUnavailableException geworfen.
 *
 */
void MemFile::lockExclusive(bool block)
{
	throw OperationUnavailableException();
}

/*!\copybrief FileObject::unlock
 *
 * \desc
 * Diese Funktion steht bei bei dieser Speicherklasse nicht zur Verfügung. Bei
 * Aufruf der Funktion wird eine OperationUnavailableException geworfen.
 *
 */
void MemFile::unlock()
{
	throw OperationUnavailableException();
}



} // end of namespace ppl7
