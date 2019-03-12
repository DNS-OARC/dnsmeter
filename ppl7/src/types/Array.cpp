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


/*!\class Array
 * \ingroup PPLGroupDataTypes
 * \brief Ein Array mit Strings
 *
 * \desc
 * Diese Klasse repräsentiert ein Array aus Strings, die über einen Index angesprochen werden
 * können. Die Zählung der Elemente beginnt dabei bei 0, das heisst das erste Element hat den
 * Index 0 (vergleichbar mit Arrays in C/C++).
 *
 */

/*!\var Array::numElements
 * \brief Anzahl Elemente im Array
 *
 * \desc
 * Diese Variable enthält die tatsächliche Anzahl Elemente im Array
 */

/*!\var Array::numCapacity
 * \brief Maximale Anzahl Elemente im Array
 *
 * \desc
 * Diese Variable enthält die maximal mögliche Anzahl Elemente im Array, ohne dass
 * Speicher reallokiert werden muss. Wird dieser Wert überschritten, wird das Array
 * durch Reallokation von Speicher vergrößert.
 * \see
 * - Array::capacity: Mit dieser Funktion kann die Variable ausgelesen werden
 * - Array::reserve: Speicher wird vorab für eine beliebige Anzahl Elemente allokiert
 */

/*!\var Array::rows
 * \brief Array mit den Datenelementen
 *
 * \desc
 * Diese Variable enthält einen Pointer auf die interne Datenstruktur, die die Werte
 * der einzelnen Elemente enthält. Die Größe des Speicherbereichs ergibt sich aus der
 * maximalen Anzahl Elemente im Array (Array::numCapacity) multipliziert mit der größe der
 * Datenstruktur für jedes Element.
 */


typedef struct {
	String *value;
} ROW;

static String EmptyString;


/*!\brief Konstruktor
 *
 * \desc
 * Der Standard-Konstruktor erstellt ein leeres Array
 */
Array::Array()
{
	numElements=0;
	numCapacity=0;
	rows=NULL;
}

/*!\brief Copy-Konstruktor
 *
 * \desc
 * Mit dem Copy-Konstruktor wird der Inhalt des Arrays \p other 1:1 kopiert.
 *
 * @param other Anderes Array
 */
Array::Array(const Array &other)
{
	numElements=0;
	numCapacity=0;
	rows=NULL;
	add(other);
}

/*!\brief Konstruktor aus String
 *
 * \desc
 * Mit diesem Konstruktor wird der String \p str anhand des Trennzeichens
 * \p delimiter in einzelne Elemente zerlegt
 *
 * @param str String
 * @param delimiter Trennzeichen oder Trennstring
 * @param limit Maximale Anzahl Elemente, normalerweise unbegrenzt
 * @param skipemptylines Leere Elemente überspringen. Folgen zwei Trennzeichen hintereinander, würde
 * normalerweise ein leeres Element in das Array eingefügt. Durch setzen dieses Parameters auf \c true
 * werden keine leeren Elemente eingefügt.
 *
 * \see
 * Dieser Konstruktor verwendet die Funktion Array::explode zum Zerlegen des Strings.
 */
Array::Array(const String &str, const String &delimiter, size_t limit, bool skipemptylines)
{
	numElements=0;
	numCapacity=0;
	rows=NULL;
	explode(str,delimiter,limit,skipemptylines);
}

/*!\brief Destruktor
 *
 * \desc
 * Das Array und aller dadurch belegter Speicher wird wieder freigegeben.
 */
Array::~Array()
{
	clear();
}


/*!\brief Array löschen
 *
 * \desc
 * Der durch das Array belegte Speicher wird freigegeben. Das Array ist danach leer und kann erneut befüllt werden.
 *
 */
void Array::clear()
{
	ROW *r=(ROW*)rows;
	if (r) {
		for (size_t i=0;i<numCapacity;i++) {
			if (r[i].value) {
				delete(r[i].value);
				r[i].value=NULL;
			}
		}
		free(rows);
	}
	rows=NULL;
	numCapacity=0;
	numElements=0;
}

/*!\brief Array kopieren
 *
 * \desc
 * Der bisherige Inhalt des Arrays wird gelöscht und mit dem das Arrays \p other gefüllt. Falls der bestehende
 * Inhalt nicht gelöscht werden soll, muss die Funktion Array::add(const Array &other) verwendet werden.
 *
 * @param other Zu kopierendes Array
 */
void Array::copy(const Array &other)
{
	clear();
	reserve(other.numElements);
	ROW *r=(ROW*)other.rows;
	for (size_t i=0;i<other.numElements;i++) {
		if (r[i].value!=NULL) set(i,*r[i].value);
	}
}

/*!\brief Array hinzufügen
 *
 * \desc
 * Der Inhalt des Arrays \p other wird an das Array angehangen.
 *
 * @param other Zu kopierendes Array
 */
void Array::add(const Array &other)
{
	ROW *r=(ROW*)other.rows;
	size_t first=numElements;
	reserve(numElements+other.numElements);
	for (size_t i=0;i<other.numElements;i++) {
		if (r[i].value!=NULL) set(first+i,*r[i].value);
	}
}

