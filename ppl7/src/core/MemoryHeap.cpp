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

typedef struct tagHeapElement {
	struct tagHeapElement	*next, *previous;
	void					*ptr;
} HEAPELEMENT;

typedef struct tagHeapBlock {
	struct tagHeapBlock	*previous, *next;
	void				*buffer;
	void				*bufferend;
	size_t				elements;
	size_t				num_free;
	HEAPELEMENT			*free;
	HEAPELEMENT			*elbuffer;

} HEAPBLOCK;


/*!\class MemoryHeap
 * \ingroup PPLGroupMemory
 * \brief Speicherverwaltung in Heaps
 *
 * Diese Klasse kann verwendet werden, wenn häufig kleine gleichgroße Speicherblöcke allokiert
 * und wieder freigegeben werden müssen, z.B. von einer häufig verwendeten Klasse oder
 * Kontrollstrukturen für Listen und binäre Bäume. Statt den Speicherbereich jedesmal
 * per Malloc oder new vom Betriebssystem anzufordern, verwaltet diese Klasse eigene
 * größere Speicherblöcke, aus denen die malloc-Anfragen bedient werden. Dadurch wird
 * verhindert, dass der Speicher zu sehr fragmentiert wird.
 *
 * Bevor die Klasse verwendet werden kann, muss sie zunächst mittels Konstruktor oder
 * der Funktion MemoryHeap::init initialisiert werden. Dabei muss insbesondere die Größe
 * der Elemente angegeben werden und die Anzahl Elemente, um die der Heap jeweils wachsen
 * soll, wenn kein Speicher mehr frei ist. Initial kann dabei auch schon Speicher allokiert
 * werden.
 *
 * Die Wachstumsgröße selbst wächst bei jeder Vergrößerung um 30%.
 *
 */


/*!\brief Konstruktor
 *
 * \desc
 * Bei Verwendung dieses Konstruktors muss anschließend die Funktion MemoryHeap::init
 * aufgerufen werden.
 *
 */
MemoryHeap::MemoryHeap()
{
	blocks=NULL;
	myElementSize=0;
	increaseSize=0;
	blocksAllocated=0;
	blocksUsed=0;
	freeCount=0;
	myGrowPercent=30;
	mem_allocated=sizeof(MemoryHeap);
	mem_used=sizeof(MemoryHeap);
}

/*!\brief Konstruktor mit Initialisierung
 *
 * \desc
 * Bei Verwendung dieses Konstruktors wird die Klasse gleichzeitig auch initialisiert.
 *
 * @param elementsize Die Größe der Elemente in Bytes (wird auf 4 Byte aufgerundet)
 * @param startnum Anzahl Elemente, für die sofort Speicher allokiert werden soll
 * @param increase Anzahl Elemente, um die der Heap wachsen soll, wenn keine Elemente mehr
 * frei sind.
 * @param growpercent Optional: Wachstumsrate der Speichervergrößerung (Default=30%)
 * \exception OutOfMemoryException: Wird geworfen, wenn nicht genug Speicher verfügbar ist, um den
 * Heap anzulegen.
 */
MemoryHeap::MemoryHeap(size_t elementsize, size_t startnum, size_t increase, size_t growpercent)
{
	blocks=NULL;
	myElementSize=0;
	increaseSize=0;
	blocksAllocated=0;
	blocksUsed=0;
	freeCount=0;
	myGrowPercent=growpercent;
	mem_allocated=sizeof(MemoryHeap);
	mem_used=sizeof(MemoryHeap);
	init(elementsize, startnum, increase);
}

/*!\brief Destruktor
 *
 * \desc
 * Der Destruktor sorgt dafür, dass der komplette durch den Heap belegte Speicher
 * freigegeben wird.
 */
MemoryHeap::~MemoryHeap()
{
	clear();
}

/*!\brief Gesamten Speicher freigeben
 *
 * \desc
 * Sämtlicher durch den Heap belegte Speicher wird freigegeben. Alle durch
 * MemoryHeap::malloc oder Heap:calloc allokierten Speicherblöcke verlieren ihre Gültigkeit und
 * dürfen nicht mehr verwendet werden.
 *
 */
