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
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include "ppl7.h"


namespace ppl7 {

/*!\class MemoryGroup
 * \ingroup PPLGroupMemory
 * \brief Speicherverwaltung: MemSpace
 *
 * \desc
 * MemoryGroup ist ein Wrapper für Malloc & Co. der libc-Bibliothek und dient zur Gruppierung
 * von Speicher. Die Klasse merkt sich jeden über sie allokierten Speicherbereich in einer
 * Liste und kann dadurch sämtlichen Speicher auf einmal über den Destruktor oder durch
 * Aufruf von MemoryGroup::clear wieder löschen.
 * \par
 * Durch Aufruf von MemoryGroup::malloc, MemoryGroup:calloc oder MemoryGroup::strdup wird
 * Speicher allokiert. Mit MemoryGroup:realloc kann ein existierender Speicherblock vergrößert
 * oder verkleinert werden. Mit MemoryGroup::free wird ein Speicherblock wieder freigegeben.
 * Mit MemoryGroup::clear werden alle über diese Klasse allokierten Speicherbereiche auf einmal
 * freigegeben.
 * \attention
 * Speicher, der durch diese Klasse allokiert wurde, darf nicht mit der %free-Funktion der
 * libc freigegeben werden und umgekehrt.
 * \note
 * Die Klasse entspricht funktional der Klasse CMemSpace aus Version 6 der PPLib.
 */

typedef struct tagMemGroup {
	void *adr;
	size_t size;
	struct tagMemGroup *previous, *next;
} MEMGROUP;

static size_t mysize=(sizeof(MEMGROUP)+15)&(((size_t)-1)^15);


/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Der Konstruktor initialisiert die internen Variablen.
 *
 */
MemoryGroup::MemoryGroup()
{
	first=last=NULL;
	totalSize=sizeof(MemoryGroup);
	totalBlocks=0;
}

/*!\brief Destruktor der Klasse
 *
 * \desc
 * Der Destruktor ruft die Funktion MemoryGroup::clear auf und stellt somit sicher, dass
 * sämtlicher durch die Klasse allokierte Speicher wieder freigegeben wird.
 */
MemoryGroup::~MemoryGroup()
{
	clear();
}


/*!\brief Kompletten Speicher freigeben
 *
 * \desc
 * Durch Aufruf dieser Funktion werden sämtliche Speicherblöcke freigegeben, die über diese
 * Klasse allokiert wurden. Eventuell noch vorhandene Pointer auf Speicherbereiche dieser
 * Klasse werden ungültig.
 */
void MemoryGroup::clear()
{
	MEMGROUP *m;
	while (first) {
		m=(MEMGROUP *)first;
		first=m->next;
		free(m);
	}
	first=last=NULL;
	totalSize=sizeof(MemoryGroup);
	totalBlocks=0;
}

/*!\brief Speicherblock zur Liste hinzufügen
 *
 * \desc
 * Mit dieser internen Funktion wird ein Speicherblock zur Verwaltungsliste hinzugefügt.
 * Ausserdem werden die internen Zähler für Anzahl Speicherblöcke und Gesamtspeichergröße
 * hochgezählt.
 *
 * @param[in] block Pointer auf die Verwaltungsstruktur des Speicherblocks
 */
void MemoryGroup::addToList(void *block)
{
	MEMGROUP* m=(MEMGROUP*)block;
	m->previous=(MEMGROUP*)last;
	m->next=NULL;
	if (last) ((MEMGROUP*)last)->next=m;
	last=m;
	if (!first) first=m;
	totalSize+=m->size+mysize;
	totalBlocks++;
}

/*!\brief Speicherblock aus der Liste entfernen
 *
 * \desc
 * Mit dieser internen Funktion wird ein Speicherblock aus der Verwaltungsliste entfernt.
 * Ausserdem werden die internen Zähler für Anzahl Speicherblöcke und Gesamtspeichergröße
 * runtergezählt.
 *
 * @param[in] block Pointer auf die Verwaltungsstruktur des Speicherblocks
 */
void MemoryGroup::removeFromList(void *block)
{
	MEMGROUP* m=(MEMGROUP*)block;
	if (m->previous) m->previous->next=m->next;
	if (m->next) m->next->previous=m->previous;
	if (m==first) first=m->next;
	if (m==last) last=m->previous;
	totalSize-=(m->size+mysize);
	totalBlocks--;
}


/*!\brief Speicher allokieren
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock allokiert.
 *
 * @param size Gewünschte Größe des Speicherblocks in Bytes
 * \return Pointer auf den Beginn des Speicherblocks
 * \exception OutOfMemoryException wird geworfen, wenn kein Speicher mehr frei ist
 */
void *MemoryGroup::malloc(size_t size)
{
	if (!size) throw IllegalArgumentException();
	MEMGROUP *m=(MEMGROUP*)::malloc(mysize+size);
	if (!m) throw OutOfMemoryException();
	m->adr=(char*)m+mysize;
	m->size=size;
	addToList(m);
	return m->adr;
}

/*!\brief Speicher allokieren und löschen
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock allokiert und mit 0-Bytes initialisiert.
 *
 * @param size Gewünschte Größe des Speicherblocks in Bytes
 * \return Pointer auf den Beginn des Speicherblocks
 * \exception OutOfMemoryException wird geworfen, wenn kein Speicher mehr frei ist
 */
void *MemoryGroup::calloc(size_t size)
{
	if (!size) throw IllegalArgumentException();
	MEMGROUP *m=(MEMGROUP*)::calloc(1,mysize+size);
	if (!m) throw OutOfMemoryException();
	m->adr=(char*)m+mysize;
	m->size=size;
	addToList(m);
	return m->adr;
}

/*!\brief String kopieren
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock allokiert, der groß genug ist, um
 * den String \p str aufzunehmen. Anschließend wird der String in den neuen Speicherblock kopiert.
 *
 * @param str Pointer auf einen C-String, der mit einem 0-Byte terminiert sein muß.
 * \return Pointer auf den Beginn des Speicherblocks
 * \exception OutOfMemoryException wird geworfen, wenn kein Speicher mehr frei ist
 */
char *MemoryGroup::strdup(const char *str)
{
	if (!str) throw NullPointerException();
	size_t size=strlen(str)+1;
	MEMGROUP *m=(MEMGROUP*)::malloc(mysize+size);
	if (!m) throw OutOfMemoryException();
	m->adr=(char*)m+mysize;
	memcpy(m->adr,str,size);
	((char*)m->adr)[size]=0;
	m->size=size;
	addToList(m);
	return (char*)m->adr;
}

/*!\brief Teilstring kopieren
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock mit der Größe \p size allokiert. Anschließend
 * werden die ersten \p size Bytes des Strings \p str in den neuen Speicherblock kopiert.
 *
 * @param str Pointer auf einen C-String
 * @param size Anzahl Bytes, die kopiert werden sollen
 * \return Pointer auf den Beginn des Speicherblocks
 * \exception OutOfMemoryException wird geworfen, wenn kein Speicher mehr frei ist
 */
char *MemoryGroup::strndup(const char *str, size_t size)
{
	if (!str) throw NullPointerException();
	MEMGROUP *m=(MEMGROUP*)::malloc(mysize+size+1);
	if (!m) throw OutOfMemoryException();
	m->adr=(char*)m+mysize;
	memcpy(m->adr,str,size);
	((char*)m->adr)[size]=0;
	m->size=size+1;
	addToList(m);
	return (char*)m->adr;
}

/*!\brief Speicherblock verkleinern oder vergrößern
 *
 * \desc
 * Mit dieser Funktion wird ein vorhandener Speicherblock verkleinert oder vergrößert. Der
 * Speicherblock \p adr muß zuvor über diese Klasse allokiert worden sein.
 *
 * @param adr Pointer auf vorhandenen Speicherblock
 * @param size Neue Größe in Bytes
 * \return Pointer auf den Beginn des neuen Speicherblocks
 * \exception OutOfMemoryException wird geworfen, wenn kein Speicher mehr frei ist
 * \exception IllegalMemoryAddressException wird geworfen, wenn der Speicherblock nicht über diese
 * Klasse allokiert wurde
 * \exception NullPointerException wird geworfen, wenn \p adr auf NULL zeigt
 */
void *MemoryGroup::realloc(void *adr, size_t size)
{
	if (!adr) throw NullPointerException();
	MEMGROUP *m=(MEMGROUP*)((char*)adr-mysize);
	// Sicherstellen, dass der Speicherblock von dieser Klasse verwaltet wird
	if (m->adr!=adr)  throw IllegalMemoryAddressException();
	MEMGROUP *old=m;
	removeFromList(old);
	m=(MEMGROUP*)::realloc(m,mysize+size);
	if (!m) {
		addToList(old);
		throw OutOfMemoryException();
	}
	m->size=size;
	m->adr=(char*)m+mysize;
	addToList(m);
	return m->adr;
}

/*!\brief Speicherblock freigeben
 *
 * \desc
 * Mit dieser Funktion wird ein vorhandener Speicherblock wieder freigegeben.
 * der Speicherblock \p adr muß zuvor über diese Klasse allokiert worden sein.
 * @param adr Pointer auf vorhandenen Speicherblock
 * \exception IllegalMemoryAddressException wird geworfen, wenn der Speicherblock nicht über diese
 * Klasse allokiert wurde
 * \exception NullPointerException wird geworfen, wenn \p adr auf NULL zeigt
 */
void MemoryGroup::free(void *adr)
{
	if (!adr) throw NullPointerException();
	MEMGROUP *m=(MEMGROUP*)((char*)adr-mysize);
	// Sicherstellen, dass der Speicherblock von dieser Klasse verwaltet wird
	if (m->adr!=adr)  throw IllegalMemoryAddressException();
	removeFromList(m);
	::free(m);
}

/*!\brief Anzahl Speicherblöcke
 *
 * \desc
 * Mit dieser Funktion kann abgefragt werdenm, wieviele Speicherblöcke zur Zeit von der
 * Klasse allokiert sind.
 *
 * @return Anzahl Speicherblöcke
 */
size_t MemoryGroup::count() const
{
	return totalBlocks;
}

/*!\brief Belegter Speicher
 *
 * \desc
 * Diese Funktion liefert die Anzahl Bytes zurück, die aktuell von der Klasse belegt werden.
 * Diese errechnen sich aus dem Speicher für die Klasse selbst (=sizeof(MemoryGroup)) plus die
 * allokierten Speicherblöcke und die Verwaltungsstruktur pro Speicherblock.
 *
 * @return Belegter Speicher in Bytes
 */
size_t MemoryGroup::size() const
{
	return totalSize;
}


}	// EOF namespace ppl7