/*!\brief String anhängen
 *
 * \desc
 * Der Inhalt des Strings \p value wird dem Array hinzugefügt.
 *
 * @param value String
 */
void Array::add(const String &value)
{
	set(numElements,value);
}

/*!\brief Teilstring anhängen
 *
 * \desc
 * Der Inhalt des Strings \p value und der Länge \p size wird dem Array hinzugefügt.
 *
 * @param value String
 * @param size Größe des Strings
 */
void Array::add(const String &value, size_t size)
{
	String str;
	str.set(value,size);
	set(numElements,str);
}

/*!\brief Teilstring anhängen
 *
 * \desc
 * Der Inhalt des Strings \p value und der Länge \p size wird dem Array hinzugefügt.
 *
 * @param value String
 * @param size Größe des Strings
 */
void Array::add(const char *value, size_t size)
{
	String str;
	str.set(value,size);
	set(numElements,str);
}

/*!\brief Formatierten String anhängen
 *
 * \desc
 * Mittels Formatstring \p fmt und der optionalen Parameter wird ein neuer String gebildet, der am
 * Ende des Array angehangen wird.
 *
 * @param fmt Formatstring
 * @param ... Optionale Parameter
 */
void Array::addf(const char *fmt, ...)
{
	String value;
	va_list args;
	va_start(args, fmt);
	value.vasprintf(fmt,args);
	va_end(args);
	set(numElements,value);
}

/*!\brief Wert eines Elements setzen
 *
 * \desc
 * Der Wert des Strings \p value wird an der Position \p index des Arrays gesetzt. Der vorherige Wert
 * des Arrays an dieser Stelle wird dadurch überschrieben.
 *
 * @param index Position innerhalb des Arrays, beginnend mit 0
 * @param value String
 */
void Array::set(size_t index, const String &value)
{
	ROW *r;
	if (index>=numCapacity) {
		// Array muss vergroessert werden
		reserve(index+10);
	}
	r=(ROW*)rows;
	if ((index+1)>numElements) numElements=index+1;
	if (value.notEmpty()) {
		if (r[index].value==NULL) {
			r[index].value=new String;
			if (!r[index].value) throw OutOfMemoryException();
		}
		r[index].value->set(value);
	} else {
		if (r[index].value!=NULL) {
			delete r[index].value;
			r[index].value=NULL;
		}
	}
}

/*!\brief Formatierten String setzen
 *
 * \desc
 * Mittels Formatstring \p fmt und der optionalen Parameter wird ein neuer String gebildet, der an
 * die Position \p index des Arrays gesetzt wird. Der vorherige Wert
 * des Arrays an dieser Stelle wird überschrieben.
 *
 * @param index Position innerhalb des Arrays, beginnend mit 0
 * @param fmt Formatstring
 * @param ... Optionale Parameter
 */
void Array::setf(size_t index, const char *fmt, ...)
{
	String value;
	va_list args;
	va_start(args, fmt);
	value.vasprintf(fmt,args);
	va_end(args);
	set(index,value);
}


/*!\brief Element im Array einfügen
 *
 * \desc
 * Alle vorhandenen Elemente des Arrays ab Position \p index werden um eins nach hinten
 * verschoben. Anschließend wird das neue Element \p value an der Position \p index
 * eingefügt.
 *
 * @param index Position, an der das Element eingefügt werden soll
 * @param value Wert des Elements
 */
void Array::insert(size_t index, const String &value)
{
	ROW *r=(ROW*)rows;
	// Zunächst sorgen wir dafür, dass im Array genug Platz ist
	reserve(numElements+2);
	// Nun verschieben wir alle Elemente ab Position index um eins nach hinten
	if (numElements>index) {
		for (size_t i=numElements;i>index;i--) {
			r[i].value=r[i-1].value;
		}
		numElements++;
		r[index].value=NULL;
	}
	// Den neuen Wert einfügen
	set(index,value);
}

/*!\brief Array einfügen
 *
 * \desc
 * Das komplette Array \p other wird ab der Position \p index eingefügt. Alle bisher
 * vorhandenen Elemente werden ab dieser Position um die Größe des einzufügenden
 * Arrays nach hinten verschoben.
 *
 * @param index Position, an der das Element eingefügt werden soll
 * @param other Einzufügendes Array
 */
void Array::insert(size_t index, const Array &other)
{
	if (other.numElements==0) return;	// Anderes Array ist leer
	// Zunächst sorgen wir dafür, dass im Array genug Platz ist
	reserve(numElements+other.numElements+2);
	ROW *r=(ROW*)rows;
	// Nun verschieben wir alle Elemente ab Position index um die größe des anderen
	// Arrays nach hinten
	if (numElements>index) {
		for (size_t i=numElements;i>index;--i) {
			size_t ii=i-1;
			r[ii+other.numElements].value=r[ii].value;
			r[ii].value=NULL;
		}
	}
	// Die neuen Werte einfügen
	ROW *r2=(ROW*)other.rows;
	for (size_t i=0;i<other.numElements;i++) {
		if (r2[i].value!=NULL) {
			r[index+i].value=new String;
			if (!r[index+i].value) throw OutOfMemoryException();
			r[index+i].value->set(r2[i].value);
		}
	}
	if (index>numElements) numElements+=(index-numElements);
	numElements+=other.numElements;
}