void MemoryHeap::clear()
{
	HEAPBLOCK *next, *bl=(HEAPBLOCK*)blocks;
	while (bl) {
		next=bl->next;
		::free(bl->elbuffer);
		::free(bl->buffer);
		::free(bl);
		bl=next;
	}
	freeCount=0;
	blocksAllocated=0;
	blocksUsed=0;
	blocks=NULL;
	mem_allocated=sizeof(MemoryHeap);
	mem_used=sizeof(MemoryHeap);
}

/*!\brief Derzeitige Kapazität des Heaps
 *
 * \desc
 * Mit dieser Funktion kann abgefragt werden wieviele Elemente insgesamt allokiert werden
 * können, ohne dass neue Speicherblöcke vom Betriebssystem angefordert werden müssen.
 * Wieviele davon tatsächlich schon verbraucht sind, kann mittels MemoryHeap::count() abgefragt
 * werden.
 *
 * @return Anzahl Elemente, für die Speicher vorrätig ist
 */
size_t MemoryHeap::capacity() const
{
	return blocksAllocated;
}

/*!\brief Anzahl belegter Elemente
 *
 * \desc
 * Liefert die Anzahl Elemente zurück, die derzeit in Verwendung sind.
 *
 * @return Anzahl Elemente
 */
size_t MemoryHeap::count() const
{
	return blocksUsed;
}

/*!\brief Größe der Elemente
 *
 * \desc
 * Liefert die Größe eines Elementes in Bytes zurück
 *
 * @return Größe in Bytes
 */
size_t MemoryHeap::elementSize() const
{
	return myElementSize;
}

/*!\brief Speicher reservieren
 *
 * \desc
 * Mit dieser Funktion kann vorab Speicher für eine bestimmte Anzahl Elemente reserviert werden.
 * Der Aufruf dieser Funktion ist immer dann sinnvoll, wenn schon vorher bekannt ist, wieviele
 * Elemente benötigt werden, insbesondere, wenn sehr viele Elemente benötigt werden
 * (z.B. Aufbau eines binären Baums).
 *
 * @param num Anzahl Elemente, für die Speicher vorab allokiert werden soll
 *
 * \note Falls schon Speicher allokiert wurde, wird die Anzahl der bereits allokierten Elemente
 * mit \p num verrechnet und nur die Differenz zusätzlich reserviert.
 */
void MemoryHeap::reserve(size_t num)
{
	if (num>blocksAllocated) {
		size_t grow=num-blocksAllocated;
		increase(grow);
	}
}

/*!\brief Initialisierung der Klasse
 *
 * \desc
 * Diese Funktion muss vor allen anderen aufgerufen werden, um die Klasse zu initialisieren.
 * Dabei muss insbesondere die Größe der Elemente angegeben werden und die Anzahl Elemente,
 * um die der Heap jeweils wachsen soll, wenn kein Speicher mehr frei ist. Initial kann dabei
 * auch schon Speicher allokiert werden.
 *
 * @param elementsize Die Größe der Elemente in Bytes (wird auf 4 Byte aufgerundet)
 * @param startnum Anzahl Elemente, für die sofort Speicher allokiert werden soll
 * @param increase Anzahl Elemente, um die der Heap wachsen soll, wenn keine Elemente mehr
 * frei sind.
 * @param growpercent Optional: Wachstumsrate der Speichervergrößerung (Default=30%)
 * \exception OutOfMemoryException: Wird geworfen, wenn nicht genug Speicher verfügbar ist, um den
 * Heap anzulegen.
 */
void MemoryHeap::init(size_t elementsize, size_t startnum, size_t increase, size_t growpercent)
{
	if (myElementSize) throw AlreadyInitializedException();
	//Elementsize auf 4 Byte aufrunden
	if (!elementsize) throw IllegalArgumentException("elementsize");
	elementsize=(elementsize+3)&0xfffffffc;

	this->myElementSize=elementsize;
	this->increaseSize=increase;
	myGrowPercent=growpercent;
	if (startnum) this->increase(startnum);
}

/*!\brief Heap vergrößern
 *
 * \desc
 * Interne Funktion, die aufgerufen wird, um den Heap um eine bestimmte Anzahl Elemente zu
 * vergrößern.
 *
 * @param num Anzahl Elemente, für die neuer Speicher allokiert werden soll
 * \exception OutOfMemoryException: Wird geworfen, wenn nicht genug Speicher verfügbar ist, um den
 * Heap anzulegen.
 *
 */
