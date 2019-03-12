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
#ifdef HAVE_TYPES_H
#include <types.h>
#endif

#include <ostream>

#include "ppl7.h"



namespace ppl7 {

/*!\class AssocArray
 * \ingroup PPLGroupDataTypes
 * \brief Komplexes mehrdimensionales %Array mit Strings als Schlüssel
 *
 * \desc
 * Die Klasse AssocArray dient als Container für beliebige Key-Value-Paare. Ein Schlüssel
 * (Key) besteht aus einem String, der aus beliebigen Zeichen bestehen kann. Ein Value kann
 * veschiedene Datentypen enthalten. Gegenwärtig werden folgende Datentypen unterstützt:
 * - String
 * - Array
 * - ByteArray
 * - ByteArrayPtr
 * - AssocArray
 * - DateTime
 * \par
 * Die Schlüssel werden sortiert in einem AVL-Baum verwaltet (siehe AVLTree), so dass auch bei
 * sehr großen Arrays eine schnelle Verarbeitung gewährleistet ist. Gross-/Kleinschreibung wird
 * ignoriert, der Schlüssel "TEST" wäre also identisch mit "test" oder "Test".
 * \par
 * Mehrdimensionale Arrays sind möglich, indem einem Schlüssel als Wert einfach ein anderes Array
 * zugeordnet wird. In einem solchen Array kann jedes Element direkt angesprochen werden, indem man
 * die einzelnen Schlüssel durch Slash (/) getrennt zu einem einzigen Schlüssel zusammenfasst.
 * \par
 * Mehrdimensionale Arrays werden automatisch generiert. Gibt man bei einem leeren %Array dem Schlüssel
 * <tt>"ebene1/ebene2/key"</tt> einen Wert, werden automatisch folgende Aktionen ausgeführt:
 * - Es wird ein neues AssocArray generiert und mit dem Schlüssel "ebene1" in das %Array eingefügt
 * - In das %Array "ebene1" wird ein weiteres neues %Array mit dem Schlüssel "ebene2" eingefügt
 * - In das %Array "ebene2" wird der eigentliche Wert unter dem Schlüssel "key" eingefügt
 *
 * \par Beispiel:
 * Einen Wert setzen und wieder auslesen:
 * \code
 * ppl7::AssocArray a;
 * // Wert setzen
 * a.set("ebene1/ebene2/key","Ein Wert");
 * // Wert auslesen
 * a.get("ebene1/ebene2/key").toString().printnl();
 * \endcode
 * \par
 * Durch ein AssocArray durchiterieren:
 * \code
void IterateArray(const ppl7::AssocArray &a)
{
	ppl7::AssocArray::Iterator it;
	a.reset(it);
	while (a.getNext(it)) {
		const ppl7::String &key=it.key();
		const ppl7::Variant &var=*it.value().value;
		if (var.isString()) {
			cout << "Key: " << key << ", Value: " << var.toString() << endl;
		}
	}
}
 * \endcode
 * \par
 * Wenn von vorneherein bekannt ist, dass im Array nur Strings vorhanden sind, kann man auch noch auf diese
 * Weise durchiterieren:
 * \code
void IterateArray(const ppl7::AssocArray &a)
{
	ppl7::AssocArray::Iterator it;
	ppl7::String Key, Value;
	a.reset(it);
	while (a.GetNext(it,Key,Value)) {
		cout << "Key: " << Key << ", Value: " << Value << endl;
	}
}
 * \endcode
 */


/*!\class AssocArray::ArrayKey
 * \brief Datentyp für Schlüssel
 *
 * \desc
 * Das AssocArray verwendet einen eigenen von der String-Klasse abgeleiteten Datentyp. Dieser
 * unterscheidet sich von der String-Klasse nur durch die Vergleichoperatoren. Diese behandelt
 * rein nummerische Schlüssel anders als alphanummerische.
 *
 */

AssocArray::ArrayKey::ArrayKey()
{

}

AssocArray::ArrayKey::ArrayKey(const String &other)
{
	set(other);
}

/*!\brief Generische Vergleichfunktion
 *
 * \desc
 * Diese Funktion vergleicht den eigenen Wert mit dem aus \p str. Sind Beide Strings nummerisch,
 * wird ein nummerischer Vergleich durchgeführt, andernfalls ein Stringvergleich. Die Funktion wird
 * von den Vergleichoperatoren aufgerufen.
 *
 * @param str Vergleichswert
 * @return Liefert folgende Werte zurück:
 * - 0: Beide Werte sind identisch
 * - -1: Der angegebene Wert \p str ist kleiner als der eigene
 * - 1: Der angegebene Wert \p str ist größer als der eigene
 */
int AssocArray::ArrayKey::compare(const ArrayKey &str) const
{
	if (isNumeric()==true && str.isNumeric()==true) {
		pplint64 v1=toInt64();
		pplint64 v2=str.toInt64();
		if (v2<v1) return 1;
		if (v2>v1) return -1;
		return 0;
	}
	int cmp=strCaseCmp(str);
	if (cmp==0) return 0;
	if (cmp<0) return -1;
	return 1;
}

AssocArray::ArrayKey& AssocArray::ArrayKey::operator=(const String &str)
{
	set(str);
	return *this;
}

bool AssocArray::ArrayKey::operator<(const ArrayKey &str) const
{
	int c=compare(str);
	if (c<0) return true;
	return false;
}
bool AssocArray::ArrayKey::operator<=(const ArrayKey &str) const
{
	int c=compare(str);
	if (c<=0) return true;
	return false;
}

bool AssocArray::ArrayKey::operator==(const ArrayKey &str) const
{
	int c=compare(str);
	if (c==0) return true;
	return false;
}

bool AssocArray::ArrayKey::operator!=(const ArrayKey &str) const
{
	int c=compare(str);
	if (c!=0) return true;
	return false;
}

bool AssocArray::ArrayKey::operator>=(const ArrayKey &str) const
{
	int c=compare(str);
	if (c>=0) return true;
	return false;
}

bool AssocArray::ArrayKey::operator>(const ArrayKey &str) const
{
	int c=compare(str);
	if (c>0) return true;
	return false;
}


/*!\brief Konstruktor des Assoziativen Arrays
 *
 * \desc
 * Initialisiert die Instanz mit 0 und initialisiert den AVL-Baum.
 */
AssocArray::AssocArray()
{
	maxint=0;
}

/*!\brief Copy-Konstruktor des Assoziativen Arrays
 *
 * \desc
 * Macht eine Kopie des Assoziativen Arrays \p other.
 *
 * \param[in] other Referenz auf zu kopierendes Assoziatives Array
 *
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
AssocArray::AssocArray(const AssocArray &other)
{
	maxint=0;
	add(other);
}

/*!\brief Destruktor der Klasse
 *
 * \desc
 * Der Destruktor ruft die Funktion AssocArray::clear auf, um alle vorhandenen Elemente zu
 * löschen.
 */
AssocArray::~AssocArray()
{
	clear();
}

/*!\brief Inhalt des Arrays löschen
 *
 * \desc
 * Mit dieser Funktion wird der komplette Inhalt des Arrays gelöscht. Dabei der Destruktor für jedes
 * vorhandene Element aufgerufen, der wiederum sicherstellt, dass die darin enthaltenen Daten
 * ordnungsgemäß gelöscht werden.
 *
 */
void AssocArray::clear()
{
	iterator it;
	for (it=Tree.begin();it!=Tree.end();++it) {
		delete (*it).second;
	}
	Tree.clear();
	maxint=0;
}

/*!\brief Interne Funktion zum Suchen eines Elements
 *
 * \desc
 * Diese Funktion zerlegt den angegebenen Schlüssel (\p key) in seine einzelnen Elemente.
 * Als Trennzeichen wird wie bei einer Unix-Pfadangabe der Slash (/) verwendet. Die Funktion
 * sucht zunächst nach dem erste Element des Schlüssels im eigenen Baum. Ist dies vorhanden
 * und handelt es sich bei dessen Datentyp wieder um ein AssocArray, wird deren
 * findInternal-Funktion mit dem restlichen Schlüssel rekursiv aufgerufen. Dies geschieht
 * solange, bis das letzte Element des Keys gefunden wurde.
 *
 * \param[in] key String mit dem gesuchten Schlüssel
 * \return Konnte der Schlüssel gefunden werden, wir der Pointer auf das Element (Variant)
 * zurückgegeben. Wurde der Schlüssel nicht gefunden, wird NULL zurückgegeben
 * \exception InvalidKeyException: Wird geworfen, wenn der Schlüssel ungültig oder leer ist
  * \note
 * Die Funktion wird von allen Get...- und Concat-Funktionen verwendet.
 */
Variant *AssocArray::findInternal(const ArrayKey &key) const
{
	//printf ("AssocArray::findInternal (key=%ls)\n",(const wchar_t*)key);
	Array tok(key,"/",0,true);
	if (tok.count()==0) throw InvalidKeyException(key);
	ArrayKey firstkey=tok.shift();
	ArrayKey rest=tok.implode("/");

	const_iterator it=Tree.find(firstkey);
	if (it==Tree.end()) return NULL;
	// Ist noch was im Pfad rest?
	if (tok.count()>0) {			// Ja, koennen wir iterieren?
		if (it->second!=NULL && it->second->isAssocArray()) {
			return it->second->toAssocArray().findInternal(rest);
		} else {
			return NULL;
		}
	}
	return it->second;
}

/*!\brief Interne Funktion, die ein Element im Baum sucht oder anlegt
 *
 * \desc
 * Diese Funktion durchsucht den Baum nach dem gewünschten Element. Ist es vorhanden, wird dessen Pointer
 * zurückgeliefert, wenn nicht, wird es angelegt, jedoch ohne Daten. Bei verschachtelten Schlüsseln wird
 * die Funktion rekursiv aufgerufen, bis das letzte Element erreicht ist. Die Funktion wird intern von allen
 * Funktionen verwendet, die Daten in das Array speichern.
 *
 * \param[in] key Pointer auf den Namen des Schlüssels
 * \param[in] var Pointer auf die Daten, die unter diesem Schlüssel abgelegt werden sollen
 * \return Bei Erfolg liefert die Funktion einen Pointer auf das gewünschte Element zurück.
 * Im Fehlerfall wird eine Exception geworfen.
 *
 * \exception InvalidKeyException: Wird geworfen, wenn der Schlüssel ungültig oder leer ist
 * \exception std::bad_alloc: Kein Speicher mehr frei
 *
 * \remarks Bei der Angabe eines verschachtelten Schlüssels kann es vorkommen, dass bereits vorhandene
 * Elemente überschrieben werden. Beispiel:
 *
 * Das Element <tt>ebene1/schlüssel1</tt> ist im Baum bereits vorhanden und beinhaltet einen String. Nun wird
 * das neue Element <tt>ebene1/schlüssel1/unterschlüssel1</tt> angelegt. Da Schlüssel eindeutig sein müssen,
 * wird der String <tt>ebene1/schlüssel1</tt> gelöscht und in ein Array umgewandelt.
 */
void AssocArray::createTree(const ArrayKey &key, Variant *var)
{
	Array tok(key,"/",0,true);
	if (tok.count()==0) throw InvalidKeyException(key);
	String firstkey=tok.shift();
	ArrayKey rest=tok.implode("/");
	//printf ("firstkey=%ls, rest=%ls\n",(const wchar_t *)firstkey,(const wchar_t *)rest);
	if (firstkey=="[]") {
		firstkey.setf("%llu",maxint);
		maxint++;
	}
	// Beginnt Firstkey mit einer Zahl?
	if (firstkey.isNumeric()) {
		ppluint64 keyint=firstkey.toInt64();
		if (keyint>=maxint) maxint=keyint+1;
	}

	iterator it=Tree.find(firstkey);
	if (it!=Tree.end()) {
		// Ist noch was im Pfad rest?
		if (tok.count()>0) {          // Ja, koennen wir iterieren?
			if (it->second->isAssocArray()==false) {
				delete (it->second);		// Nein, wir loeschen daher diesen Zweig und machen ein Array draus
				it->second=new Variant(ppl7::AssocArray());
			}
			it->second->toAssocArray().createTree(rest,var);
			return;
		}
		// Nein, wir haben die Zielposition gefunden
		delete it->second;
		it->second=var;
		return;
	}

	// Key ist nicht in diesem Array, wir legen ihn an

	// Ist noch was im Pfad rest?
	if (tok.count()>0) {          // Ja, wir erstellen ein Array und iterieren
		//printf ("Iteration\n");
		Variant *newnode=new Variant(ppl7::AssocArray());
		Tree.insert(std::pair<ArrayKey, Variant*>(firstkey,newnode));
		newnode->toAssocArray().createTree(rest,var);
	} else {
		Tree.insert(std::pair<ArrayKey, Variant*>(firstkey,var));
	}
}


/*!\brief Anzahl Schlüssel zählen
 *
 * \desc
 * Diese Funktion liefert die Anzahl Schlüssel auf dieser Ebene des Array zurück.
 *
 * \param[in] recursive Falls recursive auf true gesetzt wird die Funktion rekusriv für jeden
 * Schlüssel aufgerufen, dessen Wert ebenfalls ein Array ist.
 *
 * \returns Anzahl Schlüssel
 */
size_t AssocArray::count(bool recursive) const
{
	if (!recursive) return Tree.size();
	const_iterator it;
	size_t num=Tree.size();
	for (it=Tree.begin();it!=Tree.end();++it) {
		if (it->second->isAssocArray()) num+=it->second->toAssocArray().count(recursive);
	}
	return num;
}

/*!\brief Anzahl Elemente
 *
 * \desc
 * Diese Funktion liefert die Anzahl Elemente auf dieser Ebene des Arrays zurück.
 *
 * \returns Anzahl Elemente
 */
size_t AssocArray::size() const
{
	return Tree.size();
}

/*!\brief Anzahl Schlüssel für ein bestimmtes Element zählen
 *
 * \desc
 * Diese Funktion liefert die Anzahl Schlüssel zurück, die in dem angegebenen Key enthalten sind.
 *
 * \param[in] key Schlüssel-Name eines Assoziativen Arrays innerhalb dieses Arrays
 * \param[in] recursive Falls recursive auf true gesetzt wird die Funktion rekusriv für jeden
 * Schlüssel aufgerufen, dessen Wert ebenfalls ein Array ist.
 *
 * \returns Anzahl Schlüssel
 */
size_t AssocArray::count(const String &key, bool recursive) const
{
	const Variant *p=findInternal(key);
	if (!p) return (size_t) 0;
	if (p->isAssocArray()) return p->toAssocArray().count(recursive);
	return 1;
}

/*!\brief Inhalt des Arrays ausgeben
 *
 * \desc
 * Diese Funktion dient Debugging-Zwecken. Der Aufruf bewirkt, dass der Inhalt des kompletten Arrays auf
 * STDOUT ausgegeben wird.
 *
 * \param[in] prefix Optionaler Text, der bei der Ausgabe jedem Element vorangestellt wird
 *
 * \par Beispiel:
 * \code
ppl7::AssocArray a;
ppl7::Binary bin;
bin.load("main.cpp");

a.set("key1","value1");
a.set("array1/unterkey1","value2");
a.set("array1/unterkey2","value3");
a.set("array1/noch ein array/unterkey1","value4");
a.set("array1/unterkey2","value5");
a.set("key2","value6");
a.set("dateien/main.cpp",bin);
a.set("array2/unterkey1","value7");
a.set("array2/unterkey2","value8");
a.set("array2/unterkey1","value9");
a.list("prefix");
\endcode
	Ausgabe:
\code
prefix/array1/noch ein array/unterkey1=value4
prefix/array1/unterkey1=value2
prefix/array1/unterkey2=value5
prefix/array2/unterkey1=value9
prefix/array2/unterkey2=value8
prefix/dateien/main.cpp=Binary, 806 Bytes
prefix/key1=value1
prefix/key2=value6
\endcode
 *
 * \remarks Die Funktion gibt nur "lesbare" Element aus. Enthält das Array Pointer oder Binaries, wird das Element zwar
 * ausgegeben, jedoch werden als Wert nur Meta-Informationen ausgegeben (Datentyp, Pointer, Größe).
 */
void AssocArray::list(const String &prefix) const
{
	String key;
	String pre;
	if (prefix.notEmpty()) key=prefix+"/";

	const_iterator it;
	for (it=Tree.begin();it!=Tree.end();++it) {
		Variant *p=it->second;
		if (p->isString()) {
			PrintDebug("%s%s=%s\n",(const char*)key,(const char*)it->first,(const char*)p->toString().getPtr());
		} else if (p->isWideString()) {
			PrintDebug("%s%s=%ls\n",(const char*)key,(const char*)it->first,(const wchar_t*)p->toWideString().getPtr());
		} else if (p->isByteArray()) {
			PrintDebug("%s%s=ByteArray, %zu Bytes\n",(const char*)key,(const char*)it->first,p->toByteArray().size());
		} else if (p->isByteArrayPtr()) {
			PrintDebug("%s%s=ByteArrayPtr, %zu Bytes\n",(const char*)key,(const char*)it->first,p->toByteArrayPtr().size());
		} else if (p->isAssocArray()) {
			pre.setf("%s%s",(const char*)key,(const char*)it->first);
			p->toAssocArray().list(pre);
		} else if (p->isPointer()) {
			PrintDebug("%s%s=Pointer, %tu\n",(const char*)key,(const char*)it->first,(std::ptrdiff_t)p->toPointer().ptr());
		} else if (p->isArray()) {
			const Array &a=(const Array &)*p;
			for (size_t i=0;i<a.size();i++) {
				PrintDebug("%s%s/Array(%zu)=%s\n",(const char*)key,(const char*)it->first,i,(const char*)a[i]);
			}
		} else if (p->isDateTime()) {
			PrintDebug("%s%s=DateTime %s\n",(const char*)key,(const char*)it->first, (const char*) p->toDateTime().getISO8601withMsec());
		} else {
			PrintDebug("%s%s=UnknownDataType Id=%i\n",(const char*)key,(const char*)it->first,p->type());
		}
	}
}

/*!\brief %String hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt eines Strings dem Array hinzu.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Wert
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const String &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

void AssocArray::set(const String &key, const WideString &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}


/*!\brief %String mit bestimmter Länge hinzufügen
 *
 * \desc
 * Diese Funktion fügt die ersten \p size Zeichen des Strings \p value unter dem
 * Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Wert
 * \param[in] size Anzahl Zeichen, die aus dem String \p value übernommen werden sollen
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const String &value, size_t size)
{
	Variant *var=new Variant(String(value,size));
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %DateTime hinzufügen
 *
 * \desc
 * Diese Funktion fügt den in \p value angegebenen Zeitstempel unter dem
 * Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Zeitstempel
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const DateTime &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %ByteArray hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt des ByteArrays \p value
 * unter dem Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Daten
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const ByteArray &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %ByteArrayPtr hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt des ByteArrayPtrs \p value
 * unter dem Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Daten
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const ByteArrayPtr &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %Array hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt des Arrays \p value
 * unter dem Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Daten
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const Array &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %AssocArray hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt des AssocArrays \p value
 * unter dem Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Daten
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const AssocArray &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %Pointer hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt des Pointers \p value
 * unter dem Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Daten
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::set(const String &key, const Pointer &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %Variant hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt des Variants \p value
 * unter dem Schlüssel \p key in das Assoziative Array ein.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Daten
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception TypeConversionException: Der Datentyp des Variants wurde nicht
 * erkannt oder wird nicht unterstützt.
 */
void AssocArray::set(const String &key, const Variant &value)
{
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief Formatierten String hinzufügen
 *
 * \desc
 * Diese Funktion fügt den Inhalt eines formatierten Strings dem Array hinzu.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] fmt Pointer auf den Format-String des Wertes
 * \param[in] ... Beliebig viele Parameter, die vom Formatstring verwendet werden
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::setf(const String &key, const char *fmt, ...)
{
	String value;
	va_list args;
	va_start(args, fmt);
	value.vasprintf(fmt,args);
	va_end(args);
	Variant *var=new Variant(value);
	try {
		createTree(key,var);
	} catch (...) {
		delete var;
		throw;
	}
}

/*!\brief %String verlängern
 *
 * \desc
 * Diese Funktion fügt den Inhalt des Strings \p value an den bereits vorhandenen
 * Wert des Schlüssels \p key an. Falls der optionale Parameter \p concat einen Wert
 * enthält, wird dieser als Trennung zwischen bestehendem und neuem String verwendet.
 * War der Schlüssel bisher nicht vorhanden, wird ein neuer angelegt.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] value Wert
 * \param[in] concat Trennzeichen (Optional, Default=keins)
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception TypeConversionException: Schlüssel ist bereits vorhanden, enthält aber keinen String
 */
void AssocArray::append(const String &key, const String &value, const String &concat)
{
	Variant *node=findInternal(key);
	if (!node) {
		set(key,value);
		return;
	}
	if (concat.notEmpty()) node->toString().append(concat);
	node->toString().append(value);
}

/*!\brief %String mit Formatiertem String verlängern
 *
 * \desc
 * Diese Funktion erstellt zuerst einen neuen String anhand des Formatstrings
 * \p fmt und der zusätzlichen optionalen Parameter. Dieser wird an den bereits vorhandenen
 * Wert des Schlüssels \p key angehangen. Falls der optionale Parameter \p concat einen Wert
 * enthält, wird dieser als Trennung zwischen bestehendem und neuem String verwendet.
 * War der Schlüssel bisher nicht vorhanden, wird ein neuer angelegt.
 *
 * \param[in] key Name des Schlüssels
 * \param[in] concat Trennzeichen (Optional, Default=keins)
 * \param[in] fmt Formatstring
 * \param[in] ... Optionale Parameter
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception TypeConversionException: Schlüssel ist bereits vorhanden, enthält aber keinen String
 */
void AssocArray::appendf(const String &key, const String &concat, const char *fmt, ...)
{
	String var;
	va_list args;
	va_start(args, fmt);
	var.vasprintf(fmt,args);
	va_end(args);
	append(key,var,concat);
}

/*!\brief %AssocArray kopieren
 *
 * \desc
 * Mit dieser Funktion wird der komplette Inhalt des Assoziativen Arrays \p other
 * in dieses hineinkopiert. Das Array wird vorher nicht gelöscht, so dass vorhandene
 * Schlüssel erhalten bleiben. Gibt es in \p other jedoch gleichnamige Schlüssel,
 * werden die bisherigen Werte überschrieben.
 *
 * \param[in] a Das zu kopierende AssocArray
 *
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
void AssocArray::add(const AssocArray &other)
{
	const_iterator it;
	for (it=other.Tree.begin();it!=other.Tree.end();++it) {
		set(it->first,*it->second);
	}
}




/*!\brief Schlüssel auslesen
 *
 * \desc
 * Diese Funktion liefert den Wert des Schlüssels \p key als Variant zurück. Dieser kann
 * von der aufrufenden Anwendung in den jeweiligen Datentyp umgewandelt werden.
 *
 * @param key Name des Schlüssels
 * @return Referenz auf einen Variant mit dem Wert des Schlüssels
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception KeyNotFoundException: Schlüssel wurde nicht gefunden
 * \example
 * Der Variant kann z.B. folgendermaßen in einen String umgewandelt werden:
 * \code
ppl7::String &str=a.get(L"key1").toString();
\endcode
 */
Variant& AssocArray::get(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	return (*node);
}

/*!\brief Schlüssel vorhanden
 *
 * \desc
 * Diese Funktion prüft, ob der Schlüssels \p key im Assoziativen Array enthalten ist.
 *
 * @param key Name des Schlüssels
 * @return Liefert \c true zurück, wenn der Schlüssel vorhanden ist, sonst \c false
 * \exception InvalidKeyException: Ungültiger Schlüssel
 */
bool AssocArray::exists(const String &key) const
{
	if (findInternal(key)) return true;
	return false;
}

/*!\brief String auslesen
 *
 * \desc
 * Diese Funktion liefert den Wert des Schlüssels \p key als String zurück, sofern
 * der Schlüssel auch tatsächlich einen String enthält.
 *
 * @param key Name des Schlüssels
 * @return Referenz auf einen String mit dem Wert des Schlüssels
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception KeyNotFoundException: Schlüssel wurde nicht gefunden
 */
String& AssocArray::getString(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	if (!node->isString()) throw TypeConversionException("%s is not a String",(const char*)key);
	return node->toString();
}

String& AssocArray::getString(const String &key, String &default_value) const
{
	Variant *node=findInternal(key);
	if (!node) return default_value;
	if (node->isString() || node->isWideString()) return node->toString();
	return default_value;
}

int AssocArray::getInt(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	if (node->isString()) return node->toString().toInt();
	if (node->isWideString()) return node->toWideString().toInt();
	throw TypeConversionException("%s cannot be converted to Int",(const char*)key);
}

int AssocArray::getInt(const String &key, int default_value) const
{
	Variant *node=findInternal(key);
	if (!node) return default_value;
	if (node->isString()) return node->toString().toInt();
	if (node->isWideString()) return node->toWideString().toInt();
	return default_value;
}

long long AssocArray::getLongLong(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	if (node->isString()) return node->toString().toLongLong();
	if (node->isWideString()) return node->toWideString().toLongLong();
	throw TypeConversionException("%s cannot be converted to long long int",(const char*)key);
}

long long AssocArray::getLongLong(const String &key, long long default_value) const
{
	Variant *node=findInternal(key);
	if (!node) return default_value;
	if (node->isString()) return node->toString().toLongLong();
	if (node->isWideString()) return node->toWideString().toLongLong();
	return default_value;
}


/*!\brief Key vorhanden und True
 *
 * \desc
 * Liefert True zurück, wenn der Schlüssel \b key vorhanden ist,
 * und dessen Value einen String oder WideString enthält, dessen
 * Boolean Wert True entspricht.
 *
 * @param key Name des Schlüssels
 * @return True oder False
 */
bool AssocArray::isTrue(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) return false;
	if (node->isString()) return node->toString().isTrue();
	if (node->isWideString()) return node->toWideString().isTrue();
	return false;
}