/*!\brief Element mittels Formatstring einfügen
 *
 * \desc
 * Alle vorhandenen Elemente des Arrays ab Position \p index werden um eins nach hinten
 * verschoben. Anschließend wird ein neuer Wert mittels des Formatstrings \p fmt
 * und den zusätzlichen Parametern gebildet und an der Position \p index
 * eingefügt.
 *
 * @param index Position, an der das Element eingefügt werden soll
 * @param fmt Formatstring
 * \param ... Zusätzliche optionale Parameter
 */
void Array::insertf(size_t index, const char *fmt, ...)
{
	String value;
	va_list args;
	va_start(args, fmt);
	value.vasprintf(fmt,args);
	va_end(args);
	insert(index,value);
}


/*!\brief Platz reservieren
 *
 * \desc
 * Durch Aufruf dieser Funktion wird vorab Speicher allokiert, um die durch \p size angegebene
 * Anzahl Elemente aufnehmen zu können. Die Funktion sollte immer dann aufgerufen werden, wenn
 * schon vor dem Befüllen des Array bekannt ist, wieviele Elemente es aufnehmen soll. Insbesondere
 * bei großen Arrays ist dies sinnvoll, da dadurch das Reallokieren und Kopieren von Speicher
 * während der Befüllung reduziert wird.
 *
 * @param size Anzahl Elemente, die das Array aufnehmen soll
 *
 * \note
 * Ist die Kapazität des Arrays bei Aufruf der Funktion bereits höher als der angegebene Wert
 * \p size, bleibt das Array unverändert. Die Kapazität kann nachträglich nicht verkleinert werden.
 *
 * \see
 * Mit der Funktion Array::capacity kann abgefragt werden, für wieviele Elemente derzeit Speicher
 * reserviert ist.
 */
void Array::reserve (size_t size)
{
	if (size>numCapacity) {
		//PrintDebugTime ("Array::reserve von %zu auf %zu Elemente\n",numCapacity,size);
		ROW *r;
		// Array muss vergroessert werden
		void *newrows=realloc(rows,(size)*sizeof(ROW));
		if (!newrows) {
			throw OutOfMemoryException();
		}
		r=(ROW*)newrows;
		for (size_t i=numCapacity;i<size;i++) {
			r[i].value=NULL;
		}
		rows=newrows;
		numCapacity=size;
	}
}

/*!\brief Anzahl Elemente, für die Speicher reserviert ist
 *
 * \desc
 * Diese Funktion gibt aus, wieviele Elemente das Array aufnehmen kann, ohne dass Speicher reallokiert
 * und kopiert werden muss.
 *
 * @return Anzahl Elemente
 * \see
 * Mit der Funktion Array::reserve kann die Kapazität des Arrays vorab bestimmt werden.
 */
size_t Array::capacity() const
{
	return numCapacity;
}

/*!\brief Anzahl Elemente im Array
 *
 * \desc
 * Diese Funktion gibt die Anzahl Elemente im Array zurück.
 *
 * @return Anzahl Elemente
 *
 * \note
 * Wird bei einem leeren Array ein String an der Position 5 eingefügt, werden die Positionen 0 bis 4 automatisch
 * als leere Elemente interpretiert. Array::count wird daher 6 zurückgeben.
 * \see
 * Die Funktionen Array::size und Array::count sind identisch.
 */
size_t Array::count() const
{
	return numElements;
}

/*!\brief Anzahl Elemente im Array
 *
 * \desc
 * Diese Funktion gibt die Anzahl Elemente im Array zurück.
 *
 * @return Anzahl Elemente
 *
 * \note
 * Wird bei einem leeren Array ein String an der Position 5 eingefügt, werden die Positionen 0 bis 4 automatisch
 * als leere Elemente interpretiert. Array::size wird daher 6 zurückgeben.
 * \see
 * Die Funktionen Array::size und Array::count sind identisch.
 */
size_t Array::size() const
{
	return numElements;
}

/*!\brief Array leer?
 *
 * \desc
 * Prüft, ob das Array leer ist.
 * @return Gibt \c true zurück, wenn das Array leer ist, also keine Elemente enthält, andernfalls \c true.
 */
bool Array::empty() const
{
	if (numElements==0) return true;
	return false;
}


/*!\brief Inhalt des Arrays ausgeben
 *
 * \desc
 * Der Inhalt des Arrays wird auf der Konsole ausgegeben
 *
 * @param prefix Optionaler String, der jedem Element vorangestellt wird.
 *
 * \example
\code
ppl7::Array a;
a.add(L"Value 1");
a.add(L"Value 2");
a.add(L"Value 3");
a.add(L"Value 4");
a.add(L"Value 5");
a.list("Mein Array");
\endcode
Ausgabe:
\verbatim
MeinArray,      0: Value 1
MeinArray,      1: Value 2
MeinArray,      2: Value 3
MeinArray,      3: Value 4
MeinArray,      4: Value 5
\endverbatim
 */
