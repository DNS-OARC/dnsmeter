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


/*!\class ByteArrayPtr
 * \ingroup PPLGroupDataTypes
 * \brief Referenz auf einen Speicherbereich
 *
 * \desc
 * Mit dieser Klasse kann ein einzelner Speicherblock repräsentiert werden.
 * Dieser besteht immer aus einer Speicheraddresse und der Größe des
 * Speicherbereichs in Bytes.
 * \par
 * Der Speicherbereich kann mit ByteArray::set gesetzt werden und mit ByteArray::adr und ByteArray::size
 * ausgelesen werden. Außerdem kann mit dem Operator [] ein bestimmtes Byte aus dem
 * Speicherbereich ausgelesen werden.
 * \par
 * Die Klasse enthält immer nur eine Referenz auf einen bestehenen Speicherbereich,
 * nicht den Speicher selbst. Soll auch der Speicher verwaltet werden, kann man
 * stattdessen die abgeleitete Klasse ByteArray verwenden.
 *
 * \see
 * - ByteArray
 */

/*!\var ByteArrayPtr::ptradr
 * \brief Pointer auf den referenzierten Speicherbereich
 *
 * \desc
 * Pointer auf den referenzierten Speicherbereich
 */

/*!\var ByteArrayPtr::ptrsize
 * \brief Größe des referenzierten Speicherbereichs
 *
 * \desc
 * Größe des referenzierten Speicherbereichs
 */

/*!\brief Konstruktor ohne Argumente
 *
 * \desc
 * Dieser Konstruktor erstellt eine Inszanz der Klasse ohne einen zugeweisenen Speicherbereich
 *
 */
ByteArrayPtr::ByteArrayPtr()
{
	ptradr=NULL;
	ptrsize=0;
}

/*!\brief Copy-Konstruktor
 *
 * \desc
 * Mit diesem Konstruktor wird eine Referenz auf den Speicherbereich einer anderen
 * ByteArrayPtr- oder ByteArray-Klasse übernommen.
 *
 * @param[in] other Referenz auf eine andere ByteArrayReferenz-Klasse
 */
ByteArrayPtr::ByteArrayPtr(const ByteArrayPtr &other)
{
	ptradr=other.ptradr;
	ptrsize=other.ptrsize;
}

ByteArrayPtr::ByteArrayPtr(const String &data)
{
	ptradr=(void*)data.getPtr();
	ptrsize=data.size();
}

ByteArrayPtr::ByteArrayPtr(const WideString &data)
{
	ptradr=(void*)data.getPtr();
	ptrsize=data.size();
}

/*!\brief Konstruktor mit Angabe einer Speicheradresse und Größe
 *
 * \desc
 * Mit diesem Konstruktor wird eine neue Instanz der Klasse erzeugt, die eine Referenz auf den mit
 * \p adr und \p size angegebenen Speicherbereich enthält.
 *
 * @param[in] adr Pointer auf den Beginn des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 */
ByteArrayPtr::ByteArrayPtr(void *adr, size_t size)
{
	ptradr=adr;
	ptrsize=size;
}

ByteArrayPtr::ByteArrayPtr(const void *adr, size_t size)
{
	ptradr=(void *)adr;
	ptrsize=size;
}

/*!\brief Prüfen, ob Speicher referenziert ist
 *
 * \desc
 * Mit dieser Funktion kann geprüft werden, ob die Klasse zur Zeit eine Referenz auf einen
 * Speicherbereich enthält.
 *
 * \return
 * Enthält die Klasse keine Referenz auf einen Speicherbereich, liefert die Funktion
 * \c true zurück, andernfalls \c false.
 */
bool ByteArrayPtr::isNull() const
{
	if (!ptradr) return true;
	return false;
}

/*!\brief Prüfen, ob der Referenzierte Speicher eine Größe von 0 hat
 *
 * \desc
 * Mit dieser Funktion kann geprüft werden, ob die Klasse zur Zeit eine Referenz auf einen
 * Speicherbereich enthält und dieser größer als 0 Byte ist.
 *
 * \return
 * Enthält die Klasse keine Referenz auf einen Speicherbereich, der mindestens 1 Byte
 * gross ist, liefert die Funktion \c true zurück, andernfalls \c false.
 */
bool ByteArrayPtr::isEmpty() const
{
	if (!ptradr) return true;
	if (!ptrsize) return true;
	return false;
}