/*!\brief AssocArray auslesen
 *
 * \desc
 *
 * @param key Name des Schlüssels
 * @return Referenz auf einen String mit dem Wert des Schlüssels
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception KeyNotFoundException: Schlüssel wurde nicht gefunden
 */
AssocArray& AssocArray::getAssocArray(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	if (!node->isAssocArray()) throw TypeConversionException("%s is not an AssocArray",(const char*)key);
	return node->toAssocArray();
}

AssocArray& AssocArray::getAssocArray(const String &key, AssocArray &default_value) const
{
	Variant *node=findInternal(key);
	if (!node) return default_value;
	if (node->isAssocArray()) return node->toAssocArray();
	return default_value;
}

Array& AssocArray::getArray(const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	if (!node->isArray()) throw TypeConversionException("%s is not an Array",(const char*)key);
	return node->toArray();
}

Array& AssocArray::getArray(const String &key, Array &default_value) const
{
	Variant *node=findInternal(key);
	if (!node) return default_value;
	if (node->isArray()) return node->toArray();
	return default_value;
}



/*!\brief Einzelnen Schlüssel löschen
 *
 * \desc
 * Mit dieser Funktion wird ein einzelner Schlüssel aus dem Array gelöscht.
 *
 * \param[in] key String mit dem Namen des zu löschenden Schlüssels
 *
 */