void Array::list(const String &prefix) const
{
	ROW *r=(ROW*)rows;
	if (prefix.isEmpty()) {
		if ((!rows) || numElements==0) {
			PrintDebug("Array ist leer\n");
		}
		for (size_t i=0;i<numElements;i++) {
			if (r[i].value!=NULL) PrintDebug ("%6zu: %s\n",i,(const char*)r[i].value->getPtr());

		}
	} else {
		if ((!rows) || numElements==0) {
			PrintDebug("Array \"%s\" ist leer\n", (const char*)prefix);
		}
		for (size_t i=0;i<numElements;i++) {
			if (r[i].value!=NULL) PrintDebug ("%s, %6zu: %s\n",(const char*)prefix, i,(const char*)r[i].value->getPtr());
		}

	}
}

/*!\brief Element als Konstante auslesen
 *
 * \desc
 * Gibt das Element an Position \p index des Arrays als Referenz zurück, dessen Inhalt nicht
 * verändert werden kann. Ist \p index größer als die Anzahl Elemente des Arrays, wird eine Exception geworfen.
 *
 * @param index Gewünschtes Element
 * @return Referenz auf den Inhalt des Elements
 * \exception OutOfBoundsEception: Wird geworfen, wenn \p index größer als die Anzahl Elemente des Arrays ist
 */
const String &Array::get(ssize_t index) const
{
	if (index<0) {
		index=numElements+index;
		if (index<0) throw OutOfBoundsEception();
	}
	ROW *r=(ROW*)rows;
	if ((size_t)index>=numElements) throw OutOfBoundsEception();
	if (r[index].value!=NULL) return *r[index].value;
	return EmptyString;
}

/*!\brief Element auslesen
 *
 * \desc
 * Gibt das Element an Position \p index des Arrays als Referenz zurück. Ist \p index größer als die Anzahl
 * Elemente des Arrays, wird eine Exception geworfen.
 *
 * @param index Gewünschtes Element
 * @return Referenz auf den Inhalt des Elements
 * \exception OutOfBoundsEception: Wird geworfen, wenn \p index größer als die Anzahl Elemente des Arrays ist
 */
String &Array::get(ssize_t index)
{
	if (index<0) {
		index=numElements+index;
		if (index<0) throw OutOfBoundsEception();
	}
	ROW *r=(ROW*)rows;
	if ((size_t)index>=numElements) throw OutOfBoundsEception();
	if (r[index].value!=NULL) return *r[index].value;
	return EmptyString;
}

/*!\brief Zufälliges Element als Konstante auslesen
 *
 * \desc
 * Gibt eine Referenz auf ein zufälliges Element des Arrays zurück, dessen Inhalt nicht verändert
 * werden kann.
 *
 * @return Referenz auf ein zufälliges Elements des Arrays.
 * Ist das Array leer, wird immer ein leerer String zurückgegeben.
 */
const String &Array::getRandom() const
{
	if (!numElements) return EmptyString;
	ROW *r=(ROW*)rows;
	size_t index=ppl7::rand(0,numElements-1);
	if (index<numElements && r[index].value!=NULL) return *r[index].value;
	return EmptyString;
}

/*!\brief Zufälliges Element auslesen
 *
 * \desc
 * Gibt eine Referenz auf ein zufälliges Element des Arrays zurück.
 *
 * @return Referenz auf ein zufälliges Elements des Arrays.
 * Ist das Array leer, wird immer ein leerer String zurückgegeben.
 */
String &Array::getRandom()
{
	if (!numElements) return EmptyString;
	ROW *r=(ROW*)rows;
	size_t index=ppl7::rand(0,numElements-1);
	if (index<numElements && r[index].value!=NULL) return *r[index].value;
	return EmptyString;
}

/*!\brief char Pointer auf ein Element auslesen
 *
 * \desc
 * Gibt einen const char Pointer auf das Element \p index zurück.
 * Liegt \p index ausserhalb des Arrays wird
 * eine Exception geworfen.
 *
 * @param index Gewünschtes Element
 * @return Pointer auf den C-String des gewünschten Elements.
 * \exception OutOfBoundsEception: Wird geworfen, wenn \p index größer als die Anzahl Elemente des Arrays ist
 */
const char *Array::getPtr(ssize_t index) const
{
	return get(index).getPtr();
}

/*!\brief Zufälliges Element als char Pointer auslesen
 *
 * \desc
 * Gibt einen const char Pointer auf ein zufälliges Element des Arrays zurück.
 *
 * @return Pointer auf den C-String eines zufälligen Elements des Arrays.
 * Ist das Array leer, wird immer ein leerer String zurückgegeben.
 */
const char *Array::getRandomPtr() const
{
	if (!numElements) return String();
	ROW *r=(ROW*)rows;
	size_t index=ppl7::rand(0,numElements-1);
	if (index<numElements && r[index].value!=NULL) return r[index].value->getPtr();
	return "";
}


/*!\brief Inhalt des Arrays ab einer bestimmten Position als String zurückgeben
 *
 * \desc
 * Inhalt des Arrays ab einer bestimmten Position als String zurückgeben
 *
 * @param index Position im Array
 * @param delimiter Trennzeichen, mit dem die Elemente des Arrays im String zusammengefügt
 *        werden sollen
 * @return String
 */
String Array::getRest(size_t index, const String &delimiter)
{
	String rest;
	ROW *r=(ROW*)rows;
	for (size_t i=index;i<numElements;i++) {
		if (i>index) rest+=delimiter;
		if (r[i].value!=NULL) rest+= *r[i].value;
	}
	return rest;
}


