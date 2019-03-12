/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: http://www.pfp.de/ppl/
 *
 *
 *******************************************************************************
 * Copyright (c) 2015, Patrick Fedick <patrick@pfp.de>
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
#ifdef HAVE_WIDEC_H
#include <widec.h>
#endif

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
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif
#ifdef HAVE_WCTYPE_H
#include <wctype.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include "ppl7.h"

namespace ppl7 {


/*!\class Variant
 * \ingroup PPLGroupDataTypes
 * \brief Flexibler Datentyp, der verschiedene Datentypen aufnehmen kann
 *
 * \desc
 * Dieser Datentyp kann andere Datentypen aufnehmen. Unterstützt werden:
 *
 * - String
 * - WideString
 * - Array
 * - AssocArray
 * - ByteArray
 * - ByteArrayPtr
 * - DateTime
 * - Pointer
 *
 * \example
 * \code
void Machwas(const Variant &object)
{
	Variant::DataType t=object.type();
	if (t==Variant::TYPE_BYTEARRAY) {
			const ppl7::ByteArray &bin= static_cast<const ppl7::ByteArray&>(object);  // Objekt zu ByteArray umwandeln
			printf ("Es ist ein ByteArray mit %i Bytes an Daten\n",bin.size());
			return;
	} else if (t==Variant::TYPE_STRING) {
			const String &str= static_cast<const String&>(object);  // Objekt zu String umwandeln
			printf ("Es ist ein String mit folgendem Inhalt: %s\n",(const char*)str);
			return;
	} else if (t==Variant::TYPE_ARRAY) {
			const Array &array= static_cast<const Array&>(object);  // Objekt zu Array umwandeln
			printf ("Es ist ein Array mit %i Elementen\n",array.count());
			return;
	}
	printf ("Datentyp wird nicht unterstützt\n");
}
\endcode
 *
 */

/*!\enum Variant::DataType
 * \brief Enumeration der verschiedenen Datenobjekte, die in der Library verwendet werden
 *
 * Enumeration der verschiedenen Datenobjekte, die in der Library verwendet werden
 */

/*!\var Variant::DataType Variant::TYPE_UNKNOWN
 * \brief Unbekannter Datentyp
 */

/*!\var Variant::DataType Variant::TYPE_STRING
 * \brief Datentyp ist String
 */

/*!\var Variant::DataType Variant::TYPE_WIDESTRING
 * \brief Datentyp ist WideString
 */

/*!\var Variant::DataType Variant::TYPE_BYTEARRAY
 * \brief Datentyp ist ByteArray
 */

/*!\var Variant::DataType Variant::TYPE_BYTEARRAYPTR
 * \brief Datentyp ist ByteArrayPtr
 */

/*!\var Variant::DataType Variant::TYPE_ASSOCARRAY
 * \brief Datentyp ist AssocArray
 */

/*!\var Variant::DataType Variant::TYPE_ARRAY
 * \brief Datentyp ist Array
 */

/*!\var Variant::DataType Variant::TYPE_DATETIME
 * \brief Datentyp ist DateTime
 */

/*!\var Variant::DataType Variant::TYPE_POINTER
 * \brief Datentyp ist Pointer
 */


/*!\var Variant::t
 * \brief Variable, zum Speichern des Datentyps
 */

/*!\var Variant::value
 * \brief Pointer auf den Inhalt des Datentyps
 */

/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Der Konstruktor initialisiert den Typ der Klasse mit Variant::TYPE_UNKNOWN.
 * Es ist aufgabe der abgeleiteten Klasse den korrekten Datentyp zu setzen.
 */
Variant::Variant()
{
	value=NULL;
	t=TYPE_UNKNOWN;
}

/*!\brief Destruktor
 *
 * \desc
 * Gibt den durch das Objekt belegten Speicher wieder frei.
 */
Variant::~Variant()
{
	clear();
}

/*!\brief Copy-Konstruktor der Klasse
 *
 * \desc
 * Der Inhalt des anderen Variant-Objekts \p value wird kopiert.
 */
Variant::Variant(const Variant &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp String
 *
 * Der Inhalt des Strings \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const String &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp WideString
 *
 * Der Inhalt des WideStrings \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const WideString &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp Array
 *
 * Der Inhalt des Arrays \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const Array &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp AssocArray
 *
 * Der Inhalt des AssocArrays \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const AssocArray &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp ByteArray
 *
 * Der Inhalt des ByteArrays \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const ByteArray &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp ByteArrayPtr
 *
 * Der Inhalt des ByteArrayPtrs \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const ByteArrayPtr &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp DateTime
 *
 * Der Inhalt des DateTimes \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const DateTime &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Konstruktor mit Datentyp Pointer
 *
 * Der Inhalt des Pointers \p value wird kopiert.
 *
 * @param value
 */
Variant::Variant(const Pointer &value)
{
	this->value=NULL;
	t=TYPE_UNKNOWN;
	set(value);
}

/*!\brief Inhalt des Objekts löschen
 *
 * \desc
 * Der im Objekte gespeicherte Datentyp wird gelöscht und sein Speicher
 * freigegeben.
 */
void Variant::clear()
{
	if (!value) return;
	switch (t) {
		case TYPE_STRING:
			delete (static_cast<String*>(value));
			break;
		case TYPE_ASSOCARRAY:
			delete (static_cast<AssocArray*>(value));
			break;
		case TYPE_BYTEARRAY:
			delete (static_cast<ByteArray*>(value));
			break;
		case TYPE_POINTER:
			delete (static_cast<Pointer*>(value));
			break;
		case TYPE_WIDESTRING:
			delete (static_cast<WideString*>(value));
			break;
		case TYPE_ARRAY:
			delete (static_cast<Array*>(value));
			break;
		case TYPE_DATETIME:
			delete (static_cast<DateTime*>(value));
			break;
		case TYPE_BYTEARRAYPTR:
			delete (static_cast<ByteArrayPtr*>(value));
			break;
		default:
			break;

	}
	value=NULL;
	t=TYPE_UNKNOWN;
}

/*!\brief Wert eines anderen Variant kopieren
 *
 * \desc
 * Der Wert des anderen Variant \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const Variant &value)
{
	clear();
	if (!value.value) return;
	switch (value.t) {
		case TYPE_STRING:
			this->value=new String(*static_cast<String*>(value.value));
			t=TYPE_STRING;
			break;
		case TYPE_ASSOCARRAY:
			this->value=new AssocArray(*static_cast<AssocArray*>(value.value));
			t=TYPE_ASSOCARRAY;
			break;
		case TYPE_BYTEARRAY:
			this->value=new ByteArray(*static_cast<ByteArray*>(value.value));
			t=TYPE_BYTEARRAY;
			break;
		case TYPE_POINTER:
			this->value=new Pointer(*static_cast<Pointer*>(value.value));
			t=TYPE_POINTER;
			break;
		case TYPE_WIDESTRING:
			this->value=new WideString(*static_cast<WideString*>(value.value));
			t=TYPE_WIDESTRING;
			break;
		case TYPE_ARRAY:
			this->value=new Array(*static_cast<Array*>(value.value));
			t=TYPE_ARRAY;
			break;
		case TYPE_DATETIME:
			this->value=new DateTime(*static_cast<DateTime*>(value.value));
			t=TYPE_DATETIME;
			break;
		case TYPE_BYTEARRAYPTR:
			this->value=new ByteArrayPtr(*static_cast<ByteArrayPtr*>(value.value));
			t=TYPE_BYTEARRAYPTR;
			break;
		default:
			break;
	}
}

/*!\brief Der Wert eines String wird übernommen.
 *
 * \desc
 * Der Wert des Strings \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const String &value)
{
	clear();
	this->value=new String(value);
	t=TYPE_STRING;
}

/*!\brief Der Wert eines WideString wird übernommen.
 *
 * \desc
 * Der Wert des WideStrings \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const WideString &value)
{
	clear();
	this->value=new WideString(value);
	t=TYPE_WIDESTRING;
}

/*!\brief Der Wert eines Array wird übernommen.
 *
 * \desc
 * Der Wert des Arrays \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const Array &value)
{
	clear();
	this->value=new Array(value);
	t=TYPE_ARRAY;
}

/*!\brief Der Wert eines AssocArray wird übernommen.
 *
 * \desc
 * Der Wert des AssocArrays \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const AssocArray &value)
{
	clear();
	this->value=new AssocArray(value);
	t=TYPE_ASSOCARRAY;
}

/*!\brief Der Wert eines ByteArray wird übernommen.
 *
 * \desc
 * Der Wert des ByteArrays \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const ByteArray &value)
{
	clear();
	this->value=new ByteArray(value);
	t=TYPE_BYTEARRAY;
}

/*!\brief Der Wert eines ByteArrayPtr wird übernommen.
 *
 * \desc
 * Der Wert des ByteArrayPtrs \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const ByteArrayPtr &value)
{
	clear();
	this->value=new ByteArrayPtr(value);
	t=TYPE_BYTEARRAYPTR;
}

/*!\brief Der Wert eines DateTime wird übernommen.
 *
 * \desc
 * Der Wert des DateTimes \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const DateTime &value)
{
	clear();
	this->value=new DateTime(value);
	t=TYPE_DATETIME;
}

/*!\brief Der Wert eines Pointer wird übernommen.
 *
 * \desc
 * Der Wert des Pointers \p value wird kopiert.
 *
 * \param value
 */
void Variant::set(const Pointer &value)
{
	clear();
	this->value=new Pointer(value);
	t=TYPE_POINTER;
}

/*!\brief Liefert den Datentyp des Objekts zurück
 *
 * \desc
 * Diese Funktion liefert den Datentyp des Objekts zurück.
 *
 * @return ID aus der Enumeration Variant::DataType
 */
Variant::DataType Variant::type() const
{
	return t;
}

/*!\brief Prüft auf einen bestimmten Datentyp
 *
 * \desc
 * Prüft, ob es sich bei diesem Objekt um den Datentyp \p type handelt.
 *
 * @param type Zu überprüfender Typ. Wert aus der Enumerationr Variant::DataType
 * @return Liefert \c true zurück, wenn es sich um den angegebenen Datentyp \p type handelt,
 * sonst \c false.
 */
bool Variant::isType(DataType type) const
{
	if (this->t==type) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp String handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp String handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp String handelt, sonst \c false.
 */
bool Variant::isString() const
{
	if (t==TYPE_STRING) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp WideString handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp WideString handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp WideString handelt, sonst \c false.
 */
bool Variant::isWideString() const
{
	if (t==TYPE_WIDESTRING) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp Array handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp Array handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp Array handelt, sonst \c false.
 */
bool Variant::isArray() const
{
	if (t==TYPE_ARRAY) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp AssocArray handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp AssocArray handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp AssocArray handelt, sonst \c false.
 */
bool Variant::isAssocArray() const
{
	if (t==TYPE_ASSOCARRAY) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp ByteArray handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp ByteArray handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp ByteArray handelt, sonst \c false.
 */
bool Variant::isByteArray() const
{
	if (t==TYPE_BYTEARRAY) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp ByteArrayPtr handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp ByteArrayPtr handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp ByteArrayPtr handelt, sonst \c false.
 */
bool Variant::isByteArrayPtr() const
{
	if (t==TYPE_BYTEARRAYPTR) return true;
	if (t==TYPE_BYTEARRAY) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp DateTime handelt
 *
 * \desc
 * Prüft, ob es sich um den Datentyp DateTime handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Datentyp DateTime handelt, sonst \c false.
 */
bool Variant::isDateTime() const
{
	if (t==TYPE_DATETIME) return true;
	return false;
}

/*!\brief Prüft, ob es sich um den Datentyp Pointer handelt
 *
 * \desc
 * Prüft, ob es sich um den Pointer DateTime handelt
 *
 * @return Liefert \c true zurück, wenn es such um den Pointer DateTime handelt, sonst \c false.
 */
bool Variant::isPointer() const
{
	if (t==TYPE_POINTER) return true;
	return false;
}

/*!\brief Typkonvertierung zu: const String
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf den gespeicherten
 * String zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf String
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen String handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const String& Variant::toString() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_STRING) throw TypeConversionException();
	return *static_cast<String *>(value);
}

/*!\brief Typkonvertierung zu: String
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf den gespeicherten
 * String zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf String
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen String handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
String& Variant::toString()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_STRING) throw TypeConversionException();
	return *static_cast<String *>(value);
}

/*!\brief Typkonvertierung zu: const WideString
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf den gespeicherten
 * WideString zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf WideString
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen WideString handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const WideString& Variant::toWideString() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_WIDESTRING) throw TypeConversionException();
	return *static_cast<WideString *>(value);
}

/*!\brief Typkonvertierung zu: WideString
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf den gespeicherten
 * WideString zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf WideString
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen WideString handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
WideString& Variant::toWideString()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_WIDESTRING) throw TypeConversionException();
	return *static_cast<WideString *>(value);
}

/*!\brief Typkonvertierung zu: const Array
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf das gespeicherten
 * Array zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf Array
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um ein Array handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const Array& Variant::toArray() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_ARRAY) throw TypeConversionException();
	return *static_cast<Array *>(value);
}

/*!\brief Typkonvertierung zu: Array
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf das gespeicherten
 * Array zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf Array
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um ein Array handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
Array& Variant::toArray()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_ARRAY) throw TypeConversionException();
	return *static_cast<Array *>(value);
}

/*!\brief Typkonvertierung zu: const AssocArray
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf das gespeicherten
 * AssocArray zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf AssocArray
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um ein AssocArray handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const AssocArray& Variant::toAssocArray() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_ASSOCARRAY) throw TypeConversionException();
	return *static_cast<AssocArray *>(value);
}

/*!\brief Typkonvertierung zu: AssocArray
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf das gespeicherten
 * AssocArray zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf AssocArray
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um ein AssocArray handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
AssocArray& Variant::toAssocArray()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_ASSOCARRAY) throw TypeConversionException();
	return *static_cast<AssocArray *>(value);
}

/*!\brief Typkonvertierung zu: const ByteArray
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf das gespeicherten
 * ByteArray zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf ByteArray
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um ein ByteArray handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const ByteArray& Variant::toByteArray() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_BYTEARRAY) throw TypeConversionException();
	return *static_cast<ByteArray *>(value);
}

/*!\brief Typkonvertierung zu: ByteArray
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf das gespeicherten
 * ByteArray zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf ByteArray
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um ein ByteArray handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
ByteArray& Variant::toByteArray()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_BYTEARRAY) throw TypeConversionException();
	return *static_cast<ByteArray *>(value);
}

/*!\brief Typkonvertierung zu: const ByteArrayPtr
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf den gespeicherten
 * ByteArrayPtr zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf ByteArrayPtr
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen ByteArrayPtr handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const ByteArrayPtr& Variant::toByteArrayPtr() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_BYTEARRAYPTR && t!=TYPE_BYTEARRAY) throw TypeConversionException();
	return *static_cast<ByteArrayPtr *>(value);
}

/*!\brief Typkonvertierung zu: ByteArrayPtr
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf den gespeicherten
 * ByteArrayPtr zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf ByteArrayPtr
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen ByteArrayPtr handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
ByteArrayPtr& Variant::toByteArrayPtr()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_BYTEARRAYPTR && t!=TYPE_BYTEARRAY) throw TypeConversionException();
	return *static_cast<ByteArrayPtr *>(value);
}

/*!\brief Typkonvertierung zu: const DateTime
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf den gespeicherten
 * DateTime zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf DateTime
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen DateTime handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const DateTime& Variant::toDateTime() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_DATETIME) throw TypeConversionException();
	return *static_cast<DateTime *>(value);
}

/*!\brief Typkonvertierung zu: DateTime
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf den gespeicherten
 * DateTime zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf DateTime
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen DateTime handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
DateTime& Variant::toDateTime()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_DATETIME) throw TypeConversionException();
	return *static_cast<DateTime *>(value);
}

/*!\brief Typkonvertierung zu: const Pointer
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine unveränderliche Referenz auf den gespeicherten
 * Pointer zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf Pointer
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen Pointer handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
const Pointer& Variant::toPointer() const
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_POINTER) throw TypeConversionException();
	return *static_cast<Pointer *>(value);
}

/*!\brief Typkonvertierung zu: Pointer
 *
 * \desc
 * Der Aufruf dieser Funktion liefert eine Referenz auf den gespeicherten
 * Pointer zurück, sofern der Variant diesen Datentyp enthält. Ist dies nicht der Fall,
 * wird eine Exception geworfen.
 *
 * @return Referenz auf Pointer
 * \exception TypeConversionException: Wird geworfen, wenn es sich nicht um einen Pointer handelt.
 * \exception EmptyDataException: Wird geworfen, wenn keine Daten in diesem Variant hinterlegt sind.
 */
Pointer& Variant::toPointer()
{
	if (!value) throw EmptyDataException();
	if (t!=TYPE_POINTER) throw TypeConversionException();
	return *static_cast<Pointer *>(value);
}


Variant::operator String() const
{
	return toString();
}

Variant::operator WideString() const
{
	return toWideString();
}

Variant::operator Array() const
{
	return toArray();
}

Variant::operator AssocArray() const
{
	return toAssocArray();
}

Variant::operator ByteArray() const
{
	return toByteArray();
}

Variant::operator ByteArrayPtr() const
{
	return toByteArrayPtr();
}

Variant::operator DateTime() const
{
	return toDateTime();
}

Variant::operator Pointer() const
{
	return toPointer();
}

Variant &Variant::operator=(const Variant &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const String &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const WideString &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const Array &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const AssocArray &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const ByteArray &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const ByteArrayPtr &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const DateTime &other)
{
	set(other);
	return *this;
}

Variant &Variant::operator=(const Pointer &other)
{
	set(other);
	return *this;
}

bool Variant::operator==(const Variant &other) const
{
	if (t!=other.t) return false;
	switch (t) {
		case TYPE_STRING:
			return (*static_cast<String*>(value) == *static_cast<String*>(other.value));
		case TYPE_ASSOCARRAY:
			return (*static_cast<AssocArray*>(value) == *static_cast<AssocArray*>(other.value));
		case TYPE_BYTEARRAY:
			return (*static_cast<ByteArray*>(value) == *static_cast<ByteArray*>(other.value));
		case TYPE_POINTER:
			return (*static_cast<Pointer*>(value) == *static_cast<Pointer*>(other.value));
		case TYPE_WIDESTRING:
			return (*static_cast<WideString*>(value) == *static_cast<WideString*>(other.value));
		case TYPE_ARRAY:
			return (*static_cast<Array*>(value) == *static_cast<Array*>(other.value));
		case TYPE_DATETIME:
			return (*static_cast<DateTime*>(value) == *static_cast<DateTime*>(other.value));
		case TYPE_BYTEARRAYPTR:
			return (*static_cast<ByteArrayPtr*>(value) == *static_cast<ByteArrayPtr*>(other.value));
		default:
			break;
	}
	return false;
}

bool Variant::operator!=(const Variant &other) const
{
	if (*this==other) return false;
	return true;
}



} // EOF namespace ppl7