void AssocArray::erase(const String &key)
{
	Array tok(key,"/",0,true);
	if (tok.count()==0) throw InvalidKeyException(key);
	ArrayKey firstkey=tok.shift();
	ArrayKey rest=tok.implode("/");
	iterator it=Tree.find(firstkey);
	if (it==Tree.end()) return;		// nothing to do
	// Ist noch was im Pfad rest?
	if (tok.count()>0) {			// Ja, koennen wir iterieren?
		if (it->second!=NULL && it->second->isAssocArray()) {
			it->second->toAssocArray().erase(rest);
			return;
		} else {
			return;
		}
	}
	Tree.erase(it);
}

/*!\brief Einzelnen Schlüssel löschen
 *
 * \desc
 * Mit dieser Funktion wird ein einzelner Schlüssel aus dem Array gelöscht.
 *
 * \param[in] key String mit dem Namen des zu löschenden Schlüssels
 *
 */
void AssocArray::remove(const String &key)
{
	erase(key);
}

AssocArray::iterator AssocArray::begin()
{
	return Tree.begin();
}

AssocArray::const_iterator AssocArray::begin() const
{
	return Tree.begin();
}

AssocArray::iterator AssocArray::end()
{
	return Tree.end();
}

AssocArray::const_iterator AssocArray::end() const
{
	return Tree.end();
}