void MemoryHeap::increase(size_t num)
{
	HEAPBLOCK *bl=(HEAPBLOCK*)::malloc(sizeof(HEAPBLOCK));
	if (!bl) throw OutOfMemoryException();
	bl->elements=num;
	bl->next=NULL;
	bl->num_free=num;
	bl->previous=NULL;
	bl->buffer=::malloc(myElementSize*num);
	if (!bl->buffer) {
		::free(bl);
		throw OutOfMemoryException();
	}
	bl->bufferend=(ppluint8*)bl->buffer+myElementSize*num;
	bl->free=(HEAPELEMENT *)::malloc(sizeof(HEAPELEMENT)*num);
	if (!bl->free) {
		::free(bl->buffer);
		::free(bl);
		throw OutOfMemoryException();
	}
	bl->elbuffer=bl->free;
	HEAPELEMENT *t,*prev=NULL;
	ppluint8 *buffer=(ppluint8*)bl->buffer;
	for(size_t i=0;i<num;i++) {
		t=&bl->free[i];
		t->previous=prev;
		t->next=&bl->free[i+1];
		t->ptr=buffer;
		buffer+=myElementSize;
		prev=t;
	}
	bl->free[num-1].next=NULL;
	bl->next=(HEAPBLOCK*)blocks;
	if (bl->next) bl->next->previous=bl;
	blocks=bl;
	blocksAllocated+=num;
	mem_allocated+=sizeof(HEAPBLOCK)+myElementSize*num+sizeof(HEAPELEMENT)*num;
	mem_used+=sizeof(HEAPBLOCK);
}

/*!\brief Mit 0 initialisierten Speicher anfordern
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock aus dem Heap allokiert und mit
 * Null-Bytes initialisiert.
 *
 * \return
 * Pointer auf den allokierten Speicherbereich
 *
 * \exception OutOfMemoryException: Wird geworfen, wenn nicht genug Speicher verfügbar ist, um den
 * Heap anzulegen.
 *
 */
void *MemoryHeap::calloc()
{
	void *block=malloc();
	memset(block,0,myElementSize);
	return block;
}

/*!\brief Speicher anfordern
 *
 * \desc
 * Mit dieser Funktion wird ein neuer Speicherblock aus dem Heap allokiert. Dieser wird
 * nicht initialisiert und kann daher Zufallsdaten enthalten.
 *
 * \return
 * Pointer auf den allokierten Speicherbereich
 *
 * \exception OutOfMemoryException: Wird geworfen, wenn nicht genug Speicher verfügbar ist, um den
 * Heap anzulegen.
 *
 */
void *MemoryHeap::malloc()
{
	if (!myElementSize) throw NotInitializedException();
	while (1) {
		// Den nächsten freien Block suchen
		HEAPBLOCK *bl=(HEAPBLOCK*)blocks;
		while (bl) {
			if (bl->num_free) {
				HEAPELEMENT *el=bl->free;
				// Element aus der Free-Kette nehmen
				bl->free=bl->free->next;
				if(bl->free) bl->free->previous=NULL;
				bl->num_free--;
				mem_used+=myElementSize+sizeof(HEAPELEMENT);
				blocksUsed++;
				return el->ptr;
			}
			bl=bl->next;
		}
		// Speicher muss vergroessert werden
		increase(increaseSize);
		increaseSize+=(increaseSize*myGrowPercent/100);
	}
	return NULL;
}

/*!\brief Speicher freigeben
 *
 * \desc
 * Speicher, der zuvor mit MemoryHeap::malloc oder MemoryHeap::calloc allokiert wurde, wird wieder
 * freigegeben.
 *
 * @param mem Pointer auf den freizugebenden Speicherbereich
 *
 * \exception MemoryHeap::HeapCorruptedException: könnte auftreten, wenn der interne Speicher des
 * Heaps, in dem die Elemente verwaltet werden, überschrieben wurde.
 * \exception MemoryHeap::ElementNotInHeapException: Der mit \p mem referenzierte Speicherblock
 * wurde nicht über diesen Heap allokiert.
 */