/*!\brief Iterator auf den Anfang setzen
 *
 * \desc
 * Mit diesem Befehl wird der Iterator zum Durchwandern des Arrays auf das erste Element gesetzt
 *
 * @param it Iterator
 *
 * \example
 * \code
ppl7::Array a1(L"red green blue yellow black white",L" ");
ppl7::Array::Iterator it;
ppl7::String value;
a1.reset(it);
try {
	while (1) {
		value=a1.getNext(it);
		value.printnl();
	}
} catch (ppl7::OutOfBoundsEception) {
	printf ("Keine weiteren Elemente\n");
}
 * \endcode
 */
void Array::reset(Iterator &it) const
{
	it.pos=0;
}

/*!\brief Referenz auf das erste Element mittels Iterator auslesen
 *
 * \desc
 * Der Iterator \p it wird auf das erste Element gesetzt. Dieses gibt die Funktion zurück und
 * erhöht den Iterator auf das nächste Element.
 *
 * @param it Iterator
 * @return Wert des ersten Elements
 * \exception OutOfBoundsEception: Wird geworfen, wenn das Array leer ist
 *
 * \example
 * \code
ppl7::Array a1(L"red green blue yellow black white",L" ");
ppl7::Array::Iterator it;
ppl7::String value;
a1.reset(it);
try {
	while (1) {
		value=a1.getNext(it);
		value.printnl();
	}
} catch (ppl7::OutOfBoundsEception) {
	printf ("Keine weiteren Elemente\n");
}
 * \endcode
 */
const String &Array::getFirst(Iterator &it) const
{
	it.pos=0;
	return getNext(it);
}

/*!\brief Referenz auf das nächste Element mittels Iterator auslesen
 *
 * \desc
 * Der Iterator \p it wird auf das erste Element gesetzt. Dieses gibt die Funktion zurück und
 * erhöht den Iterator auf das nächste Element.
 *
 * @param it Iterator
 * @return Wert des ersten Elements
 * \exception OutOfBoundsEception: Wird geworfen, wenn das Array leer ist
 * \example
 * \code
ppl7::Array a1(L"red green blue yellow black white",L" ");
ppl7::Array::Iterator it;
ppl7::String value;
a1.reset(it);
try {
	while (1) {
		value=a1.getNext(it);
		value.printnl();
	}
} catch (ppl7::OutOfBoundsEception) {
	printf ("Keine weiteren Elemente\n");
}
 * \endcode
 */
const String &Array::getNext(Iterator &it) const
{
	ROW *r=(ROW*)rows;
	if (it.pos<numElements) {
		String *s=r[it.pos].value;
		it.pos++;
		if (s!=NULL) return *s;
		return EmptyString;
	}
	throw OutOfBoundsEception();
}

/*!\brief Das erste Element aus dem Array holen
 *
 * \desc
 * Das erste Element des Arrays (also das mit dem Index 0) wird aus
 * dem Array entfernt und als String zurückgegeben. Der Rest des Arrays wird um
 * eine Position nach vorne gerückt. Ist das Array leer, wird eine
 * Exception geworfen.
 *
 * @return String mit dem Wert, der aus dem Array entfernt wurde
 * \exception OutOfBoundsEception: Wird geworfen, wenn der gewünschte Index größer als
 * die Anzahl Elemente im Array ist.
 * \note
 * Bei großen Arrays ist diese Operation recht teuer, da alle nachfolgenden Elemente
 * um eine Position nach vorne gerückt werden müssen.
 */
String Array::erase(size_t index)
{
	if (index>=numElements) throw OutOfBoundsEception();
	String ret;
	ROW *r=(ROW*)rows;
	if (r[index].value!=NULL) {
		ret=r[index].value;
		delete r[index].value;
	}
	for (size_t i=index;i<numElements-1;i++) {
		r[i].value=r[i+1].value;
	}
	numElements--;
	r[numElements].value=NULL;
	return ret;
}

/*!\brief Das erste Element aus dem Array holen
 *
 * \desc
 * Das erste Element des Arrays (also das mit dem Index 0) wird aus
 * dem Array entfernt und als String zurückgegeben. Der Rest des Arrays wird um
 * eine Position nach vorne gerückt. Ist das Array leer, wird eine
 * Exception geworfen.
 *
 * @return String mit dem Wert, der aus dem Array entfernt wurde
 * \exception EmptyDataException: Wird geworfen, wenn das Array leer ist
 * \note
 * Bei großen Arrays ist diese Operation recht teuer, da alle nachfolgenden Elemente
 * um eine Position nach vorne gerückt werden müssen.
 */
String Array::shift()
{
	if (!numElements) throw EmptyDataException();
	String ret;
	ROW *r=(ROW*)rows;
	if (r[0].value!=NULL) {
		ret=r[0].value;
		delete r[0].value;
	}
	for (size_t i=0;i<numElements-1;i++) {
		r[i].value=r[i+1].value;
	}
	numElements--;
	r[numElements].value=NULL;
	return ret;
}