AssocArray::reverse_iterator AssocArray::rbegin()
{
	return Tree.rbegin();
}

AssocArray::const_reverse_iterator AssocArray::rbegin() const
{
	return Tree.rbegin();
}


AssocArray::reverse_iterator AssocArray::rend()
{
	return Tree.rend();
}

AssocArray::const_reverse_iterator AssocArray::rend() const
{
	return Tree.rend();
}



/*!\brief Zeiger für das Durchwandern des Arrays zurücksetzen
 *
 * \desc
 * Mit dieser Funktion wird der Iterator \p it, der zum Durchwandern des Arrays mit den
 * Funktion AssocArray::getNext und AssocArray::getPrevious benötigt wird,
 * auf den Anfang zurückgesetzt.
 *
 * \param[in] it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 *
 */
void AssocArray::reset(Iterator &it) const
{
	it.reset=true;
}

void AssocArray::reset(ReverseIterator &it) const
{
	it.reset=true;
}

/*!\brief Erstes Element zurückgeben
 *
 * \desc
 * Diese Funktion liefert das erste Element des Arrays zurück. Falls der optionale Parameter
 * \p type verwendet wird, liefert die Funktion das erste Element dieses Typs zurück.
 *
 * @param it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param type Optional der gewünschte Datentyp (siehe Variant::Type)
 * @return \c true, wenn ein Element vorhanden war, sonst \c false
 */
bool AssocArray::getFirst(Iterator &it, Variant::DataType type) const
{
	it.it=Tree.begin();
	it.reset=false;
	while (1) {
		if (it.it==Tree.end()) return false;
		if (type==Variant::TYPE_UNKNOWN) break;
		if (type==it.it->second->type()) break;
		++it.it;
	}
	return true;
}

/*!\brief Nächstes Element zurückgeben
 *
 * \desc
 * Diese Funktion liefert das nächste Element des Arrays zurück. Falls der optionale Parameter
 * \p type verwendet wird, liefert die Funktion das nächste Element dieses Typs zurück.
 *
 * @param it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param type Optional der gewünschte Datentyp (siehe Variant::Type)
 * @return \c true, wenn ein Element vorhanden war, sonst \c false
 */