void MemoryHeap::free(void *mem)
{
	if (!myElementSize) throw NotInitializedException();
	HEAPBLOCK *bl=(HEAPBLOCK*)blocks;
	while (bl) {
		if (mem>=bl->buffer && mem<=bl->bufferend) {
			// Nummer des Blocks errechnen
			HEAPELEMENT *el=bl->elbuffer;
			int element=(ppluint32)((ppluint8*)mem-(ppluint8*)bl->buffer)/(int)myElementSize;
			if(el[element].ptr!=mem) {
				// Hier stimmt was nicht!!!!
				throw HeapCorruptedException();
			}
			// Element in die Free-Kette hängen
			el[element].next=bl->free;
			el[element].previous=NULL;
			if (bl->free) bl->free->previous=&el[element];
			bl->free=&el[element];
			bl->num_free++;
			mem_used-=(myElementSize+sizeof(HEAPELEMENT));
			//if (bl->num_free==bl->elements) cleanup();
			blocksUsed--;
			freeCount++;
			if (freeCount>1000) cleanup();

			return;
		}
		bl=bl->next;
	}
	throw ElementNotInHeapException();
}

/*!\brief Aufräumen
 *
 * \desc
 * Diese Funktion prüft, ob der Heap ungenutze Speicherbereiche verwaltet und gibt diese
 * frei. Das schließt Speicherbereiche, die mit MemoryHeap::reserve reserviert wurden, mit ein.
 * Die Funktion wird automatisch nach 1000 Aufrufen von MemoryHeap::free aufgerufen.
 * Ein freier Speicherblock wird in Reserve gehalten.
 */
void MemoryHeap::cleanup()
{
	// Wenn mehr als ein Block komplett leer ist, geben wir ihn frei
	HEAPBLOCK *next, *bl=(HEAPBLOCK*)blocks;
	bool flag=false;
	while (bl) {
		next=bl->next;
		if (bl->num_free==bl->elements) {
			if (flag) {
				// Block wird gelöscht
				blocksAllocated-=bl->elements;
				if (bl->previous) bl->previous->next=bl->next;
				if (bl->next) bl->next->previous=bl->previous;
				mem_allocated-=sizeof(HEAPBLOCK)+myElementSize*bl->elements+sizeof(HEAPELEMENT)*bl->elements;
				mem_used-=sizeof(HEAPBLOCK);
				::free(bl->buffer);
				::free(bl->elbuffer);
				::free(bl);
				if (bl==(HEAPBLOCK*)blocks) blocks=next;
			}
			flag=true;
		}
		bl=next;
	}
	freeCount=0;
}

/*!\brief Anzahl Bytes, die verwendet werden
 *
 * \desc
 * Gibt zurück, wieviel Bytes zur Zeit durch den Heap in Verwendung sind, einschließlich
 * der Verwaltungsstrukturen.
 *
 * @return Anzahl Byte
 */
size_t MemoryHeap::memoryUsed() const
{
	return mem_used;
}

/*!\brief Anzahl Bytes, die allokiert sind
 *
 * \desc
 * Gibt zurück, wieviel Bytes zur Zeit durch den Heap allokiert sind, einschließlich
 * Verwaltungsstrukturen und Speicherelementen, die noch nicht in Verwendung sind.
 *
 * @return Anzahl Byte
 */
size_t MemoryHeap::memoryAllocated() const
{
	return mem_allocated;
}

/*!\brief Debug-Informationen
 *
 * \desc
 * Diese Funktion gibt einige Debug-Informationen zum Heap aus, insbesondere Anzahl
 * und Größe der durch den Heap verwalteten Speicherblöcke.
 */
void MemoryHeap::dump() const
{
	HEAPBLOCK *bl=(HEAPBLOCK*)blocks;
	PrintDebug ("Dump Heap (0x%tx, ",(std::ptrdiff_t)this);
	PrintDebug ("Elementsize: %zu):\n", myElementSize);
	PrintDebug ("Memory allocated: %zu Bytes, Memory used: %zu Bytes, Memory free: %zu Bytes\n",
			mem_allocated, mem_used, (mem_allocated-mem_used));
	PrintDebug ("Blocks allocated: %zu, Blocks used: %zu, freeCount: %zu\n",
			blocksAllocated, blocksUsed, freeCount);
	while (bl) {
		PrintDebug ("HEAPBLOCK: elements: %zu, free: %zu, Bytes allocated: %zu\n",bl->elements, bl->num_free, bl->elements*myElementSize);
		bl=bl->next;
	}
}

}	// EOF namespace ppl7