/*!\brief Das letzte Element aus dem Array holen
 *
 * \desc
 * Das letzte Element des Arrays (also das mit dem höchsten Index) wird aus
 * dem Array entfernt und als String zurückgegeben. Ist das Array leer, wird eine
 * Exception geworfen.
 *
 * @return String mit dem Wert, der aus dem Array entfernt wurde
 * \exception EmptyDataException: Wird geworfen, wenn das Array leer ist
 */
String Array::pop()
{
	if (!numElements) throw EmptyDataException();
	ROW *r=(ROW*)rows;
	String ret;
	if (r[numElements-1].value!=NULL) {
		ret=r[numElements].value;
		delete r[numElements-1].value;
		r[numElements-1].value=NULL;
	}
	numElements--;
	return ret;
}


/*!\brief Array aus String erzeugen
 *
 * \desc
 * Der String \p text wird anhand des Trennzeichens \p delimiter in einzelne Elemente zerlegt, die
 * wiederum an das Array angefügt werden
 *
 * @param text Zu parsender String
 * @param delimiter Trennzeichen oder Trennstring
 * @param limit Maximale Anzahl Elemente, normalerweise unbegrenzt
 * @param skipemptylines Leere Elemente überspringen. Folgen zwei Trennzeichen hintereinander, würde
 * normalerweise ein leeres Element in das Array eingefügt. Durch setzen dieses Parameters auf \c true
 * werden keine leeren Elemente eingefügt.
 *
 * @return Referenz auf das Array
 *
 * \note
 * Vorhandene Elemente im Array gehen durch Aufruf dieser Funktion nicht verloren, die neuen Werte werden
 * am Ende angehangen. Ist \p text leer, werden dem Array keine Elemente hinzugefügt.
 *
 * \see
 * Array::implode ist die Umkehrfunktion zu dieser
 */
Array &Array::explode(const String &text, const String &delimiter, size_t limit, bool skipemptylines)
{
	if (text.isEmpty()) return *this;
	if (delimiter.isEmpty()) return *this;
	ssize_t p;
	size_t t=delimiter.len();
	size_t count=0;
	const char *del=(const char *)delimiter;
	char *etext=(char*)text.getPtr();
	char *_t;
	String str;
	while (1) {
		_t=strstr(etext,del);
		if (_t) {
			p=_t-etext;
			if (p==0 && skipemptylines==true) {
				etext+=t;
				continue;
			}
			if (limit>0 && count>=limit) {
				return *this;
			}
			str.set(etext,p);
			//add(etext,p);
			set(numElements,str);
			etext=etext+p+t;
			count++;
		} else {
			if (skipemptylines==false || strlen(etext)>0) {
				count++;
				if (limit==0 || count<=limit) {
					add(etext);
				}
			}
			return *this;
		}
	}
	return *this;
}

/*!\brief Array zu einem String zusammenfügen
 *
 * \desc
 * Der Inhalt des Arrays wird zu einem String zusammengefügt, wobei das im Parameter \p delimiter
 * angegebene Zeichen oder String als Trennzeichen verwendet wird.
 *
 * @param delimiter Trennzeichen oder String
 * @return Zusammengesetzter String mit dem Inhalt des Arrays
 */
String Array::implode(const String &delimiter) const
{
	String ret;
	for (size_t i=0;i<numElements;i++) {
		if (i) ret+=delimiter;
		ret+=get(i);
	}
	return ret;
}


/*!\brief Array aus den Aufrufparametern des Programms erzeugen
 *
 * \desc
 * Ein Array wird aus den Aufrufparametern des Programms erstellt.
 *
 * @param argc Anzahl Parameter
 * @param argv Pointer auf ein Array mit C-Strings
 * @return Gibt eine Referenz auf das Array zurück.
 */
Array &Array::fromArgs(int argc, const char **argv)
{
	clear();
	for (int i=0;i<argc;i++) {
		add(argv[i]);
	}
	return *this;
}

/*!\brief Array aus dem Aufrufstring des Programms erzeugen
 *
 * \desc
 * Ein Array wird aus dem Aufrufstring des Programms erstellt.
 *
 * @param args Aufrufstring
 * @return Gibt eine Referenz auf das Array zurück.
 */
Array &Array::fromArgs(const String &args)
{
	clear();
	String buffer(args);
	String arg;
	// Kommandozeile in argc und argv[] umwandeln

	size_t l=buffer.len();
	add(args);
	bool inDoubleQuote=false;
	bool inSingleQuote=false;
	size_t start=0;
	for (size_t i=0;i<l;i++) {
		if(buffer[i]==34 && inDoubleQuote==false && inSingleQuote==false) {
			if (i==0) {
				inDoubleQuote=true;
				start=i+1;
			}
			else if (buffer[i-1]!='\\') {
				inDoubleQuote=true;
				start=i+1;
			}
		} else if(buffer[i]=='\'' && inDoubleQuote==false && inSingleQuote==false) {
				if (i==0) {
					inSingleQuote=true;
					start=i+1;
				}
				else if (buffer[i-1]!='\\') {
					inSingleQuote=true;
					start=i+1;
				}

		} else if(buffer[i]==34 && inDoubleQuote==true && buffer[i-1]!='\\') {
			inDoubleQuote=false;
			arg=buffer.mid(start,i-start);
			if (arg.notEmpty()) add(arg);

			//if(argv[argc][0]!=0) argc++;
			start=i+1;
		} else if(buffer[i]=='\'' && inSingleQuote==true && buffer[i-1]!='\\') {
			inSingleQuote=false;
			arg=buffer.mid(start,i-start);
			if (arg.notEmpty()) add(arg);

			//if(argv[argc][0]!=0) argc++;
			start=i+1;
		} else if((buffer[i]==' ' || buffer[i]=='\t') && inDoubleQuote==false && inSingleQuote==false) {
			arg=Trim(buffer.mid(start,i-start));
			if (arg.notEmpty()) add(arg);
			start=i+1;
		}
	}
	if (start<l) {
		arg=Trim(buffer.mid(start,l-start));
		if (arg.notEmpty()) add(arg);
	}
	return *this;
}