bool AssocArray::getNext(Iterator &it, Variant::DataType type) const
{
	if (it.reset) return getFirst(it,type);
	if (it.it==Tree.end()) return false;
	while (1) {
		++it.it;
		if (it.it==Tree.end()) return false;
		if (type==Variant::TYPE_UNKNOWN) break;
		if (type==it.it->second->type()) break;
	}
	return true;
}

/*!\brief Letztes Element zurückgeben
 *
 * \desc
 * Diese Funktion liefert das letzte Element des Arrays zurück. Falls der optionale Parameter
 * \p type verwendet wird, liefert die Funktion das letzte Element dieses Typs zurück.
 *
 * @param it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param type Optional der gewünschte Datentyp (siehe Variant::Type)
 * @return \c true, wenn ein Element vorhanden war, sonst \c false
 */
bool AssocArray::getLast(ReverseIterator &it, Variant::DataType type) const
{
	it.it=Tree.rbegin();
	it.reset=false;
	while (1) {
		if (it.it==Tree.rend()) return false;
		if (type==Variant::TYPE_UNKNOWN) break;
		if (type==it.it->second->type()) break;
		++it.it;
	}
	return true;
}

/*!\brief Vorhergehendes Element zurückgeben
 *
 * \desc
 * Diese Funktion liefert das vorhergehende Element des Arrays zurück. Falls der optionale Parameter
 * \p type verwendet wird, liefert die Funktion das vorhergehende Element dieses Typs zurück.
 *
 * @param it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param type Optional der gewünschte Datentyp (siehe Variant::Type)
 * @return \c true, wenn ein Element vorhanden war, sonst \c false
 */
bool AssocArray::getPrevious(ReverseIterator &it, Variant::DataType type) const
{
	if (it.reset) return getLast(it,type);
	if (it.it==Tree.rend()) return false;
	while (1) {
		++it.it;
		if (it.it==Tree.rend()) return false;
		if (type==Variant::TYPE_UNKNOWN) break;
		if (type==it.it->second->type()) break;
	}
	return true;
}

/*!\brief Ersten %String im %Array finden und Key und Value in Strings speichern
 *
 * \desc
 * Diese Funktion sucht den ersten %String im %Array und speichert dessen
 * Schlüssel im Parameter \p key und den Wert in \p value;
 *
 * @param[in,out] it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param[out] key String, in dem der Name des Schlüssels gespeichert werden soll
 * @param[out] value String, in dem der Wert gespeichert werden soll.
 * \return Solange Elemente gefunden werden, liefert die Funktion \c true zurück, sonst \c false.
 */
bool AssocArray::getFirst(Iterator &it, String &key, String &value) const
{
	it.it=Tree.begin();
	it.reset=false;
	while (1) {
		if (it.it==Tree.end()) return false;
		if (it.it->second->isString()) break;
		++it.it;
	}
	key.set(it.it->first);
	value.set(it.it->second->toString());
	return true;
}

/*!\brief Nächsten %String im %Array finden und Key und Value in Strings speichern
 *
 * \desc
 * Diese Funktion sucht den nächsten %String im %Array und speichert dessen
 * Schlüssel im Parameter \p key und den Wert in \p value;
 *
 * @param[in,out] it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param[out] key String, in dem der Name des Schlüssels gespeichert werden soll
 * @param[out] value String, in dem der Wert gespeichert werden soll.
 * \return Solange Elemente gefunden werden, liefert die Funktion \c true zurück, sonst \c false.
 */
bool AssocArray::getNext(Iterator &it, String &key, String &value) const
{
	if (it.reset) return getFirst(it,key,value);
	if (it.it==Tree.end()) return false;
	while (1) {
		++it.it;
		if (it.it==Tree.end()) return false;
		if (it.it->second->isString()) break;
	}
	key.set(it.it->first);
	value.set(it.it->second->toString());
	return true;
}

/*!\brief Letzten %String im %Array finden und Key und Value in Strings speichern
 *
 * \desc
 * Diese Funktion sucht den letzten %String im %Array und speichert dessen
 * Schlüssel im Parameter \p key und den Wert in \p value;
 *
 * @param[in,out] it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param[out] key String, in dem der Name des Schlüssels gespeichert werden soll
 * @param[out] value String, in dem der Wert gespeichert werden soll.
 * \return Solange Elemente gefunden werden, liefert die Funktion \c true zurück, sonst \c false.
 */
bool AssocArray::getLast(ReverseIterator &it, String &key, String &value) const
{
	it.it=Tree.rbegin();
	it.reset=false;
	while (1) {
		if (it.it==Tree.rend()) return false;
		if (it.it->second->isString()) break;
		++it.it;
	}
	key.set(it.it->first);
	value.set(it.it->second->toString());
	return true;
}

/*!\brief Vorhergehenden %String im %Array finden und Key und Value in Strings speichern
 *
 * \desc
 * Diese Funktion sucht den vorhergehenden %String im %Array und speichert dessen
 * Schlüssel im Parameter \p key und den Wert in \p value;
 *
 * @param[in,out] it Iterator. Dieser muss vom Typ ppl7::AssocArray::Iterator sein.
 * @param[out] key String, in dem der Name des Schlüssels gespeichert werden soll
 * @param[out] value String, in dem der Wert gespeichert werden soll.
 * \return Solange Elemente gefunden werden, liefert die Funktion \c true zurück, sonst \c false.
 */
bool AssocArray::getPrevious(ReverseIterator &it, String &key, String &value) const
{
	if (it.reset) return getLast(it,key,value);
	if (it.it==Tree.rend()) return false;
	while (1) {
		++it.it;
		if (it.it==Tree.rend()) return false;
		if (it.it->second->isString()) break;
	}
	key.set(it.it->first);
	value.set(it.it->second->toString());
	return true;
}



/*! \brief Wandelt ein Key-Value Template in ein Assoziatives Array um
 *
 * \desc
 * Diese Funktion wandelt einen Text mit Key-Value-Paaren in ein
 * Assoziatives Array um. Leere Zeilen oder Zeilen mit Raute (#)
 * am Anfang (Kommentarzeilen) werden ignoriert.
 *
 * \param[in] templ String mit den Key-Value-Paaren
 * \param[in] linedelimiter Das Zeichen, was als Zeilenende interpretiert werden soll. Default ist \c Newline
 * \param[in] splitchar Das Zeichen, was als Trennzeichen zwischen Schlüssel (Key) und Wert (Value)
 * interpretiert werden soll. Der Default ist das Gleichheitszeichen (=)
 * \param[in] concat Ist concat gesetzt und kommen im Text mehrere identische Schlüssel vor, werden die Werte
 * zu einem String zusammengeführt, wobei als Trennzeichen \c concat verwendet wird. Ist concat leer,
 * wird ein vorhandener Schlüssel überschrieben. Der Default ist, dass Werte mit gleichem Schlüssel mit
 * Newline aneinander gehangen werden.
 * \param[in] dotrim Ist \c dotrim=true, werden einzelnen Werte vor dem Einfügen ins Array mit der Funktion
 * Trim getrimmt, also Leerzeilen, Tabs und Zeilenumbrüche am Anfang und Ende gelöscht. Der Default
 * ist \c false.
 *
 * \return Die Funktion gibt die Anzahl gelesener Key-Value-Paare zurück, oder 0, wenn der Text
 * keine verwertbaren Zeilen enthielt.
 *
 * \note Falls das %Array vor dem Aufruf dieser Funktion bereits Datensätze enthielt, werden diese
 * nicht gelöscht. Die Funktion kann also benutzt werden, um Werte aus verschiedenen Templates in ein
 * einziges %Array einzulesen. Soll das %Array geleert werden, muß vorher die Funktion AssocArray::clear
 * aufgerufen werden.
 *
 * \see Um Konfigurationsdateien mit verschiedenen Abschnitten (z.B. .ini-Dateien) in ein
 * Assoziatives %Array einzulesen, gibt es die Member-Funktion
 * AssocArray::fromConfig
 *
 */