/*!\brief Größe des Speicherblocks auslesen
 *
 * \desc
 * Mit dieser Funktion kann die Größe des Speicherblocks ausgelesen werden.
 *
 * @return Größe des Speicherblocks oder 0, wenn kein Speicher zugeordnet ist.
 */
size_t ByteArrayPtr::size() const
{
	return ptrsize;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit dieser Funktion wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
const void *ByteArrayPtr::adr() const
{
	return ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit dieser Funktion wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
const void *ByteArrayPtr::ptr() const
{
	return ptradr;
}


/*!\brief Referenz auf Speicherbereich setzen
 *
 * \desc
 * Mit dieser Funktion wird der Klasse der Speicherbereich mit der Adresse \p adr und der
 * Größe \p size zugeordnet. Der Speicherbereich selbst wird von der Klasse nicht verwaltet,
 * das heisst die Anwendung muss sich um dessen Freigabe kümmern.
 *
 * @param[in] adr Startadresse des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 *
 */
void ByteArrayPtr::use(void *adr, size_t size)
{
	ptradr=adr;
	ptrsize=size;
}

/*!\brief Referenz auf Speicherbereich von einer anderen ByteArrayPtr-Instanz kopieren
 *
 * \desc
 * Mit dieser Funktion wird eine Referenz auf einen Speicherbereich von einer anderen
 * ByteArrayPtr- oder ByteArray- Instanz kopiert.
 *
 * @param[in] other Referenz auf ein anderes ByteArray-Objekt.
 */
void ByteArrayPtr::use(const ByteArrayPtr &other)
{
	ptradr=other.ptradr;
	ptrsize=other.ptrsize;
}

/*!\brief Speicherreferenz von anderem ByteArrayPtr-Objekt übernehmen
 *
 * \desc
 * Mit diesem Operator wird eine Referenz auf einen Speicherbereich von einer anderen
 * ByteArrayPtr- oder ByteArray Instanz übernommen.
 *
 * @param[in] other Referenz auf ein anderes ByteArrayPtr-Objekt.
 * @return Referenz auf das Objekt
 */
ByteArrayPtr &ByteArrayPtr::operator=(const ByteArrayPtr &other)
{
	ptradr=other.ptradr;
	ptrsize=other.ptrsize;
	return *this;
}


/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
ByteArrayPtr::operator const void*() const
{
	return ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
ByteArrayPtr::operator const unsigned char*() const
{
	return (const unsigned char*)ptradr;
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
ByteArrayPtr::operator const char*() const
{
	return (const char*)ptradr;
}

/*!\brief Einzelnes Byte aus dem Speicherbereich auslesen
 *
 * \desc
 * Mit dem Operator [] kann ein bestimmtes Byte \p pos aus dem Speicherbereich
 * ausgelesen werden. Ist kein Speicher referenziert oder ist \p pos größer als
 * der Speicherblock, wird eine Exception ausgelöst.
 *
 * @param [in] pos Auszulesendes Byte, beginnend mit 0.
 * @return Wert der Speicherstelle
 * \exception OutOfBoundsEception Diese Exception wird geworfen, wenn die mit
 * \p pos angegebene Speicherstelle ausseralb des referenzierten Speichers liegt oder
 * kein Speicher referenziert ist.
 */
unsigned char ByteArrayPtr::operator[](size_t pos) const
{
	if (ptradr!=NULL && pos<ptrsize) return ((unsigned char*)ptradr)[pos];
	throw OutOfBoundsEception();
}


void ByteArrayPtr::set(size_t pos, unsigned char value)
{
	if (pos<ptrsize) ((unsigned char*)ptradr)[pos]=value;
	else throw OutOfBoundsEception();
}

unsigned char ByteArrayPtr::get(size_t pos) const
{
	if (ptradr!=NULL && pos<ptrsize) return ((unsigned char*)ptradr)[pos];
	throw OutOfBoundsEception();
}


/*!\brief Speicherbereich als Hexwerte in einen String exportieren
 *
 * \desc
 * Der referenzierte Speicherbereich wird als String mit Hexadezimalwerten
 * exportiert, wobei jedes Byte als zwei Zeichen langer Hexadezimalwert
 * dargestellt wird.
 *
 * @return String mit Hexadezimal-Werten
 */
String ByteArrayPtr::toHex() const
{
	unsigned char *buff=(unsigned char*)ptradr;
	String str;
	for(size_t i=0;i<ptrsize;i++) str.appendf("%02x",buff[i]);
	return str;
}

/*!\brief Speicherbereich als Base64 in einen String exportieren
 *
 * \desc
 * Der referenzierte Speicherbereich wird im Base64-Format als String
 * exportiert.
 *
 * @return Inhalt des Speicherbereichs als Base64-String.
 */
String ByteArrayPtr::toBase64() const
{
	return ToBase64(*this);
}

/*!\brief Adresse des Speicherblocks auslesen
 *
 * \desc
 * Mit diesem Operator wird die Adresse des Speicherblocks ausgelesen
 *
 * @return Adresse des Speicherblocks
 */
const char* ByteArrayPtr::toCharPtr() const
{
	return (const char*)ptradr;
}

/*!\brief CRC32-Prüfsumme berechnen
 *
 * \desc
 * Diese Funktion liefert die CRC32-Prüfsumme des Speicherbereichs zurück.
 *
 * @return CRC32-Prüfsumme
 */
ppluint32 ByteArrayPtr::crc32() const
{
	if (ptrsize==0) throw EmptyDataException();
	return Crc32(ptradr,ptrsize);
}

void ByteArrayPtr::hexDump() const
{
	if (ptrsize==0) throw EmptyDataException();
	PrintDebug ("HEXDUMP of ByteArray: %zi Bytes starting at Address %p:\n",ptrsize,ptradr);
	HexDump(ptradr,ptrsize,true);
}

void ByteArrayPtr::hexDump(size_t bytes) const
{
	if (ptrsize==0) throw EmptyDataException();
	if (bytes>ptrsize) bytes=ptrsize;
	PrintDebug ("HEXDUMP of ByteArray: %zi Bytes starting at Address %p:\n",bytes,ptradr);
	HexDump(ptradr,bytes,true);

}

void ByteArrayPtr::hexDump(size_t offset, size_t bytes) const
{
	if (ptrsize==0) throw EmptyDataException();
	if (bytes>ptrsize-offset) bytes=ptrsize-offset;
	char *start=(char*)ptradr+offset;
	PrintDebug ("HEXDUMP of ByteArray: %zi Bytes starting at Address %p:\n",bytes,start);
	HexDump(start,bytes,true);
}



/*!\brief Speicher mit bestimmtem Wert füllen
 *
 * \desc
 * Diese Funktion füllt den gesamten verwalteten Speicher mit dem Bytewert \p value
 *
 * \param[in] value Bytewert
 */
void ByteArrayPtr::memset(int value)
{
	::memset(ptradr,value,ptrsize);
}


int ByteArrayPtr::memcmp(const ByteArrayPtr &other) const
{
	//size_t max=ptrsize;
	size_t min=ptrsize;
	//if (other.ptrsize>max) max=other.ptrsize;
	if (other.ptrsize<min) min=other.ptrsize;
	for (size_t i=0;i<min;i++) {
		if ( ((char*)ptradr)[i] < ((char*)other.ptradr)[i] ) return -1;
		if ( ((char*)ptradr)[i] > ((char*)other.ptradr)[i] ) return 1;
	}
	if (ptrsize<other.ptrsize) return -1;
	if (ptrsize>other.ptrsize) return 1;
	return 0;
}

bool ByteArrayPtr::operator<(const ByteArrayPtr &other) const
{
	int c=memcmp(other);
	if (c<0) return true;
	return false;
}

bool ByteArrayPtr::operator<=(const ByteArrayPtr &other) const
{
	int c=memcmp(other);
	if (c<=0) return true;
	return false;
}

bool ByteArrayPtr::operator==(const ByteArrayPtr &other) const
{
	int c=memcmp(other);
	if (c==0) return true;
	return false;
}

bool ByteArrayPtr::operator!=(const ByteArrayPtr &other) const
{
	int c=memcmp(other);
	if (c!=0) return true;
	return false;
}

bool ByteArrayPtr::operator>=(const ByteArrayPtr &other) const
{
	int c=memcmp(other);
	if (c>=0) return true;
	return false;
}

bool ByteArrayPtr::operator>(const ByteArrayPtr &other) const
{
	int c=memcmp(other);
	if (c>0) return true;
	return false;
}

std::ostream& operator<<(std::ostream& s, const ByteArrayPtr &ba)
{
	ppl7::String hex=ba.toHex();
	return s.write((const char*)hex,hex.size());
}


}	// EOF namespace ppl7