/*!\brief Element aus dem Array auslesen
 *
 * \desc
 * Gibt das Element an Position \p index des Arrays als Referenz zurück. Ist \p index größer als die Anzahl
 * Elemente des Arrays, wird eine Exception geworfen.
 *
 * @param index Gewünschtes Element
 * @return Referenz auf den Inhalt des Elements
 * \exception OutOfBoundsEception: Wird geworfen, wenn \p index größer als die Anzahl Elemente des Arrays ist
 */
String &Array::operator[](ssize_t index)
{
	return get(index);
}

/*!\brief Element aus dem Array als Konstante auslesen
 *
 * \desc
 * Gibt das Element an Position \p index des Arrays als Referenz zurück, dessen Inhalt nicht verändert werden
 * kann. Ist \p index größer als die Anzahl Elemente des Arrays, wird eine Exception geworfen.
 *
 * @param index Gewünschtes Element
 * @return Referenz auf den Inhalt des Elements
 * \exception OutOfBoundsEception: Wird geworfen, wenn \p index größer als die Anzahl Elemente des Arrays ist
 */
const String &Array::operator[](ssize_t index) const
{
	return get(index);
}


/*!\brief Inhalt eines anderen Arrays übernehmen
 *
 * \desc
 * Wie bei der Funktion Array::copy wird der aktuelle Inhalt des Arrays gelöscht und der
 * Inhalt des Arrays \p other übernommen.
 *
 * @param other Zu kopierendes Array
 * @return Referenz auf das Array
 */
Array& Array::operator=(const Array &other)
{
	copy(other);
	return *this;
}

/*!\brief Inhalt eines anderen Arrays hinzufügen
 *
 * \desc
 * Wie bei der Funktion Array::add wird der Inhalt des Arrays \p other am Ende des
 * bestehenden Arrays angefügt.
 *
 * @param other Zu kopierendes Array
 * @return Referenz auf das Array
 * \see Array::add(const Array &other)
 */
Array& Array::operator+=(const Array &other)
{
	add(other);
	return *this;
}

/*!\brief Prüfen, ob zwei Arrays identisch sind
 *
 * \desc
 * Mit dem Operator "==" wird geprüft, ob zwei Arrays inhaltlich identisch sind.
 * Dazu wird die Gesamtlänge des Arrays sowie jedes einzelne Element miteinander
 * verglichen.
 *
 * @param other Referenz auf ein zweites Array
 * @return Liefert \c true zurück, wenn beide Arrays identisch sind, sonst \c false.
 */
bool Array::operator==(const Array &other) const
{
	if (numElements!=other.numElements) return false;
	for (size_t i=0;i<numElements;i++)
		if (get(i)!=other.get(i)) return false;
	return true;
}

/*!\brief Prüfen, ob zwei Arrays unterschiedlich sind
 *
 * \desc
 * Mit dem Operator "!=" wird geprüft, ob zwei Arrays inhaltlich unterschiedlich sind.
 * Dazu wird die Gesamtlänge des Arrays sowie jedes einzelne Element miteinander
 * verglichen.
 *
 * @param other Referenz auf ein zweites Array
 * @return Liefert \c true zurück, wenn beide Arrays unterschiedlich sind, sonst \c false.
 */
bool Array::operator!=(const Array &other) const
{
	if (numElements!=other.numElements) return true;
	for (size_t i=0;i<numElements;i++)
		if (get(i)!=other.get(i)) return true;
	return false;
}


/*!\brief Zwei Arrays zusammenaddieren
 * \relates Array
 *
 * \desc
 * Mit diesem Operator wird der Inhalt zweier Arrays zu einem neuen Array
 * zusammenaddiert
 *
 * @param a1 Erstes Array
 * @param a2 Zweites Array
 * @return Neues Array mit den Werten beider Arrays
 */
Array operator+(const Array &a1, const Array& a2)
{
	Array ret(a1);
	ret.add(a2);
	return ret;
}


/*!\class Array::Iterator
 * \brief Iterator zum Durchwandern eines String Array
 *
 * \desc
 * Diese Klasse wird als Iterator zum Durchwandern eines Array verwendet.
 * Sie wird als Parameter zu den Funktionen Array::reset, Array::getFirst und Array::getNext
 * erwartet.
 */

Array::Iterator::Iterator()
{
	pos=0;
}