size_t AssocArray::fromTemplate(const String &templ, const String &linedelimiter, const String &splitchar, const String &concat, bool dotrim)
{
	String Row, Line;
	Array a;
	Array::Iterator it;
	String Key,Value;
	size_t rows=0;
	ssize_t p;
	size_t ssc=splitchar.size();
	a.explode(templ,linedelimiter);
	a.reset(it);
	while (1) {
		try {
			Line=a.getNext(it);
		} catch (OutOfBoundsEception &) {
			return rows;
		}
		Row=Trim(Line);
		if (Row.len()>0 && Row[0]!=L'#') { // Leere Zeilen und Kommentare ignorieren
			Row=Line;
			p=Row.instr(splitchar);
			if (p>0) {
				Key=Trim(Row.left(p));
				Value=Row.mid(p+ssc);
				if (dotrim) Value.trim();
				//printf ("Key=%ls\nValue=%ls\n",(const wchar_t *)Key, (const wchar_t *)Value);
				if (concat.notEmpty()) {
					append(Key, Value, concat);
				} else {
					set(Key,Value);
				}
				rows++;
			}
		}
	}
	return rows;
}


/*! \brief Wandelt eine Konfigurationsdatei in ein Assoziatives Array um
 *
 * \desc
 * Diese Funktion wandelt einen Konfigurations-Text mit mehreren Abschnitten
 * im Key-Value-Format in ein Assoziatives %Array um. Ein Abschnitt beginnt immer mit einem Keywort
 * in Eckigen klammern und enthält Key-Value-Paare. Zeilen mit Raute (#) am Anfang werden als
 * Kommentarzeilen interpretiert und ignoriert.
 * \par Beispiel einer Konfigurationsdatei
 * \code
[Abschnitt_1]
# Kommentarzeile, die überlesen wird
key1: value1
key2: value2
[Abschnitt_2]
key1: value1
key2: value2
\endcode
 *
 * \param[in] content Ein String, dre die zu parsende Konfiguration enthält.
 * \param[in] linedelimiter Das Zeichen, was als Zeilenende interpretiert werden soll. Default ist \c Newline
 * \param[in] splitchar Das Zeichen, was als Trennzeichen zwischen Schlüssel (Key) und Wert (Value)
 * interpretiert werden soll. Der Default ist das Gleichheitszeichen (=)
 * \param[in] concat Ist concat gesetzt und kommen im Text mehrere identische Schlüssel vor, werden die Werte
 * zu einem String zusammengeführt, wobei als Trennzeichen \c concat verwendet wird. Ist concat NULL,
 * wird ein vorhandener Schlüssel überschrieben. Der Default ist, dass gleiche Schlüssel mit Newline
 * aneinander gehangen werden.
 * \param[in] dotrim Ist \c dotrim=true, werden einzelnen Werte vor dem Einfügen ins Array mit der Funktion
 * Trim getrimmt, also Leerzeilen, Tabs und Zeilenumbrüche am Anfang und Ende gelöscht. Der Default
 * ist \c false.
 *
 * \return Die Funktion gibt die Anzahl gelesener Key-Value-Paare zurück, oder 0, wenn der Text
 * keine verwertbaren Zeilen enthielt.
 *
 * \note Falls das %Array vor dem Aufruf dieser Funktion bereits Datensätze enthielt, werden diese
 * nicht gelöscht. Die Funktion kann also benutzt werden, um Werte aus verschiedenen Templates in ein
 * einziges %Array einzulesen. Soll das %Array geleert werden, muß vorher die Funktion AssocArray::clear
 * aufgerufen werden.
 */
size_t AssocArray::fromConfig(const String &content, const String &linedelimiter, const String &splitchar, const String &concat, bool dotrim)
{
	String Row, Line, Section;
	Array a;
	Array::Iterator it;
	String Key,Value;
	size_t rows=0;
	ssize_t p;
	size_t ssc=splitchar.size();
	a.explode(content,linedelimiter);
	a.reset(it);
	while (1) {
		try {
			Line=a.getNext(it);
		} catch (OutOfBoundsEception &) {
			return rows;
		}
		Row=Trim(Line);
		if (Row.len()>0 && Row[0]!=L'#') { // Leere Zeilen und Kommentare ignorieren
			if (Row[0]==L'[' && Row[-1]==L']') {
				Section=Row.mid(1,Row.len()-2);
				Section.lowerCase();
				Section.trim();
			} else {
				Row=Line;
				p=Row.instr(splitchar);
				if (p>0) {
					Key=Section;
					if (Key.notEmpty()) Key+="/";
					Key+=Trim(Row.left(p));
					Value=Row.mid(p+ssc);
					if (dotrim) Value.trim();
					//printf ("Key=%ls\nValue=%ls\n",(const wchar_t *)Key, (const wchar_t *)Value);
					if (concat.notEmpty()) {
						append(Key, Value, concat);
					} else {
						set(Key,Value);
					}
					rows++;
				}
			}
		}
	}
	return rows;
}



/*!\brief Inhalt des Assoziativen Arrays in ein Template exportieren
 *
 * \desc
 * Mit dieser Funktion wird der textuelle Inhalt des Arrays als Template im Key-Value-Format in einem String
 * abgelegt.
 * Pointer oder Binäre Daten werden ignoriert.
 *
 * \param[out] s %String, in dem das Template gespeichert werden soll. Der %String wird von der Funktion nicht gelöscht,
 * der Inhalt des Arrays wird angehangen!
 * \param[in] prefix Optionaler Prefix, der jedem Key vorangestellt werden soll
 * \param[in] linedelimiter Optionaler Pointer auf einen String, der am Zeilenende ausgegeben werden soll. Der
 *            Default ist ein einzelnes Newline.
 * \param[in] splitchar Optionaler Pointer auf einen String, der als Trennzeichen zwischen Schlüssel und Wert
 *            verwendet werden soll. Der Default ist ein Gleichheitszeichen.
 * \par Beispiel
\code
#include <stdio.h>
#include <string.h>
#include <ppl7.h>

int main(int argc, char **argv)
{
	ppl7::AssocArray a;
	ppl7::ByteArray bin;
	ppl7::String out;
	bin.load("main.cpp");
	a.set("key1","Dieser Wert geht über\nmehrere Zeilen");
	a.set("array1/unterkey1","value2");
	a.set("array1/unterkey2","value3");
	a.set("array1/noch ein array/unterkey1","value4");
	a.set("array1/unterkey2","value5");
	a.set("key2","value6");
	a.set("dateien/main.cpp",&bin);
	a.set("array2/unterkey1","value7");
	a.set("array2/unterkey2","value8");
	a.set("array2/unterkey1","value9");
	a.toTemplate(&out,"foo");
	out.printnl();
}
\endcode
Ergebnis:
\code
foo/array1/noch ein array/unterkey1=value4
foo/array1/unterkey1=value2
foo/array1/unterkey2=value5
foo/array2/unterkey1=value9
foo/array2/unterkey2=value8
foo/key1=Dieser Wert geht über
foo/key1=mehrere Zeilen
foo/key2=value6
\endcode
	An diesem Beispiel sieht man, dass Pointer- und ByteArray-Werte nicht exportiert werden und Werte, die Zeilenumbrüche
	enthalten, werden auf mehrere Key-Value-Paare aufgesplittet. Die Importfunktion (AssocArray::fromTemplate,
	AssocArray::fromConfig) fügen diese wieder zu einer einzelnen Variable mit Zeilenumbruch
	zusammen.
 */
void AssocArray::toTemplate(String &s, const String &prefix, const String &linedelimiter, const String &splitchar) const
{
	String	key, pre, value, index;
	Array		Tok;
	if (prefix.notEmpty()) key=prefix+"/";
	ppl7::AssocArray::const_iterator it;
	for (it=Tree.begin();it!=Tree.end();++it) {
		Variant *p=it->second;
		if (p->isString()) {
			Tok.clear();
			Tok.explode(p->toString(),"\n");
			for (size_t i=0;i<Tok.size();i++) {
				s+=key+it->first+splitchar+Tok[i]+linedelimiter;
			}
		} else if (p->isAssocArray()) {
			pre.setf("%s%s",(const char*)key,(const char*)it->first);
			p->toAssocArray().toTemplate(s,pre,linedelimiter,splitchar);
		} else if (p->isArray()) {
			const Array &a=(const Array &)*p;
			for (size_t i=0;i<a.size();i++) {
				Tok.clear();
				Tok.explode(a[i],"\n");
				index.setf("%zu",i);
				for (size_t z=0;z<Tok.size();z++) {
					s+=key+it->first+"/"+index+splitchar+Tok[z]+linedelimiter;
				}
			}
		} else if (p->isDateTime()) {
			s+=key+it->first+splitchar+p->toDateTime().getISO8601withMsec()+linedelimiter;
		}
	}
}

/*!\brief Liefert Anzahl Bytes, die für exportBinary erforderlich sind
 *
 * \desc
 * Diese Funktion liefert die Anzahl Bytes zurück, die für den Buffer der Funktion AssocArray::exportBinary
 * erforderlich sind. Es kann dadurch ein ausreichend großer Puffer vor Aufruf der Funktion exportBinary
 * angelegt werden.
 *
 * \return Anzahl Bytes oder 0 im Fehlerfall
 *
 * \see
 * - AssocArray::exportBinary
 * - AssocArray::importBinary
 */
size_t AssocArray::binarySize() const
{
	size_t size;
	exportBinary(NULL,0, &size);
	return size;
}

/*!\brief Inhalt des Arrays in einem plattform-unabhängigen Binären-Format exportieren
 *
 * \desc
 * Mit dieser Funktion kann der komplette Inhalt des Arrays in einem plattform-unabhängigem binären Format abgelegt
 * werden, das sich zum Speichern in einer Datei oder zum Übertragen über das Internet eignet.
 *
 * \param[in] buffer Pointer auf einen ausreichend großen Puffer. Die Größe des benötigten Puffers
 *            kann zuvor mit der Funktion AssocArray::binarySize ermittelt werden. Wird als Buffer NULL
 *            übergeben, wird in der Variable \p realsize ebenfalls die Anzahl Bytes zurückgegeben
 * \param[in] buffersize Die Größe des Puffers in Bytes
 * \param[out] realsize In dieser Variable wird gespeichert, wieviele Bytes tatsächlich für den Export
 *            verwendet wurden
 * \exception ExportBufferToSmallException: Wird geworfen, wenn \p buffersize nicht groß genug ist, um
 * das Assoziative Array vollständig exportieren zu können.
 *
 * \attention
 * Es muss daran gedacht werden, dass nicht alle Datentypen exportiert werden können. Gegenwärtig
 * werden folgende Typen unterstützt:
 * - String (Wird als UTF-8 exportiert)
 * - Array
 * - AssocArray
 * - ByteArray
 * - ByteArrayPtr (wird in ein ByteArray umgewandelt!)
 * - DateTime
 * \see
 * - AssocArray::binarySize
 * - AssocArray::importBinary
 *
 * \note
 * Das exportierte Binary ist komptibel mit dem Assoziativen Array der PPL-Version 6
 */
void AssocArray::exportBinary(void *buffer, size_t buffersize, size_t *realsize) const
{
	char *ptr=(char*)buffer;
	if (realsize) *realsize=0;
	size_t p=0;
	size_t vallen=0;
	String key;
	String string;
	WideString widestring;
	ByteArray ba;
#ifdef HAVE_ICONV
	Iconv iconv(ICONV_UNICODE,"UTF-8");
#endif
	if (!buffer) buffersize=0;
	if (p+7<buffersize) strncpy(ptr,"PPLASOC",7);
	p+=7;
	AssocArray::const_iterator it;
	for (it=Tree.begin();it!=Tree.end();++it) {
		const Variant *a=it->second;
		if (p<buffersize) {
			if (a->isByteArrayPtr()) PokeN8(ptr+p,Variant::TYPE_BYTEARRAY);
			else PokeN8(ptr+p,a->type());
		}
		p++;
		key=it->first;
		size_t keylen=key.size();
		if (p+4<buffersize) PokeN16(ptr+p,(int)keylen);
		p+=2;
		if (p+keylen<buffersize) strncpy(ptr+p,(const char*)key,(int)keylen);
		p+=keylen;
		if (a->isString()) {
			string=a->toString();
			vallen=string.size();
			if (p+4<buffersize) PokeN32(ptr+p,(int)vallen);
			p+=4;
			if (p+vallen<buffersize) strncpy(ptr+p,(const char*)string,vallen);
			p+=vallen;
		} else if (a->isWideString()) {
#ifdef HAVE_ICONV
			widestring=a->toWideString();
			iconv.transcode(ByteArrayPtr(widestring.getPtr(),widestring.byteLength()),ba);
			vallen=ba.size();
			if (p+4<buffersize) PokeN32(ptr+p,(int)vallen);
			p+=4;
			if (p+vallen<buffersize) memcpy(ptr+p,ba.adr(),vallen);
			p+=vallen;
#else
			string.set(a->toWideString());
			vallen=string.size();
			if (p+4<buffersize) PokeN32(ptr+p,(int)vallen);
			p+=4;
			if (p+vallen<buffersize) strncpy(ptr+p,(const char*)string,vallen);
			p+=vallen;
#endif
		} else if (a->isAssocArray()) {
			size_t asize=0;
			if (!buffer) a->toAssocArray().exportBinary(NULL,0,&asize);
			else a->toAssocArray().exportBinary(ptr+p,buffersize-p,&asize);
			p+=asize;
		} else if (a->isArray()) {
			ppl7::Array aaa(a->toArray());
			if (p+4<buffersize) PokeN32(ptr+p,(int)aaa.size());
			p+=4;
			for (ssize_t i=0;i<(ssize_t)aaa.size();i++) {
				const String s=aaa.get(i);
				if (p+4<buffersize) PokeN32(ptr+p,(int)s.size());
				p+=4;
				vallen=s.size();
				if (p+vallen<buffersize) strncpy(ptr+p,(const char*)s,vallen);
				p+=vallen;
			}
		} else if (a->isDateTime()) {
			vallen=8;
			if (p+4<buffersize) PokeN32(ptr+p,(int)vallen);
			p+=4;
			if (p+vallen<buffersize) PokeN64(ptr+p,a->toDateTime().longInt());
			p+=vallen;
		} else if (a->isByteArray()==true || a->isByteArrayPtr()==true) {
			vallen=a->toByteArray().size();
			if (p+4<buffersize) PokeN32(ptr+p,(int)vallen);
			p+=4;
			if (p+vallen<buffersize) memcpy(ptr+p,a->toByteArrayPtr().adr(),vallen);
			p+=vallen;
		} else {
			vallen=0;
			if (p+4<buffersize) PokeN32(ptr+p,0);
			p+=4;
		}
	}
	if (p<buffersize) PokeN8(ptr+p,0);
	p++;
	if (realsize)*realsize=p;
	if (buffersize==0 || p<=buffersize) return;
	throw ExportBufferToSmallException("%zd < %zd",buffersize,p);
}