/*!\brief Elemente nach ihrem Wert sortieren
 *
 * \desc
 * Die einzelnen Elemente des Arrays werden alphabetisch sortiert. Duplikate bleiben erhalten
 */
void Array::sort()
{
	ppl7::AVLTree<ppl7::String, size_t> s;
	s.allowDupes(true);
	for (size_t i=0;i<numElements;i++) {
		s.add(get(i),i);
	}
	ppl7::AVLTree<ppl7::String, size_t>::Iterator it;
	s.reset(it);
	clear();
	while (s.getNext(it)) {
		add(it.key());
	}
}

/*!\brief Elemente nach ihrem Wert in umgekehrter Reihenfolge sortieren
 *
 * \desc
 * Die einzelnen Elemente des Arrays werden in umgekehrter alphabetischer Reihenfolge
 * sortiert. Duplikate bleiben erhalten.
 */
void Array::sortReverse()
{
	ppl7::AVLTree<ppl7::String, size_t> s;
	s.allowDupes(true);
	for (size_t i=0;i<numElements;i++) {
		s.add(get(i),i);
	}
	ppl7::AVLTree<ppl7::String, size_t>::Iterator it;
	s.reset(it);
	clear();
	while (s.getPrevious(it)) {
		add(it.key());
	}
}

/*!\brief Elemente nach ihrem Wert sortieren, Duplikate entfernen
 *
 * \desc
 * Die einzelnen Elemente des Arrays werden alphabetisch sortiert. Duplikate werden entfernt.
 */
void Array::sortUnique()
{
	ppl7::AVLTree<ppl7::String, size_t> s;
	s.allowDupes(false);
	for (size_t i=0;i<numElements;i++) {
		try {
			s.add(get(i),i);
		} catch (ppl7::DuplicateItemException &) {

		}
	}
	ppl7::AVLTree<ppl7::String, size_t>::Iterator it;
	s.reset(it);
	clear();
	while (s.getNext(it)) {
		add(it.key());
	}
}

/*!\brief Duplikate entfernen
 *
 * \desc
 * Elemente, die mehrfach im Array vorkommen, werden entfernt. Die Reihenfolge der Elemente
 * bleibt bestehen.
 *
 * \note
 * In Version 6.x.x der Bibliothek hat die Funktion die Elemente zusätzlich alphabetisch sortiert,
 * was jetzt nicht mehr der Fall ist.
 */
void Array::makeUnique()
{
	String value;
	ppl7::AVLTree<ppl7::String, size_t> s;
	s.allowDupes(false);
	for (size_t i=0;i<numElements;i++) {
		value=get(i);
		if (s.exists(value)) {
			erase(i);
			i--;
		} else {
			s.add(value,i);
		}
	}
}

size_t Array::indexOf(const String &search)
{
	if (!numElements) throw ItemNotFoundException(search);
	ROW *r=(ROW*)rows;
	for (size_t i=0;i<numElements;i++) {
		if (*r[i].value==search) return i;
	}
	throw ItemNotFoundException(search);
}

bool Array::has(const String &search)
{
	if (!numElements) return false;
	ROW *r=(ROW*)rows;
	for (size_t i=0;i<numElements;i++) {
		if (*r[i].value==search) return true;
	}
	return false;
}



/*!\brief Inhalt eines Arrays alphabetisch sortieren
 * \relates Array
 *
 * \desc
 * Die einzelnen Elemente des Arrays \p array werden alphabetisch sortiert.
 *
 * \param array Das zu sortierende Array
 * \param unique Optionaler Parameter: true=Duplikate werden entfernt
 * false=Duplikate bleiben erhalten (Default)
 * \return Sortieres Array
 */
Array Sort(const Array &array, bool unique)
{
	Array ret;
	ppl7::AVLTree<ppl7::String, size_t> s;
	s.allowDupes(!unique);
	size_t num=array.count();
	for (size_t i=0;i<num;i++) {
		try {
			s.add(array.get(i),i);
		} catch (ppl7::DuplicateItemException &) {

		}
	}
	ppl7::AVLTree<ppl7::String, size_t>::Iterator it;
	s.reset(it);
	while (s.getNext(it)) {
		ret.add(it.key());
	}
	return(ret);
}

/*!\brief Inhalt eines Arrays in umgekehrter Reihenfolge sortieren
 * \relates Array
 *
 * \desc
 * Die einzelnen Elemente des Arrays \p array werden in alphabetisch umgekehrter Reihenfolge sortiert.
 *
 * \param array Das zu sortierende Array
 * \param unique Optionaler Parameter: true=Duplikate werden entfernt
 * false=Duplikate bleiben erhalten (Default)
 * \return Sortieres Array
 */
Array SortReverse(const Array &array, bool unique)
{
	Array ret;
	ppl7::AVLTree<ppl7::String, size_t> s;
	s.allowDupes(!unique);
	size_t num=array.count();
	for (size_t i=0;i<num;i++) {
		try {
			s.add(array.get(i),i);
		} catch (ppl7::DuplicateItemException &) {

		}
	}
	ppl7::AVLTree<ppl7::String, size_t>::Iterator it;
	s.reset(it);
	while (s.getPrevious(it)) {
		ret.add(it.key());
	}
	return (ret);
}


} // EOF namespace ppl7