/*!\brief Inhalt des Arrays in einem plattform-unabhängigen Binären-Format exportieren
 *
 * \desc
 * Mit dieser Funktion kann der komplette Inhalt des Arrays in einem plattform-unabhängigem binären Format abgelegt
 * werden, das sich zum Speichern in einer Datei oder zum Übertragen über das Internet eignet.
 *
 * \param[in,out] buffer %ByteArray, in dem die exportierten Daten gespeichert werden sollen
 *
 * \attention
 * Es muss daran gedacht werden, dass nicht alle Datentypen exportiert werden können. Gegenwärtig
 * werden folgende Typen unterstützt:
 * - String (Wird als UTF-8 exportiert)
 * - Array
 * - AssocArray
 * - ByteArray
 * - ByteArrayPtr (wird in ein ByteArray umgewandelt!)
 * - DateTime
 * \see
 * - AssocArray::binarySize
 * - AssocArray::importBinary
 *
 * \note
 * Das exportierte Binary ist komptibel mit dem Assoziativen Array der PPL-Version 6
 */
void AssocArray::exportBinary(ByteArray &buffer) const
{
	buffer.free();
	size_t size;
	exportBinary(NULL,0, &size);
	buffer.malloc(size);
	exportBinary((void*)buffer.adr(),buffer.size(),NULL);
}


/*!\brief Daten aus einem vorherigen Export wieder importieren
 *
 * \desc
 * Mit dieser Funktion kann ein zuvor mit AssocArray::exportBinary exportiertes Assoziatives %Array wieder
 * importiert werden. Falls im %Array bereits Daten vorhanden sind, werden diese nicht gelöscht, können aber
 * überschrieben werden, wenn es im Export gleichnamige Schlüssel gibt.
 *
 * \param[in] bin Referenz auf ByteArray oder ByteArrayPtr mit den zu importierenden Daten
 *
 * \see
 * - CAssocArray::exportBinary
 * - CAssocArray::binarySize
 */
void AssocArray::importBinary(const ByteArrayPtr &bin)
{
	importBinary(bin.adr(),bin.size());
}

/*!\brief Daten aus einem vorherigen Export wieder importieren
 *
 * \desc
 * Mit dieser Funktion kann ein zuvor mit AssocArray::exportBinary exportiertes Assoziatives %Array wieder
 * importiert werden. Falls im %Array bereits Daten vorhanden sind, werden diese nicht gelöscht, können aber
 * überschrieben werden, wenn es im Export gleichnamige Schlüssel gibt.
 *
 * \param[in] buffer Pointer auf den Puffer, der die zu importierenden Daten enthält
 * \param[in] buffersize Größe des Puffers
 * \exception ImportFailedException
 *
 * \see
 * - AssocArray::exportBinary
 * - AssocArray::binarySize
 */
size_t AssocArray::importBinary(const void *buffer, size_t buffersize)
{
	if (!buffer) throw IllegalArgumentException();
	if (buffersize==0) throw IllegalArgumentException();
	const char *ptr=(const char*)buffer;
	size_t p=0;
	if (buffersize<8 || strncmp((const char*)ptr,"PPLASOC",7)!=0) {
		throw ImportFailedException("Not an AssocArray binary export");
	}
	p+=7;
	int type;
	size_t vallen,bytes;
	String key,str;
	DateTime dt;
	AssocArray na;
	ByteArray nb;
	WideString ws;
#ifdef HAVE_ICONV
	Iconv iconv("UTF-8",ICONV_UNICODE);
#endif
	while (p<buffersize && (type=PeekN8(ptr+p))!=0) {
		p++;
		size_t keylen=PeekN16(ptr+p);
		p+=2;
		key.set(ptr+p,keylen);
		p+=keylen;
		switch (type) {
			case Variant::TYPE_STRING:
				vallen=PeekN32(ptr+p);
				p+=4;
				set(key,(const char*)ptr+p,vallen);
				p+=vallen;
				break;
			case Variant::TYPE_WIDESTRING:
				vallen=PeekN32(ptr+p);
				p+=4;
#ifdef HAVE_ICONV
				iconv.transcode(ByteArrayPtr((const char*)ptr+p,vallen),nb);
				ws.set((const wchar_t *)nb.ptr(),nb.size());
#else
				ws.set((const wchar_t*)ptr+p,vallen);
#endif
				set(key,ws);
				p+=vallen;
				break;
			case Variant::TYPE_ASSOCARRAY:
				na.clear();
				bytes=na.importBinary(ptr+p,buffersize-p);
				p+=bytes;
				set(key,na);
				break;
			case Variant::TYPE_ARRAY:
			{
				size_t elements=PeekN32(ptr+p);
				p+=4;
				Array stringarray;
				stringarray.reserve(elements);
				for (size_t i=0;i<elements;i++) {
					str.set(ptr+p+4,PeekN32(ptr+p));
					p+=PeekN32(ptr+p)+4;
					stringarray.add(str);
				}
				set(key,stringarray);
			}
				break;
			case Variant::TYPE_BYTEARRAY:
				vallen=PeekN32(ptr+p);
				p+=4;
				nb.free();
				nb.copy(ptr+p,vallen);
				p+=vallen;
				set(key,nb);
				break;
			case Variant::TYPE_DATETIME:
				vallen=PeekN32(ptr+p);
				p+=4;
				dt.setLongInt(PeekN64(ptr+p));
				p+=vallen;
				set(key,dt);
				break;
			default:
				vallen=PeekN32(ptr+p);
				throw ImportFailedException("unknown datatype in AssocArray binary export [type=%d, size=%zu]",type,vallen);
		};
	}
	p++;
	return p;
}



/*!\brief Schlüssel auslesen
 *
 * \desc
 * Dieser Operator liefert den Wert des Schlüssels \p key als Variant zurück. Dieser kann
 * von der aufrufenden Anwendung in den jeweiligen Datentyp umgewandelt werden.
 *
 * @param key Name des Schlüssels
 * @return Referenz auf den einen Variant mit dem Wert des Schlüssels
 * \exception InvalidKeyException: Ungültiger Schlüssel
 * \exception KeyNotFoundException: Schlüssel wurde nicht gefunden
 */
const Variant &AssocArray::operator[](const String &key) const
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	return *node;
}

Variant &AssocArray::operator[](const String &key)
{
	Variant *node=findInternal(key);
	if (!node) throw KeyNotFoundException(key);
	return *node;
}


/*!\brief Assoziatives Array kopieren
 *
 * \desc
 * Mit diesem Operator wird der Inhalt das Assoziativen Arrays \p other übernommen.
 * Der bisherige Inhalt dieses Arrays geht verloren.
 *
 * @param other Zu kopierendes assoziatives Array
 * @return Referenz auf dieses Array
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 *
 */
AssocArray& AssocArray::operator=(const AssocArray& other)
{
	clear();
	add(other);
	return *this;
}

/*!\brief Assoziatives Array hinzufügen
 *
 * \desc
 * Mit diesem Operator wird der Inhalt das Assoziativen Arrays \p other dem eigenen
 * Array hinzugefügt. Das Array wird vorher nicht gelöscht, so dass vorhandene
 * Schlüssel erhalten bleiben. Gibt es in \p other jedoch gleichnamige Schlüssel,
 * werden die bisherigen Werte überschrieben.
 *
 * @param other Zu kopierendes assoziatives Array
 * @return Referenz auf dieses Array
 * \exception std::bad_alloc: Kein Speicher mehr frei
 * \exception OutOfMemoryException: Kein Speicher mehr frei
 * \exception InvalidKeyException: Ungültiger Schlüssel
 *
 */
AssocArray& AssocArray::operator+=(const AssocArray& other)
{
	add(other);
	return *this;
}

bool AssocArray::operator==(const AssocArray &other)
{
	ByteArray b1,b2;
	exportBinary(b1);
	other.exportBinary(b2);
	if (b1==b2) return true;
	return false;
}

bool AssocArray::operator!=(const AssocArray &other)
{
	if (*this == other) return false;
	return true;
}

AssocArray operator+(const AssocArray &a1, const AssocArray& a2)
{
	AssocArray a(a1);
	a.add(a2);
	return a;
}



} // EOF namespace ppl7

