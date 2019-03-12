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
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif
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
#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#ifdef HAVE_PCRE
//#define PCRE_STATIC
#include <pcre.h>
#endif

namespace ppl7 {

static size_t InitialBuffersize=128;

static char*empty_string=(char*)"";

/*!\brief Zeichenkodierung festlegen
 *
 * Standardmäßig erwartet die String-Klasse bei Übergabe von "const char *", dass
 * die darin enthaltenen Strings \b UTF-8 kodiert sind. Dieses Verhalten kann man
 * mit dieser Funktion ändern.
 *
 * \param encoding
 *
 * \attention
 * Die Funktion ist nicht Thread-sicher und sollte daher nur einmal am Anfang des
 * Programms aufgerufen werden.
 */
void String::setGlobalEncoding(const char *encoding)
{
	if (!encoding) throw NullPointerException();
	char *ret=setlocale(LC_CTYPE,encoding);
	if (!ret) throw UnsupportedCharacterEncodingException();
}

const char *String::getGlobalEncoding()
{
	const char *ret=setlocale(LC_CTYPE,NULL);
	//printf ("%s\n",ret);
	return ret;
}

/*!\class String
 * \ingroup PPLGroupDataTypes
 * \ingroup PPLGroupStrings
 * \brief String-Klasse
 *
 * \desc
 * Diese Klasse kann verwendet werden, um beliebige Strings zu speichern und zu verarbeiten. Dabei
 * braucht sich der Anwender keine Gedanken um den verwendeten Speicher zu machen.
 * Die einzelnen Zeichen des Strings werden intern im Unicode-Format gespeichert. Bei Übernahme eines
 * C-Strings wird erwartet, dass dieser im UTF-8 Format vorliegt, mit der statischen Funktion
 * String::setGlobalEncoding kann jedoch auch eine andere Kodierung vorgegeben werden.
 *
 */


/*!\brief Konstruktor für leeren String
 *
 * \desc
 * Es wird ein leerer String erstellt.
 */
String::String() throw()
{
	ptr=empty_string;
	stringlen=0;
	s=0;
}

/*!\brief Konstruktor aus C-String
 *
 * \desc
 * Ein String wird aus einem C-String erstellt.
 *
 * @param str C-String mit 0-Byte am Ende
 * @exception OutOfMemoryException
 * @exception UnsupportedFeatureException
 * @exception UnsupportedCharacterEncodingException
 * @exception CharacterEncodingException
 */
String::String(const char *str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str);
}

/*!\brief Konstruktor aus Wide-Character-String mit bestimmer Länge
 *
 * \desc
 * Ein String wird aus dem Wide-Character-String \p str erstellt, von dem maximal
 * \p size Zeichen übernommen werden.
 *
 * @param str Wide-Character-String, der mit einem 0-Wert Endet
 * @param size Maximale Anzahl Zeichen, die übernommen werden sollen
 * @exception OutOfMemoryException
 */
String::String(const char *str, size_t size)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str,size);
}

/*!\brief Konstruktor aus String-Pointer
 *
 * \desc
 * Ein String wird aus einem anderen String erstellt.
 *
 * @param str Pointer auf einen anderen String
 * @exception OutOfMemoryException
 */
String::String(const String *str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str);
}

/*!\brief Konstruktor aus anderem String (Copy-Konstruktor)
 *
 * \desc
 * Ein String wird aus einem anderen String erstellt.
 *
 * @param str Referenz auf einen anderen String
 * @exception OutOfMemoryException
 */
String::String(const String &str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str);
}

/*!\brief Konstruktor aus Standard-Template String
 *
 * \desc
 * Ein String wird aus einem String der Standard-Template-Library (STL) erstellt.
 *
 * @param str Referenz auf String der STL
 * @exception OutOfMemoryException
 * @exception UnsupportedFeatureException
 * @exception UnsupportedCharacterEncodingException
 * @exception CharacterEncodingException
 */
String::String(const std::string &str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str.data(),str.size());
}

/*!\brief Konstruktor aus Standard-Template Wide-String
 *
 * \desc
 * Ein String wird aus einem Wide-String der Standard-Template-Library (STL) erstellt.
 *
 * @param str Referenz auf Wide-String der STL
 * @exception OutOfMemoryException
 */
String::String(const std::wstring &str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str.data(),str.size());
}

String::String(const WideString *str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str);
}

String::String(const WideString &str)
{
	ptr=empty_string;
	stringlen=0;
	s=0;
	set(str,str.size());
}

/*!\brief Destruktor
 *
 * \desc
 * Der Destructor gibt den durch den String belegten Speicher wieder frei.
 *
 */
String::~String() throw()
{
	if (ptr!=empty_string) free(ptr);
}

/*!\brief String leeren
 *
 * \desc
 * Mit dieser Funktion wird der String geleert und der bisher allokierte Speicher wieder
 * freigegeben.
 */
void String::clear() throw()
{
	if (ptr!=empty_string) free(ptr);
	ptr=empty_string;
	stringlen=0;
	s=0;
}

/*!\brief Anzahl Zeichen, die in den bereits allokierten Speicher passen
 *
 * \desc
 * Diese Funktion liefert die Anzahl Zeichen zurück, die in den derzeitig allokierten
 * Puffer passen, ohne dass neuer Speicher allokiert werden muss.
 *
 * @return Anzahl Zeichen
 */
size_t String::capacity ( ) const
{
	if (!s) return 0;
	return (s/sizeof(char))-1;
}

/*!\brief Reserviert Speicher für den String
 *
 * \desc
 * Mit dieser Funktion kann vor Verwendung des Strings vorgegeben werden, wieviel
 * Speicher initial reserviert werden soll. Dies ist insbesondere dann sinnvoll,
 * wenn der String während seiner Lebenszeit häufig verlängert wird.
 *
 * @param[in] size Anzahl Zeichen, für die Speicher reserviert werden soll.
 *
 * \note
 * Enthält der String bereits Zeichen, gehen diese nicht verloren, der existierende
 * Speicherbereich kann aber zwecks Vergrößerung umkopiert werden. Der Aufruf
 * der Funktion String::clear führt dazu, dass der Speicher wieder freigegeben wird.
 *
 */
void String::reserve(size_t size)
{
	size_t bytes=(size+1)*sizeof(char);
	if (s>=bytes) return; // Nothing to do
	char *p;
	if (ptr==empty_string) p=(char*)malloc(bytes);
	else p=(char*)realloc(ptr,bytes);
	if (!p) throw OutOfMemoryException();
	ptr=p;
	s=bytes;
}

/*!\brief Länge des Strings
 *
 * \desc
 * Diese Funktion gibt die Anzahl Zeichen zurück, aus denen der String besteht.
 *
 * \note
 * Die Funktionen String::len, String::length und String::size sind identisch.
 * \see
 * String::capacity
 *
 * @return Anzahl Zeichen
 */
size_t String::len() const
{
	return stringlen;
}

/*!\brief Länge des Strings
 *
 * \desc
 * Diese Funktion gibt die Anzahl Zeichen zurück, aus denen der String besteht.
 *
 * \note
 * Die Funktionen String::len, String::length und String::size sind identisch.
 * \see
 * String::capacity
 *
 * @return Anzahl Zeichen
 */
size_t String::length() const
{
	return stringlen;
}

/*!\brief Länge des Strings
 *
 * \desc
 * Diese Funktion gibt die Anzahl Zeichen zurück, aus denen der String besteht.
 *
 * \note
 * Die Funktionen String::len, String::length und String::size sind identisch.
 * \see
 * String::capacity
 *
 * @return Anzahl Zeichen
 */
size_t String::size() const
{
	return stringlen;
}


/*! \brief Prüft, ob der String leer ist.
 *
 * \desc
 * Diese Funktion prüft, ob der String leer ist.
 *
 * \returns Ist der String leer, liefert die Funktion \c true zurück, sonst \c false.
 * \see String::notEmpty
 */
bool String::isEmpty() const
{
	if (stringlen==0) return true;
	return false;
}

/*! \brief Prüft, ob der String Zeichen enthält
 *
 * \desc
 * Diese Funktion prüft, ob der String Zeichen enthält.
 *
 * \returns Enthält der String Zeichen, liefert die Funktion \c true zurück, sonst \c false.
 * \see String::isEmpty
 */
bool String::notEmpty() const
{
	if (stringlen==0) return false;
	return true;
}

/*!\brief Prüft, ob der String nummerisch ist
 *
 * \desc
 * Diese Funktion prüft, ob im String nur nummerische Zeichen vorhanden sind, also die Ziffern
 * 0-9, Punkt, Komma und Minus.
 *
 * @return Ist der String nummerisch, wird 1 zurückgegeben. Ist er es nicht oder ist der String
 * leer, wird 0 zurückgegeben.
 */
bool String::isNumeric() const
{
	if (!stringlen) return false ;
	size_t dotcount=0;
	for (size_t i=0;i<stringlen;i++) {
		int c=((char*)ptr)[i];
		if (c<'0' || c>'9') {
			if (c!='.' && c!=',' && c!='-') return false ;
			if (c=='-' && i>0) return false;
			if (c=='.' || c==',') {
				dotcount++;
				if (dotcount>1) return false;
			}
		}
	}
	return(true);
}

/*!\brief Prüft, ob der String einen Integer Wert enthält
 *
 * \desc
 * Diese Funktion prüft, ob im String einen integer Wert enthält, also nur die Ziffern
 * 0-9 und optional ein Minus am Anfang enthalten sind
 *
 * @return Ist der String ein Integer, wird 1 zurückgegeben. Ist er es nicht oder ist der String
 * leer, wird 0 zurückgegeben.
 */
bool String::isInteger() const
{
	if (!stringlen) return false;
	for (size_t i=0;i<stringlen;i++) {
		int c=((char*)ptr)[i];
		if (c<'0' || c>'9') {
			if (c=='-' && i==0) continue;		// Minus am Anfang ist erlaubt
			return false;
		}
	}
	return true;
}

/*!\brief Prüft, ob der String "wahr" ist
 *
 * Diese Funktion überprüft den aktuellen String, ob er "wahr" ist. Dies ist der Fall,
 * wenn eine der folgenden Bedingungen erfüllt ist:
 * - Der String enthält eine Ziffer ungleich 0
 * - Der String enthält das Wort "true" (Gross- oder Kleingeschrieben)
 * - Der String enthält das Wort "wahr" (Gross- oder Kleingeschrieben)
 * - Der String enthält das Wort "yes" (Gross- oder Kleingeschrieben)
 * - Der String enthält das Wort "ja" (Gross- oder Kleingeschrieben)
 *
 * \returns Liefert true (1) zurück, wenn der String "wahr" ist, sonst false (0). Ein Fehlercode wird nicht gesetzt
 * \see CWString::IsFalse()
 */
bool String::isTrue() const
{
	if (!stringlen) return false;
	if (atol(ptr)!=0) return true;
	if (strCaseCmp("true")==0) return true;
	if (strCaseCmp("wahr")==0) return true;
	if (strCaseCmp("ja")==0) return true;
	if (strCaseCmp("yes")==0) return true;
	if (strCaseCmp("t")==0) return true;
	return false;
}

/*!\brief Prüft, ob der String "unwahr" ist
 *
 * Diese Funktion überprüft den aktuellen String, ob er "unwahr" ist. Dies ist der Fall,
 * wenn eine der folgenden Bedingungen erfüllt ist:
 * - Der String zeigt auf NULL
 * - Die Länge des Strings ist 0
 * - Der String enthält die Ziffer 0
 * - Der String enthält nicht das Wort "true", "wahr", "yes" oder "ja" (Gross-/Kleinschreibung egal)
 *
 * \returns Liefert true (1) zurück, wenn der String "unwahr" ist, sonst false (0). Ein Fehlercode wird nicht gesetzt
 * \see CWString::IsTrue()
 */
bool String::isFalse() const
{
	if (isTrue()) return false;
	return true;
}



/*!\brief String anhand eines C-Strings setzen
 *
 * \desc
 * Mit dieser Funktion wird der String anhand eines char * gesetzt. Dabei wird er
 * intern nach Unicode konvertiert.
 *
 * \param str Pointer auf einen String
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 * \exception UnsupportedFeatureException
 * \exception UnsupportedCharacterEncodingException
 * \exception CharacterEncodingException
 *
 * \note
 * Multibyte-Characters zählen als ein Zeichen.
 *
 */
String & String::set(const char *str, size_t size)
{
	if (!str) {
		clear();
		return *this;
	}
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=::strlen(str);
	if (inbytes==0) {
		clear();
		return *this;
	}
	size_t outbytes=inbytes*sizeof(char)+1;
	if (outbytes>=s) {
		if (ptr!=empty_string) free(ptr);
		stringlen=0;
		s=InitialBuffersize;
		if (s<=outbytes) s=((outbytes/InitialBuffersize)+1)*InitialBuffersize+1;
		ptr=(char*)malloc(s);
		if (!ptr) {
			s=0;
			throw OutOfMemoryException();
		}
	}
#ifdef HAVE_STRNCPY_S
	strncpy_s((char*)ptr, s, str, inbytes);
#else
	strncpy((char*)ptr,str,inbytes);
#endif
	stringlen=inbytes;
	((char*)ptr)[stringlen]=0;
	return *this;
}

/*!\brief String anhand eines wchar_t* setzen
 *
 * \desc
 * Mit dieser Funktion wird der String anhand eines wchar_t * gesetzt.
 *
 * \param str Pointer auf einen String
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 */
String & String::set(const wchar_t *str, size_t size)
{
	if (str==NULL || size==0) {
		clear();
		return *this;
	}
	wchar_t *tmpbuffer=NULL;
	size_t inbytes;
	if (size!=(size_t)-1) {
		size_t buffersize = (size + 1) * sizeof(wchar_t);
		tmpbuffer=(wchar_t*)malloc(buffersize);
		if (!tmpbuffer) throw OutOfMemoryException();
#ifdef HAVE_WCSNCPY_S
		wcsncpy_s(tmpbuffer, buffersize, str, size);
		str = tmpbuffer;
#else
		str=wcsncpy(tmpbuffer,str,size);
#endif
		tmpbuffer[size]=0;
		inbytes=size;
	} else {
		inbytes=wcslen(str);
	}
	size_t outbytes=inbytes*sizeof(wchar_t)+4;
	if (outbytes>=s) {
		if (ptr!=empty_string) free(ptr);
		stringlen=0;
		s=InitialBuffersize;
		if (s<=outbytes) s=((outbytes/InitialBuffersize)+1)*InitialBuffersize+4;
		ptr=(char*)malloc(s);
		if (!ptr) {
			s=0;
			free(tmpbuffer);
			throw OutOfMemoryException();
		}
	}
#ifdef HAVE_WCSTOMBS_S
	// ptr=char* ziel
	// str=wchar_t *quelle
	// s=Size von ziel
	/*errno_t wcstombs_s(  
   size_t *pReturnValue,  
   char *mbstr,  
   size_t sizeInBytes,  
   const wchar_t *wcstr,  
   size_t count   
); 
	*/
	wcstombs_s(&stringlen, ptr, s, str, s);

#else
	stringlen=wcstombs(ptr, str, s);
#endif
	free(tmpbuffer);
	if (stringlen==(size_t)-1) {
		stringlen=0;
		throw CharacterEncodingException();
	}
	return *this;
}

/*!\brief String anhand eines String-Pointers setzen
 *
 * \desc
 * Mit dieser Funktion wird der String anhand des Pointers \p str eines anderen
 * Strings gesetzt.
 *
 * \param str Pointer auf einen String
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 */
String & String::set(const String *str, size_t size)
{
	if (!str) {
		clear();
		return *this;
	}
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=str->stringlen;
	if (inbytes>str->stringlen) inbytes=str->stringlen;
	return set(str->ptr,inbytes);
}

/*!\brief Wert eines anderen Strings übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des Strings \p str übernommen.
 *
 * \param str Referenz auf einen anderen String
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 */
String & String::set(const String &str, size_t size)
{
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=str.stringlen;
	if (inbytes>str.stringlen) inbytes=str.stringlen;
	return set(str.ptr,inbytes);
}

String & String::set(const WideString *str, size_t size)
{
	if (!str) {
		clear();
		return *this;
	}
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=str->size();
	if (inbytes>str->size()) inbytes=str->size();
	return set(str->getPtr(),inbytes);
}

String & String::set(const WideString &str, size_t size)
{
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=str.size();
	if (inbytes>str.size()) inbytes=str.size();
	return set(str.getPtr(),inbytes);
}

/*!\brief Wert eines Strings der STL übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des STL-Strings \p str übernommen.
 *
 * \param str Referenz auf einen String der Standard Template Library (STL)
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 */
String & String::set(const std::string &str, size_t size)
{
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=str.length();
	if (inbytes>str.length()) inbytes=str.length();
	return set((const char*)str.c_str(),inbytes);
}

/*!\brief Wert eines Wide-Strings der STL übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des STL-Wide-Strings \p str übernommen.
 *
 * \param str Referenz auf einen Wide-String der Standard Template Library (STL)
 * \param size Optionaler Parameter, der die Anzahl zu importierender Zeichen angibt.
 * Ist der Wert nicht angegeben, wird der komplette String übernommen. Ist der Wert größer als
 * der angegebene String, wird er ignoriert und der komplette String importiert.
 * \return Referenz auf den String
 * \exception OutOfMemoryException
 */
String & String::set(const std::wstring &str, size_t size)
{
	size_t inbytes;
	if (size!=(size_t)-1) inbytes=size;
	else inbytes=str.length();
	if (inbytes>str.length()) inbytes=str.length();
	return set(str.c_str(),inbytes);
}


/*!\brief Einzelnes Zeichen ersetzen
 *
 * \desc
 * Mit dieser Funktion wird ein einzelnes Zeichen eines Strings an der Position
 * \p position durch das Zeichen \p c ersetzt.
 *
 * @param position Position innerhalb des Strings (Zählung beginnt bei 0)
 * @param c Unicode-Wert, der gesetzt werden soll
 * @return Referenz auf den String
 * \throw OutOfBoundsEception: Wird geworfen, wenn \p position größer ist, als die
 * Länge des Strings
 */
String & String::set(size_t position, char c)
{
	if (position>=stringlen) throw OutOfBoundsEception();
	ptr[position]=c;
	return *this;
}

/*! \brief Erzeugt einen formatierten String
 *
 * \desc
 * Erzeugt einen String anhand des übergebenen Formatstrings \p fmt
 * und den optionalen Parametern \p ...
 *
 * \param fmt Der Formatstring
 * \param ... Optionale Parameter
 *
 * @return Referenz auf den String
 *
 * \par Example:
 * \code
int main(int argc, char **argv)
{
	String s;
	s.setf ("Anzahl Parameter: %i\n",argc);
	s.printnl();
	return 0;
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String & String::setf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char *buff=NULL;
#ifdef HAVE_VASPRINTF
	if (::vasprintf (&buff, (char*)fmt, args)>0 && buff!=NULL) {
#else
	if (compat::vasprintf (&buff, (char*)fmt, args)>0 && buff!=NULL) {
#endif
		try {
			set(buff);
			free(buff);
		} catch(...) {
			free(buff);
			va_end(args);
			throw;
		}
		return *this;
	}
	va_end(args);
    free(buff);
    throw Exception("String::setf");
}

/*!\brief Einzelnes ASCII-Zeichen übernehmen
 *
 * \desc
 * Ein einzelnes ASCII-Zeichen \p c wird in den String übernommen.
 *
 * @param c ASCII-Wert des gewünschten Zeichens
 *
 * @return Referenz auf den String
 */
String & String::set(char c)
{
	char buffer[2];
	buffer[0]=c;
	buffer[1]=0;
	return set(buffer,1);
}

/*! \brief Erzeugt einen formatierten String
 *
 * Erzeugt einen String anhand des übergebenen Formatstrings \p fmt
 * und den optionalen Parametern in \p args.
 *
 * \param fmt Der Formatstring
 * \param args Pointer auf Liste der Parameter. Muss zuvor mit va_start initialisiert worden sein.
 * @return Referenz auf den String
 * \par Example:
 * \code
void MyFunction(const char *fmt, ...)
{
	String s;
	va_list args;
	va_start(args, fmt);
	s.vasprintf(fmt,args);
	va_end(args);
	printf ("Ergebnis: %s",(const char*)s);
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String & String::vasprintf(const char *fmt, va_list args)
{
	char *buff=NULL;
#ifdef HAVE_VASPRINTF
	if (::vasprintf (&buff, (char*)fmt, args)>0 && buff!=NULL) {
#else
	if (compat::vasprintf (&buff, (char*)fmt, args)>0 && buff!=NULL) {
#endif
		try {
			set(buff);
			free(buff);
		} catch(...) {
			free(buff);
			throw;
		}
		return *this;
	}
    free(buff);
    throw Exception();
}


/*!\brief String-Speicher übernehmen
 *
 * \desc
 * Mit dieser Funktion wird der Klasse die Verwaltung des Speicherbereich mit der Adresse \p adr und der
 * Größe \p size übergeben. Der Speicher muss zuvor mit "malloc" bzw. "calloc" allokiert worden sein
 * und darf von der Anwendung selbst nicht mehr freigegeben werden.
 *
 * @param[in] adr Startadresse des Speicherbereichs
 * @param[in] size Größe des Speicherbereichs in Bytes
 * @param[in] stringlen Optionaler Parameter, der die Länge des Strings innerhalb des übergebenen
 * Speicherbereichs angibt. Darf maximal \b size-1 groß sein. Ist der Wert nicht angegeben, wird die
 * Länge des Strings mit \b strlen berechnet
 *
 * \note Der String muss mit einem Null-Byte enden. Um dies sicherzustellen überschreibt die Methode
 * das letzte Byte des übergebenen Speicherbereichs mit 0.
 *
 * \return Referenz auf den String
 *
 */
String & String::useadr(void *adr, size_t size, size_t stringlen)
{
	if (adr==NULL || size==0) throw IllegalArgumentException("adr and size must not be 0");
	if (ptr!=empty_string) free(ptr);
	ptr=(char*)adr;
	s=size;
	ptr[s-1]=0;
	this->stringlen=stringlen;
	if (stringlen==(size_t)-1) stringlen=strlen(ptr);
	if (stringlen>=size) stringlen=size-1;
	return *this;
}

/*!\brief Fügt einen Wide-Character String an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen Wide-Character String an das Ende des bestehenden an
 *
 * \param[in] str Pointer auf einen Wide-Character String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::append(const wchar_t *str, size_t size)
{
	String a;
	a.set(str,size);
	return append((const char*)a,a.size());
}

/*!\brief Fügt einen C-String an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen C-String an das Ende des bestehenden an. Der String muss entweder
 * UTF-8 kodiert sein, oder es muss mit der statischen Funktion String::setGlobalEncoding
 * zuvor eine andere Kodierung gesetzt worden sein.
 *
 * \param[in] str Pointer auf einen Wide-Character String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 * \exception UnsupportedFeatureException
 * \exception UnsupportedCharacterEncodingException
 * \exception CharacterEncodingException
 *
 */
String & String::append(const char *str, size_t size)
{
	if (str==NULL || size==0) return *this;
	if (ptr==empty_string) {
		return set(str,size);
	}
	size_t inchars;
	if (size!=(size_t)-1) {
		inchars=size;
		if (inchars>strlen(str)) inchars=strlen(str);
	}
	else inchars=strlen(str);
	size_t outbytes=(inchars+stringlen)*sizeof(char)+1;
	if (outbytes>=s) {
		size_t newbuffersize=((outbytes/InitialBuffersize)+1)*InitialBuffersize+16;
		char *t=(char*)realloc(ptr,newbuffersize);
		if (!t) throw OutOfMemoryException();
		ptr=t;
		s=newbuffersize;
	}
	memcpy(((char*)ptr)+stringlen,str,inchars);
	stringlen+=inchars;
	ptr[stringlen]=0;
	return *this;
}

/*!\brief Fügt einen als Pointer übergebenen String an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen als Pointer übergebenen String an das Ende des bestehenden an. Ist der Pointer
 * NULL oder der Inhalt des Strings leer, wird der bisherige String beibehalten, es erfolgt
 * keine Exception.
 *
 * \param[in] str Pointer auf ein String-Objekt
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::append(const String *str, size_t size)
{
	if (!str) return *this;
	return append(str->ptr,size);
}

/*!\brief Fügt einen String an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen String an das Ende des bestehenden an.
 *
 * \param[in] str Referenz auf ein String-Objekt
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::append(const String &str, size_t size)
{
	return append(str.ptr,size);
}

/*!\brief Fügt einen std::string an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen std::string an das Ende des bestehenden an.
 *
 * \param[in] str Referenz auf ein String-Objekt der STL
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::append(const std::string &str, size_t size)
{
	if (size==(size_t)-1) return append(str.data(),str.size());
	return append(str.data(),size);
}

/*!\brief Fügt einen std::wstring an das Ende des bestehenden an
 *
 * \desc
 * Fügt einen std::wstring an das Ende des bestehenden an.
 *
 * \param[in] str Referenz auf ein Wide-String-Objekt der STL
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::append(const std::wstring &str, size_t size)
{
	if (size==(size_t)-1) return append(str.data(),str.size());
	return append(str.data(),size);
}


/*!\brief Fügt einen Formatierten String an das Ende des bestehenden an
 *
 * Anhand des übergebenen Formatstrings \p fmt und den optionalen Parametern \p ...
 * wird ein neuer String gebildet, der an das Ende des bestehenden angehangen wird
 *
 * \param fmt Der Formatstring
 * \param ... Optionale Parameter
  * @return Referenz auf den String
 *
 * \par Example:
 * \code
int main(int argc, char **argv)
{
	String s;
	s="Hallo Welt!";
	s.appendf (" Es wurden %i Parameter übergeben\n",argc);
	s.printnl();
	return 0;
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String & String::appendf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char *buff=NULL;
#ifdef HAVE_VASPRINTF
	if (::vasprintf (&buff, (const char*)fmt, args)>0 && buff!=NULL) {
#else
	if (compat::vasprintf (&buff, (const char*)fmt, args)>0 && buff!=NULL) {
#endif
		try {
			String a;
			a.set(buff);
			free(buff);
			append(a.ptr,a.stringlen);
		} catch(...) {
			free(buff);
			va_end(args);
			throw;
		}
		return *this;
	}
	va_end(args);
    free(buff);
    throw Exception();
}

/*!\brief Einzelnes ASCII-Zeichen anhängen
 *
 * \desc
 * Ein einzelnes ASCII-Zeichen \p c wird in an den String angehangen.
 *
 * @param c ASCII-Wert des gewünschten Zeichens
 *
 * @return Referenz auf den String
 */
String & String::append(char c)
{
	char buffer[2];
	buffer[0]=c;
	buffer[1]=0;
	return append(buffer,1);
}


/*!\brief Fügt einen Wide-Character String am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen Wide-Character String am Anfang des bestehenden Strings ein
 *
 * \param[in] str Pointer auf einen Wide-Character String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::prepend(const wchar_t *str, size_t size)
{
	String a;
	a.set(str,size);
	return prepend((const char*)a.ptr,size);
}

/*!\brief Fügt einen String am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen String am Anfang des bestehenden Strings ein
 *
 * \param[in] str Pointer auf einen String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::prepend(const String *str, size_t size)
{
	if (!str) return *this;
	if (ptr==empty_string) {
		set(str,size);
		return *this;
	}
	String a;
	a.set(str,size);
	return prepend(a.ptr,a.stringlen);
}

/*!\brief Fügt einen String am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen String am Anfang des bestehenden Strings ein
 *
 * \param[in] str Referenz auf einen String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::prepend(const String &str, size_t size)
{
	if (ptr==empty_string) {
		set(str,size);
		return *this;
	}
	String a;
	a.set(str,size);
	return prepend(a.ptr,a.stringlen);
}

/*!\brief Fügt einen std::string der STL am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen std::string der Standard Template Library am Anfang des bestehenden Strings ein
 *
 * \param[in] str Referenz auf einen std::string
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::prepend(const std::string &str, size_t size)
{
	if (ptr==empty_string) {
		set(str,size);
		return *this;
	}
	String a;
	a.set(str,size);
	return prepend(a.ptr,a.stringlen);
}

/*!\brief Fügt einen std::wstring der STL am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen std::wstring der Standard Template Library am Anfang des bestehenden Strings ein
 *
 * \param[in] str Referenz auf einen std::wstring
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::prepend(const std::wstring &str, size_t size)
{
	if (ptr==empty_string) {
		set(str,size);
		return *this;
	}
	String a;
	a.set(str,size);
	return prepend(a.ptr,a.stringlen);
}

/*!\brief Fügt einen C-String am Anfang des bestehenden Strings ein
 *
 * \desc
 * Fügt einen C-String am Anfang des bestehenden Strings ein
 *
 * \param[in] str Pointer auf einen C-String
 * \param[in] size Optional die Anzahl Zeichen (nicht Bytes) im String, die kopiert werden sollen.
 *
 * \return Referenz auf den String
 *
 * \exception OutOfMemoryException
 */
String & String::prepend(const char *str, size_t size)
{
	if (str==NULL || size==0) return *this;
	if (ptr==empty_string) {
		set(str,size);
		return *this;
	}
	size_t inchars;
	if (size!=(size_t)-1) {
		inchars=size;
		if (inchars>strlen(str)) inchars=strlen(str);
	}
	else inchars=strlen(str);
	size_t outbytes=(inchars+stringlen)*sizeof(char)+1;
	if (outbytes>=s) {
		size_t newbuffersize=((outbytes/InitialBuffersize)+1)*InitialBuffersize+16;
		char *t=(char*)realloc(ptr,newbuffersize);
		if (!t) throw OutOfMemoryException();
		ptr=t;
		s=newbuffersize;
	}
	// Bestehenden Speicherblock nach hinten moven
	memmove(((char*)ptr)+inchars,ptr,stringlen);
	// Neuen Speicherblock davor kopieren
	memcpy(ptr,str,inchars);
	stringlen+=inchars;
	ptr[stringlen]=0;
	return *this;
}

/*!\brief Fügt einen Formatierten String am Anfang bestehenden ein
 *
 * Anhand des übergebenen Formatstrings \p fmt und den optionalen Parametern \p ...
 * wird ein neuer String gebildet, der am Anfang des bestehenden eingehangen wird.
 *
 * \param fmt Der Formatstring
 * \param ... Optionale Parameter
 * \par Example:
 * \code
int main(int argc, char **argv)
{
	String s;
	s="Vielen Dank!\n";
	s.prependf ("Es wurden %i Parameter übergeben. ",argc);
	s.printnl();
	return 0;
}
\endcode
 *
 * \copydoc sprintf.dox
 */
String & String::prependf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char *buff=NULL;
#ifdef HAVE_VASPRINTF
	if (::vasprintf (&buff, (const char*)fmt, args)>0 && buff!=NULL) {
#else
	if (compat::vasprintf (&buff, (const char*)fmt, args)>0 && buff!=NULL) {
#endif
		try {
			String a;
			a.set(buff);
			free(buff);
			prepend(a.ptr,a.stringlen);
		} catch(...) {
			free(buff);
			va_end(args);
			throw;
		}
		return *this;
	}
	va_end(args);
    free(buff);
    throw Exception();
}

/*!\brief Einzelnes ASCII-Zeichen am Anfang einfügen
 *
 * \desc
 * Ein einzelnes ASCII-Zeichen \p c wird in am Anfang des Strings eingefügt.
 * Die nachfolgenden Zeichen des Strings verschieben sich nach rechts.
 *
 * @param c ASCII-Wert des gewünschten Zeichens
 *
 * @return Referenz auf den String
 */
String & String::prepend(char c)
{
	char buffer[2];
	buffer[0]=c;
	buffer[1]=0;
	return prepend(buffer,1);
}

/*!\brief String in eine beliebige lokale Kodierung umwandeln
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des Strings in eine beliebige lokale
 * Kodierung umgewandelt und als ByteArray zurückgegeben.
 *
 * \param[in] encoding Das gewünschte Encoding
 *
 * @return ByteArray mit der UTF8-Repräsentation des Strings.
 *
 * \attention
 * Für diese Funktion wird "Iconv" benötigt. Ist keine Iconv-Bibliothek auf dem
 * System vorhanden, wird eine UnsupportedFeatureException geworfen.
 */
ByteArray String::toEncoding(const char *encoding) const
{
#ifndef HAVE_ICONV
	throw UnsupportedFeatureException();
#else
	iconv_t iconv_handle=iconv_open(encoding,"");
	if ((iconv_t)(-1)==iconv_handle) {
		throw UnsupportedCharacterEncodingException();
	}

	size_t buffersize=(stringlen+4)*sizeof(wchar_t);
	char *buffer=(char*)malloc(buffersize);
	if (!buffer) {
		iconv_close(iconv_handle);
		throw OutOfMemoryException();
	}
	size_t outbytes=buffersize;
	char *b=buffer;
	char *inbuffer=(char*)ptr;
	size_t inbytes=stringlen;

	size_t res=iconv((iconv_t)iconv_handle, (ICONV_CONST char **)&inbuffer, &inbytes,
			(char**)&b, &outbytes);
	iconv_close(iconv_handle);
	if (res==(size_t)(-1)) {
		free(buffer);
		throw CharacterEncodingException();
	}
	//b[0]=0;
	//HexDump(buffer,buffersize-outbytes+4);
	ByteArray ret(buffer,buffersize-outbytes);
	//ret.hexDump();
	free(buffer);
	return ret;
#endif
}

/*!/brief Diese Funktion liefert den String immer UTF-8 kodiert zurück
 *
 * \desc
 * Diese Funktion liefert den String immer UTF-8 kodiert zurück, unabhängig davon, welche
 * Kodierung das System verwendet.
 * @return
 */
ByteArray String::toUtf8() const
{
	const char * l=setlocale(LC_CTYPE,NULL);
	//printf ("Locale: %s\n",l);
	// de_DE.UTF-8
	if (strcasestr(l,"utf-8")) {
		return ByteArray(ptr,stringlen);
	}
	return toEncoding("UTF-8");
}

ByteArray String::toUCS4() const
{
	ByteArray ret;
	if (stringlen) {
		ppluint32 *ucs4=(ppluint32*)malloc(stringlen*4+4);
		if (!ucs4) throw OutOfMemoryException();
		for (size_t i=0;i<stringlen;i++) ucs4[i]=(ppluint32)ptr[i];
		ucs4[stringlen]=0;
		ret.useadr(ucs4,stringlen*4);
	}
	return ret;
}

String &String::fromUCS4(const ppluint32 *str, size_t size)
{
	clear();
	/*TODO: Muss noch implementiert werden
	 *
	 */
	throw UnsupportedFeatureException("String::fromUCS not implemented yet");
	/*
	for (size_t i=0;str[i]!=0;i++) {
		if (size!=(size_t)-1 && i>=size) break;
		wchar_t c=(wchar_t)str[i];
		append(c);
	}
	*/
	return *this ;
}

String &String::fromUCS4(const ByteArrayPtr &bin)
{
	return fromUCS4((ppluint32*)bin.ptr(),bin.size());
}


/*!\brief Einzelnes Zeichen auslesen
 *
 * \desc
 * Mit dieser Funktion kann Bytewert eines einzelnen Zeichens an der Position
 * \p pos ausgelesen werden. Enthält \p pos einen positiven Wert, wird die Position des
 * Zeichens vom Anfang des Strings ermittelt, wobei 0 dem ersten Zeichen entspricht.
 * Ist der Wert negativ, wird das Zeichen vom Ende des Strings ermittelt, wobei -1
 * dem letzten Zeichen des Strings entspricht.
 *
 * @param pos Position des Zeichens innerhalb des Strings
 * @return Bytewert des Zeichens
 * \exception OutOfBoundsEception Wird geworfen, wenn die angegebene Position \p pos
 * ausserhalb des Strings liegt oder der String leer ist.
 */
char String::get(ssize_t pos) const
{
	if (pos>=0 && stringlen>(size_t)pos) return ((char*)ptr)[pos];
	if (pos<0 && (size_t)(0-pos)<stringlen) return ((char*)ptr)[stringlen+pos];
	throw OutOfBoundsEception();
}


/*!\brief Einzelnes Zeichen auslesen
 *
 * \desc
 * Mit diesem Operator kann der Bytewert eines einzelnen Zeichens an der Position
 * \p pos ausgelesen werden. Enthält \p pos einen positiven Wert, wird die Position des
 * Zeichens vom Anfang des Strings ermittelt, wobei 0 dem ersten Zeichen entspricht.
 * Ist der Wert negativ, wird das Zeichen vom Ende des Strings ermittelt, wobei -1
 * dem letzten Zeichen des Strings entspricht.
 *
 * @param pos Position des Zeichens innerhalb des Strings
 * @return Bytewert des Zeichens
 * \exception OutOfBoundsEception Wird geworfen, wenn die angegebene Position \p pos
 * ausserhalb des Strings liegt oder der String leer ist.
 */
char String::operator[](ssize_t pos) const
{
	if (pos>=0 && stringlen>(size_t)pos) return ((char*)ptr)[pos];
	if (pos<0 && (size_t)(0-pos)<=stringlen) return ((char*)ptr)[stringlen+pos];
	throw OutOfBoundsEception();
}


/*!\brief String auf STDOUT ausgeben
 *
 * Diese Funktion gibt den aktuellen String auf STDOUT aus. Dazu ist es notwendig den String vom internen
 * Unicode-Format in das Encoding des Betriebssystems umzurechnen. Da dieses von den lokalen Einstellungen
 * des Betriebssystems und des Users abhängig ist, wird die Environment-Variable "LANG" ausgewertet.
 * Ist diese nicht gesetzt oder enthält ein unbekanntes Encoding, wird der String immer in UTF-8 ausgegeben.
 *
 * \param withNewline Ein optionaler Parameter, der angibt, ob nach der Ausgabe ein Zeilenumbruch
 * angehangen werden soll (default=false)
 * \par Exceptions:
 * Keine
 *
 */
void String::print(bool withNewline) const throw()
{
	if (stringlen>0) {
		if (withNewline) printf("%s\n",(char*)ptr);
		else printf("%s",(char*)ptr);
	} else if (withNewline) {
		printf("\n");
	}
}

/*!\brief String auf STDOUT mit abschließendem Zeilenumbruch ausgeben
 *
 * Diese Funktion gibt den aktuellen String mit abschließendem Zeilenumbruch auf STDOUT aus.
 * Dazu ist es notwendig den String vom internen
 * Unicode-Format in das Encoding des Betriebssystems umzurechnen. Da dieses von den lokalen Einstellungen
 * des Betriebssystems und des Users abhängig ist, wird die Environment-Variable "LANG" ausgewertet.
 * Ist diese nicht gesetzt oder enthält ein unbekanntes Encoding, wird der String immer in UTF-8 ausgegeben.
 *
 * \par Exceptions:
 * Keine
 *
 */
void String::printnl() const throw()
{
	print(true);
}

/*!\brief Hexdump des Strings ausgeben
 *
 * \desc
 * Mit dieser zu Debug-Zwecken gedachten Funktion wird der Inhalt des
 * Strings als HexDump auf der Konsole ausgegeben.
 */
void String::hexDump() const
{
	PrintDebug ("HEXDUMP of String %p: %zi Bytes starting at Address %p:\n",this,stringlen*sizeof(char),ptr);
	if (stringlen) HexDump(ptr,stringlen*sizeof(char),true);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const char* str)
{
	return set(str);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const wchar_t* str)
{
	return set(str);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const String *str)
{
	return set(str);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const String &str)
{
	return set(str);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const std::string &str)
{
	return set(str);
}

/*!\brief String übernehmen
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str kopiert. Der Operator
 * ist identisch mit der Funktion String::set
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(const std::wstring &str)
{
	return set(str);
}

/*!\brief Zeichen übernehmen
 *
 * \desc
 * Mit diesem Operator wird ein einzelnes Zeichen in den String kopiert.
 *
 * @param[in] c Unicode Wert des zu übernehmenden Zeichens
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator=(char c)
{
	return set(c);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const char* str)
{
	return append(str);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const wchar_t* str)
{
	return append(str);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const String& str)
{
	return append(str);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const std::string &str)
{
	return append(str);
}

/*!\brief String addieren
 *
 * \desc
 * Mit diesem Operator wird der Angegebene String \p str an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] str Zu kopierender String
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(const std::wstring &str)
{
	return append(str);
}

/*!\brief Zeichen anhängen
 *
 * \desc
 * Mit diesem Operator wird das angegebene Zeichen \p c an den bisher vorhandenen
 * String angehangen. Der Operator ist identisch mit der Funktion String::append.
 *
 * @param[in] c Unicode-Wert des anzuhängenden Zeichens
 * @return Referenz auf diese Instanz der Klasse
 */
String& String::operator+=(char c)
{
	return append(c);
}

/*!\brief Führt einen Vergleich mit einem anderen String durch
 *
 * \desc
 * Führt einen Vergleich mit einem anderen String durch.
 *
 * \param str String, mit dem verglichen werden soll
 * \param size Optionaler Parameter, der die Anzahl zu berücksichtigender Zeichen innerhalb des
 * Strings \p str angibt. Wird er nicht angegeben, wird ein vergleich mit dem kompletten String
 * \p str durchgeführt.
 *
 * \return Ist der String innerhalb dieses Objekts kleiner als der mit \a str angegebene, wird ein
 * negativer Wert zurückgegeben, ist er größer, erfolgt ein positiver Return-Wert,
 * sind beide identisch, wird 0 zurückgegeben.
 *
 * \see strCaseCmp Vergleich zweier Strings unter Ignorierung der Gross-/Kleinschreibung
 */
int String::strcmp(const String &str, size_t size) const
{
	if (size!=(size_t)-1) return ::strncmp(ptr,str.ptr,size);
	return ::strcmp(ptr,str.ptr);
}

int String::strcmp(const char *str, size_t size) const
{
	if (size!=(size_t)-1) return ::strncmp(ptr,str,size);
	return ::strcmp(ptr,str);
}


/*!\brief Stringvergleich mit Ignorierung von Gross-/Kleinschreibung
 *
 * \desc
 * Führt einen Vergleich mit einem anderen String durch, unter Ignorierung der
 * Gross-/Kleinschreibung.
 *
 * \param str String, mit dem verglichen werden soll
 * \param size Optionaler Parameter, der die Anzahl zu berücksichtigender Zeichen innerhalb des
 * Strings \p str angibt. Wird er nicht angegeben, wird ein vergleich mit dem kompletten String
 * \p str durchgeführt.
 *
 *
 * \return Ist der String innerhalb dieses Objekts kleiner als der mit \a str angegebene, wird ein
 * negativer Wert zurückgegeben, ist er größer, erfolgt ein positiver Return-Wert,
 * sind beide identisch, wird 0 zurückgegeben.
 *
 * \see strcmp Vergleich zweier Strings unter Berücksichtigung der Gross-/Kleinschreibung
 */
int String::strCaseCmp(const String &str, size_t size) const
{
	if (size!=(size_t)-1) return strncasecmp(ptr,str.ptr,size);
	return strcasecmp(ptr,str.ptr);
}

int String::strCaseCmp(const char *str, size_t size) const
{
	if (size!=(size_t)-1) return strncasecmp(ptr,str,size);
	return strcasecmp(ptr,str);
}


/*!\brief Linken Teilstring zurückgeben
 *
 * \desc
 * Gibt die ersten \p len Zeichen des Strings als neuen zurück.
 *
 * @param len Länge des Teilstrings
 * @return Neuer String
 */
String String::left(size_t len) const
{
	if(stringlen > 0) {
		if(len > stringlen) len = stringlen;
		return String(ptr,len);
	}
	return String();
}

 /*!\brief Rechten Teilstring zurückgeben
  *
  * \desc
  * Gibt die letzten \p len Zeichen des Strings als neuen zurück.
  *
  * @param len Länge des Teilstrings
  * @return Neuer String
  */
String String::right(size_t len) const
{
	if(stringlen > 0) {
		if(len > stringlen) len = stringlen;
		return String(ptr+stringlen-len,len);
	}
	return String();
}

/*!\brief Teilstring zurückgeben
 *
 * \desc
 * Gibt \p len Zeichen des Strings, beginnend ab Position \p start als
 * neuen String zurück.
 *
 * @param start Startposition
 * @param len Optionale Länge des Teilstrings. Ist der Parameter nicht angegeben, wird
 * der komplette String ab Position \p start zurückgegeben.
 * @return Neuer String
 */
String String::mid(size_t start, size_t len) const
{
	if (len==(size_t)-1) len=stringlen;
	if (start<stringlen && stringlen>0 && len>0) {
		if (start+len>stringlen) len=stringlen-start;
		return String(ptr+start,len);
	}
	return String();
}

/*!\brief Teilstring zurückgeben
 *
 * \desc
 * Gibt \p len Zeichen des Strings, beginnend ab Position \p start als
 * neuen String zurück.
 *
 * @param start Startposition
 * @param len Optionale Länge des Teilstrings. Ist der Parameter nicht angegeben, wird
 * der komplette String ab Position \p start zurückgegeben.
 * @return Neuer String
 */
String String::substr(size_t start, size_t len) const
{
	if (len==(size_t)-1) len=stringlen;
	if (start<stringlen && stringlen>0 && len>0) {
		if (start+len>stringlen) len=stringlen-start;
		return String(ptr+start,len);
	}
	return String();
}

/*! \brief Wandelt alle Zeichen des Strings in Kleinbuchstaben um
 *
 * \desc
 * Diese Funktion wandelt alle Zeichen des Strings in Kleinbuchstaben um. Die genaue Funktionsweise hängt davon ab,
 * welche Spracheinstellungen aktiv sind, genauer vom Wert "LC_CTYPE".
 *
 * \attention Unter UNIX (und möglicherweise anderen Betriebssystemen) werden die Lokalisationseinstellungen der
 * Umgebung nicht automatisch übernommen, sondern stehen standardmäßig auf "C". Dadurch werden nur US-ASCII
 * (ASCII 32 bis 127) umgewandelt. Man sollte daher bei Programmstart mit "setlocale" die gewünschte
 * Spracheinstellung vornehmen.
 *
 * \example
 * \code
 * #include <locale.h>
 * ...
 * // Lokalisierung explizit setzen
 * setlocale(LC_CTYPE,"de_DE.UTF-8");
 * // oder Lokalisierung von den Systemeinstellungen übernehmen
 * setlocale(LC_CTYPE,"");
 * \endcode
 * \par
 */
void String::lowerCase()
{
	if (stringlen==0) return;
	// Wir wandeln den String zunächst nach Unicode um
	wchar_t *buffer=(wchar_t*)malloc((stringlen+1)*sizeof(wchar_t));
	if (!buffer) throw OutOfMemoryException();
	size_t l;
#ifdef HAVE_MBSTOWCS_S
	mbstowcs_s(&l, buffer, stringlen, ptr, stringlen);
#else
	l = mbstowcs(buffer, ptr, stringlen);
#endif
	if (l==(size_t)-1) {
		free(buffer);
		throw CharacterEncodingException();
	}
	// Umwandeln
	for (size_t i=0;i<l;i++) {
		wchar_t wc=buffer[i];
		wchar_t c=towlower(wc);
		if (c!=(wchar_t)WEOF) {
			buffer[i]=c;
		}
	}
	// Zurück im String speichern
	set(buffer,l);
	free(buffer);
}

/*! \brief Wandelt alle Zeichen des Strings in Grossbuchstaben um
 *
 * \desc
 * Diese Funktion wandelt alle Zeichen des Strings in Großbuchstaben um. Die genaue Funktionsweise hängt davon ab,
 * welche Spracheinstellungen aktiv sind, genauer vom Wert "LC_CTYPE".
 *
 * \attention Unter UNIX (und möglicherweise anderen Betriebssystemen) werden die Lokalisationseinstellungen der
 * Umgebung nicht automatisch übernommen, sondern stehen standardmäßig auf "C". Dadurch werden nur US-ASCII
 * (ASCII 32 bis 127) umgewandelt. Man sollte daher nach Programmstart mit "setlocale" die gewünschte
 * Spracheinstellung vornehmen.
 *
 * \example
 * \code
 * #include <locale.h>
 * ...
 * setlocale(LC_CTYPE,"de_DE.UTF-8");
 * \endcode
 */
void String::upperCase()
{
	if (stringlen==0) return;
	// Wir wandeln den String zunächst nach Unicode um
	wchar_t *buffer=(wchar_t*)malloc((stringlen+1)*sizeof(wchar_t));
	if (!buffer) throw OutOfMemoryException();
	size_t l;
#ifdef HAVE_MBSTOWCS_S
	mbstowcs_s(&l, buffer, stringlen, ptr, stringlen);
#else
	l=mbstowcs(buffer, ptr,stringlen);
#endif
	if (l==(size_t)-1) {
		free(buffer);
		throw CharacterEncodingException();
	}
	// Umwandeln
	for (size_t i=0;i<l;i++) {
		wchar_t wc=buffer[i];
		wchar_t c=towupper(wc);
		if (c!=(wchar_t)WEOF) {
			buffer[i]=c;
		}
	}
	// Zurück im String speichern
	set(buffer,l);
	free(buffer);
}

/*!\brief Anfangsbuchstaben der Wörter groß
 *
 * \desc
 * Diese Funktion wandelt die Anfangsbuchstaben aller im String enthaltenen Wörter in
 * Großbuchstaben um.
 */
void String::upperCaseWords()
{
	if (stringlen==0) return;

	// Wir wandeln den String zunächst nach Unicode um
	wchar_t *buffer=(wchar_t*)malloc((stringlen+1)*sizeof(wchar_t));
	if (!buffer) throw OutOfMemoryException();
	size_t l;
#ifdef HAVE_MBSTOWCS_S
	mbstowcs_s(&l, buffer, stringlen, ptr, stringlen);
#else
	l=mbstowcs(buffer, ptr,stringlen);
#endif
	if (l==(size_t)-1) {
		free(buffer);
		throw CharacterEncodingException();
	}
	bool wordstart=true;
	for (size_t i=0;i<l;i++) {
		wchar_t wc=buffer[i];
		if (wordstart) {
			wchar_t c=towupper(wc);
			if (c!=(wchar_t)WEOF) {
				buffer[i]=c;
			}
		}
		if (wc<48 || (wc>57 && wc<65) || (wc>90 && wc<97) || (wc>122 && wc<127) ) {
			wordstart=true;
		} else {
			wordstart=false;
		}
	}
	set(buffer,l);
	free(buffer);
}

//! \brief Schneidet Leerzeichen, Tabs, Returns und Linefeeds am Anfang und Ende des Strings ab
void String::trim()
{
	if (stringlen>0) {
		size_t i,start,ende,s;
		start=0; s=0;
		ende=stringlen;
		for (i=0;i<stringlen;i++) {
			if (ptr[i]==13 || ptr[i]==10 || ptr[i]==32 || ptr[i]=='\t') {
				if (s==0) start=i+1;
			} else {
				s=1; ende=i;
			}
		}
		ptr[ende+1]=0;
		if (start>0)
			memmove(ptr,ptr+start,(ende-start+2)*sizeof(char));
		stringlen=strlen(ptr);
		ptr[stringlen]=0;
	}
}

/*!\brief Schneidet Leerzeichen, Tabs, Returns und Linefeeds am Anfang und Ende des Strings ab
 *
 * \desc
 * Es wird eine Kopie des Strings angelegt und bei dieser alle Leerzeichen, Tabs, Returns und
 * Linefeeds am Anfang und Ende des Strings abgeschnitten. Das Ergebnis wird als Returnwert
 * zurückgegeben. Der Original-String bleibt unverändert.
 */
String String::trimmed() const
{
	String ret=*this;
	ret.trim();
	return ret;
}

String String::toLowerCase() const
{
	String res(this);
	res.lowerCase();
	return res;
}

String String::toUpperCase() const
{
	String res(this);
	res.upperCase();
	return res;
}

String String::toUpperCaseWords() const
{
	String res(this);
	res.upperCaseWords();
	return res;
}


//! \brief Schneidet Leerzeichen, Tabs Returns und Linefeeds am Anfang des Strings ab
void String::trimLeft()
{
	if (stringlen>0) {
		size_t i,start,s;
		start=0; s=0;
		//ende=stringlen;
		for (i=0;i<stringlen;i++) {
			if (ptr[i]==13 || ptr[i]==10 || ptr[i]==32 || ptr[i]=='\t') {
				if (s==0) start=i+1;
			} else {
				s=1; // ende=i;
			}
		}
		if (start>0)
			memmove(ptr,ptr+start,(stringlen-start+1)*sizeof(char));
		stringlen=strlen(ptr);
		ptr[stringlen]=0;
	}
}

//! \brief Schneidet Leerzeichen, Tabs Returns und Linefeeds am Ende des Strings ab
void String::trimRight()
{
	if (stringlen>0) {
		size_t i,ende;
		ende=0;
		for (i=stringlen;i>0;i--) {
			char w=ptr[i-1];
			if (w!=13 && w!=10 && w!=32 && w!='\t') {
				ende=i;
				break;
			}
		}
		ptr[ende]=0;
		stringlen=strlen(ptr);
		ptr[stringlen]=0;
	}
}

//! \brief Schneidet die definierten Zeichen am Anfang des Strings ab
void String::trimLeft(const String &chars)
{
	if (stringlen>0 && chars.stringlen>0) {
		size_t i,start,s,z;
		start=0; s=0;
		for (i=0;i<stringlen;i++) {
			int match=0;
			for (z=0;z<chars.stringlen;z++) {
				if (ptr[i]==chars.ptr[z]) {
					if (s==0) start=i+1;
					match=1;
					break;
				}
			}
			if (!match) {
				s=1;
			}
		}
		if (start>0) {
			memmove(ptr,ptr+start,(stringlen-start+1)*sizeof(char));
			stringlen=strlen(ptr);
		}
	}
}

//! \brief Schneidet die definierten Zeichen am Ende des Strings ab
void String::trimRight(const String &chars)
{
	if (stringlen>0 && chars.stringlen>0) {
		size_t i,ende,z;
		ende=0;
		for (i=stringlen;i>0;i--) {
			char w=ptr[i-1];
			int match=0;
			for (z=0;z<chars.stringlen;z++) {
				if (w==chars.ptr[z]) {
					//if (s==0) start=i+1;
					match=1;
					break;
				}
			}
			if (!match) {
				ende=i;
				break;
			}
		}
		ptr[ende]=0;
		stringlen=strlen(ptr);
	}
}

//! \brief Schneidet die definierten Zeichen am Anfang und Ende des Strings ab
void String::trim(const String &chars)
{
	trimLeft(chars);
	trimRight(chars);
}

/*!\brief Schneidet Zeichen am Ende des Strings ab
 *
 * \desc
 * Diese Funktion schneidet \p num Zeichen vom Ende des Strings ab. Falls \p num
 * größer als der String ist, bleibt ein leerer String zurück.
 *
 * @param num Anzahl Zeichen, die abgeschnitten werden sollen
 */
void String::chopRight(size_t num)
{
	if (stringlen>0) {
		if (stringlen<num) num=stringlen;
		stringlen-=num;
		ptr[stringlen]=0;
	}
}

/*!\brief Schneidet Zeichen am Ende des Strings ab
 *
 * \desc
 * Diese Funktion schneidet \p num Zeichen vom Ende des Strings ab. Falls \p num
 * größer als der String ist, bleibt ein leerer String zurück.
 *
 * @param num Anzahl Zeichen, die abgeschnitten werden sollen
 *
 * \see
 * Die Funktion ist identisch zu String::chopRight
 */
void String::chop(size_t num)
{
	if (stringlen>0) {
		if (stringlen<num) num=stringlen;
		stringlen-=num;
		ptr[stringlen]=0;
	}
}

/*!\brief Schneidet Zeichen am Anfang des Strings ab
 *
 * \desc
 * Diese Funktion schneidet \p num Zeichen vom Anfang des Strings ab. Falls \p num
 * größer als der String ist, bleibt ein leerer String zurück.
 *
 * @param num Anzahl Zeichen, die abgeschnitten werden sollen
 */
void String::chopLeft(size_t num)
{
	if (stringlen>0) {
		if (stringlen<num) num=stringlen;
		memmove(ptr,ptr+num,(stringlen-num)*sizeof(char));
		stringlen-=num;
		ptr[stringlen]=0;
	}
}

/*!\brief Schneidet Returns und Linefeeds am Anfanng und Ende des Strings ab
 *
 * \desc
 * Schneidet Returns und Linefeeds am Anfanng und Ende des Strings ab
 */
void String::chomp()
{
	trim("\n\r");
}

/*!\brief Schneidet den String an einer bestimmten Stelle ab
 *
 * \desc
 * Der String wird an einer bestimmten Stelle einfach abgeschnitten
 * \param pos Die Position, an der der String abgeschnitten wird. Bei Angabe von 0 ist der String anschließend
 * komplett leer. Ist \c pos größer als die Länge des Strings, passiert nichts.
 */
void String::cut(size_t pos)
{
	if (stringlen==0) return;
	if (pos>stringlen) return;
	ptr[pos]=0;
	stringlen=pos;
}

/*! \brief Schneidet den String beim ersten Auftauchen eines Zeichens/Strings ab
 *
 * Der String wird beim ersten Auftauchen eines Zeichens oder eines Strings abgeschnitten.
 * \param[in] letter Buchstabe oder Buchstabenkombination, an der der String abgeschnitten werden
 * soll. Zeigt der Pointer auf NULL oder ist der String leer, passiert nichts.
 */
void String::cut(const String &letter)
{
	if (stringlen==0) return;
	if (letter.isEmpty()) return;
	ssize_t p=instr(letter,0);
	if (p>=0) {
		ptr[p]=0;
		stringlen=p;
	}
}


String String::strchr(char c) const
{
	String ret;
	if (stringlen>0) {
		char *p=::strchr(ptr, c);
		if (p) ret.set(p);
	}
	return ret;
}

String String::strrchr(char c) const
{
	String ret;
	if (stringlen>0) {
		char *p=::strrchr(ptr, c);
		if (p) ret.set(p);
	}
	return ret;
}

/*!\brief Teilstring finden
 *
 * \desc
 * Diese Funktion findet die erste Position der Zeichenfolge \p needle
 * innerhalb des Strings. Abschließende `\0'-Zeichen werden nicht
 * miteinander verglichen.
 *
 * @param needle
 * @return
 * Die Funktion gibt einen String zurück, der mit der gefundenen
 * Zeichenkette beginnt und den Rest des Strings bis zum Ende enthält.
 * Wurde die Zeichenkette nicht gefunden, wird ein leerer String
 * zurückgegeben.
 * \note
 * Ein Sonderfall besteht, wenn \p needle leer ist. In diesem Fall wird
 * der komplette String zurückgegeben.
 */
String String::strstr(const String &needle) const
{
	String ret;
	if (stringlen>0) {
		if (needle.len()==0) return *this;
		char *p=::strstr(ptr, needle.ptr);
		if (p) ret.set(p);
	}
	return ret;
}

/*! \brief Sucht nach einem String
 *
 * Find sucht nach dem Suchstring \a needle ab der gewünschten Position \a start.
 *
 * \param[in] needle Gesuchter Teilstring
 * \param[in] start Optionale Startposition innerhalb des Suchstrings. Ist der Parameter 0 oder wird er weggelassen,
 * wird der String vom Anfang an durchsucht. Ist der Wert jedoch negativ, wird rückwärts vom
 * Ende des Strings gesucht.
 *
 * \return Liefert die Position innerhalb des Strings, an der der Suchstring gefunden wurde
 * oder -1 wenn er nicht gefunden wurde. Ist \p needle ein leerer String, liefert die
 * Funktion immer 0 zurück.
 */
ssize_t String::find(const String &needle, ssize_t start) const
{
	if (stringlen==0) return -1;
	if (needle.stringlen==0) return 0;
	if (start>0 && (size_t)start>=stringlen) return -1;
	if (start<0 && ((size_t)((ssize_t)stringlen+start))>=stringlen) return -1;


	//Position to return
	size_t p = -1;
	//Length of the string to search for
	size_t lstr = needle.stringlen;
	//Current position to search from and position of found string
	char *found = NULL, *tmp = NULL;

	//Search forward
	if(start >= 0) {
		//Search first occurence, starting at the given position...
		found = ::strstr(ptr + start, needle.ptr);
		//...and calculate the position to return if str was found
		if (found != NULL) {
			p = found - ptr;
		}
	} else {
		//Search backward
		//Start counting from behind at 0, not -1
		start++;

		/* Beginning at the start of the contained string, start searching for
			   every occurence of the str and make it the position last found as long
			   as the found string doesn't exceed the defined end of the search */
		while((found = ::strstr((tmp == NULL ? ptr : tmp + 1), needle.ptr)) != NULL && found - ptr + lstr <= stringlen + start)
			tmp = found;

		//Calculate the position to return if str was found
		if(tmp != NULL) {
			p = tmp - ptr;
		}
	}
	return p;
}


/*! \brief Sucht nach einem String, Gross-/Kleinschreibung wird ignoriert
 *
 * \desc
 * Diese Funktion sucht innerhalb des aktuellen String nach \p needle ab der
 * gewünschten Position \a start. Gross-/Kleinschreibung wird dabei ignoriert.
 *
 * \param[in] needle Gesuchter Teilstring
 * \param[in] start Optionale Startposition innerhalb des Suchstrings. Ist der Parameter 0 oder wird er weggelassen,
 * wird der String vom Anfang an durchsucht. Ist der Wert jedoch negativ, wird rückwärts vom
 * Ende des Strings gesucht.
 *
 * \return Liefert die Position innerhalb des Strings, an der der Suchstring gefunden wurde
 * oder -1 wenn er nicht gefunden wurde. Ist \p needle ein leerer String, liefert die
 * Funktion immer 0 zurück.
 */
ssize_t String::findCase(const String &needle, ssize_t start) const
{
	String CaseNeedle(needle);
	String CaseSearch(ptr,stringlen);
	CaseNeedle.lowerCase();
	CaseSearch.lowerCase();
	return CaseSearch.find(CaseNeedle,start);
}

/*! \brief Sucht nach einem String
 *
 * \desc
 * Diese Funktion sucht nach dem Suchstring \a needle ab der gewünschten Position \a start.
 *
 * \param[in] needle Gesuchter Teilstring
 * \param[in] start Optionale Startposition innerhalb des Suchstrings. Ist der Parameter 0
 * oder nicht angegeben, wird der String vom Anfang an gesucht.
 *
 * \return Liefert die Position innerhalb des Strings, an der der Suchstring gefunden wurde
 * oder -1 wenn er nicht gefunden wurde. Ist \p needle ein leerer String, liefert die
 * Funktion immer 0 zurück.
 */
ssize_t String::instr(const String &needle, size_t start) const
{
	if (stringlen==0) return -1;
	if (needle.stringlen==0) return 0;
	if (start>=stringlen) return -1;
	const char * p;
	p=::strstr((ptr+start),needle.ptr);
	if (p!=NULL) {
		return ((ssize_t)(p-ptr));
	}
	return -1;
}

/*! \brief Sucht nach einem String, Gross-/Kleinschreibung wird ignoriert
 *
 * \desc
 * Diese Funktion sucht nach dem Suchstring \a needle ab der gewünschten Position \a start.
 * Gross-/Kleinschreibung wird dabei ignoriert.
 *
 * \param[in] needle Gesuchter Teilstring
 * \param[in] start Optionale Startposition innerhalb des Suchstrings. Ist der Parameter 0
 * oder nicht angegeben, wird der String vom Anfang an gesucht.
 *
 * \return Liefert die Position innerhalb des Strings, an der der Suchstring gefunden wurde
 * oder -1 wenn er nicht gefunden wurde. Ist \p needle ein leerer String, liefert die
 * Funktion immer 0 zurück.
 */
ssize_t String::instrCase(const String &needle, size_t start) const
{
	String CaseNeedle(needle);
	String CaseSearch(ptr,stringlen);
	CaseNeedle.lowerCase();
	CaseSearch.lowerCase();
	return CaseSearch.instr(CaseNeedle,start);
}


bool String::has(const String &needle, bool ignoreCase) const
{
	if (ignoreCase) {
		String CaseSearch(ptr,stringlen);
		String CaseNeedle(needle);
		CaseNeedle.lowerCase();
		CaseSearch.lowerCase();
		return CaseSearch.has(CaseNeedle,false);
	}
	if (stringlen==0) return false;
	if (needle.stringlen==0) return false;
	const char * p;
	p=::strstr(ptr,needle.ptr);
	if (p!=NULL) return true;
	return false;

}

String &String::stripSlashes()
{
	if (stringlen==0) return *this;
	size_t p=0, np=0;
	char a, lastchar=0;
	while ((a=ptr[p])) {
		if (lastchar!='\\' && p>0) {
			ptr[np]=lastchar;
			np++;
		}
		lastchar=a;
		p++;
	}
	if (lastchar) {
		ptr[np]=lastchar;
		np++;
	}
	ptr[np]=0;
	if (stringlen!=np) {
		stringlen=np;
	}
	return *this;
}

/*!\brief String wiederholen
 *
 * \desc
 * Mit dieser Funktion wird der Inhalt des Strings mehrfach wiederholt.
 *
 * @param num Anzahl Wiederholungen. Falls \p num 0 ist, ist der String anschließend leer.
 *
 * @return Referenz auf den verlängerten String.
 */
String& String::repeat(size_t num)
{
	if (stringlen==0) return *this;
	if (num==0) {
		clear();
		return *this;
	}
	size_t newsize=(stringlen*num+16)*sizeof(char);
	char *buf=(char*)malloc(newsize);
	if (!buf) throw OutOfMemoryException();
	char *tmp=buf;
	for (size_t i=0;i<num;i++) {
#ifdef HAVE_STRNCPY_S
		strncpy_s(tmp, newsize, ptr, stringlen);
#else
		strncpy(tmp,ptr,stringlen);
#endif
		tmp+=stringlen;
	}
	free(ptr);
	ptr=buf;
	stringlen=stringlen*num;
	ptr[stringlen]=0;
	s=newsize;
	return *this;
}

/*! \brief Füllt den String mit einem Zeichen
 *
 * Der String wird mit einem gewünschten Zeichen gefüllt
 * \param unicode Der Unicode des Zeichens, mit dem der String gefüllt werden soll
 * \param num Die Länge des gewünschten Strings
 * \return Referenz auf den neuen String
 */
String& String::repeat(char code, size_t num)
{
	if (!code) {
		throw IllegalArgumentException();
	}
	if (!num) {
		clear();
		return *this;
	}
	size_t newsize=(num+16)*sizeof(char);
	char *buf=(char*)malloc(newsize);
	if (!buf) throw OutOfMemoryException();
	for (size_t i=0;i<num;i++) buf[i]=code;
	if (ptr!=empty_string) free(ptr);
	ptr=buf;
	stringlen=num;
	ptr[stringlen]=0;
	s=newsize;
	return *this;
}

/*!\brief String wiederholen
 *
 * \desc
 * Mit dieser Funktion wird der übergebene String \p str \p num mal wiederholt und
 * das Ergebnis in diesem String gespeichert.
 *
 * @param str Der zu wiederholende String
 * @param num Anzahl wiederholungen
 * @return Referenz auf den String
 */
String& String::repeat(const String& str, size_t num)
{
	if (str.stringlen==0 || num==0) {
		clear();
		return *this;
	}
	size_t newsize=(str.stringlen*num+16)*sizeof(char);
	char *buf=(char*)malloc(newsize);
	if (!buf) throw OutOfMemoryException();
	char *tmp=buf;
#ifdef HAVE_STRNCPY_S
	size_t buffer_left = newsize;
#endif
	for (size_t i=0;i<num;i++) {
#ifdef HAVE_STRNCPY_S
		strncpy_s((char*)tmp, buffer_left, str.ptr, str.stringlen);
		buffer_left -= str.stringlen;
#else
		strncpy(tmp,str.ptr,str.stringlen);
#endif
		tmp+=str.stringlen;
	}
	if (ptr!=empty_string) free(ptr);
	ptr=buf;
	stringlen=num;
	ptr[stringlen]=0;
	s=newsize;
	return *this;
}

/*!\brief String multiplizieren
 *
 * \desc
 * Der aktuelle String wird \p count mal hintereinander wiederholt und
 * als neuer String zurückgegeben.
 *
 * @param[in] count Anzahl wiederholungen
 * @return Neuer String
 * \exception OutOfMemoryException Tritt auf, wenn kein Speicher mehr verfügbar ist.
 */
String String::repeated(size_t count) const
{
	String ret;
	for (size_t i=0;i<count;i++) ret.append(ptr,stringlen);
	return ret;
}


String& String::replace(const String &search, const String &replacement)
//! \brief Ersetzt einen Teilstring durch einen anderen
{
	if (stringlen==0 || search.stringlen==0) return *this;
	size_t start = 0, slen = search.stringlen;
	ssize_t end;
	// collect the result
	String ms;
	//Do while str is found in the contained string
	while((end = find(search, start)) >= 0) {
		//The result is built from the parts that don't match str and the replacement string
		ms += mid(start, end - start);
		ms += replacement;
		//New start for search is behind the replaced part
		start = end + slen;
	}
	//Add the remaining part of the contained string to the result
	ms += mid(start);
	//The result is assigned to this mstring
	return set(ms);
}

String &String::pregEscape()
/*! \brief Fügt dem String Escape-Zeichen zu, zur Verwendung in einem Match
 *
 * \desc
 * Der Befehl scannt den String nach Zeichen mit besonderer Bedeutung in einer Perl-Regular-Expression und
 * escaped diese mit einem Slash. Das Ergebnis kann dann in einer Regular Expression verwendet werden.
 *
 * Folgende Zeichen werden escaped: - + \ * /
 */
{
	if (stringlen==0) return *this;
	String t;
	String compare="-+\\*/";
	String letter;
	for (size_t i=0;i<stringlen;i++) {
		letter.set(ptr[i]);
		if (compare.instr(letter,0)>=0) t+="\\";
		t+=letter;
	}
	if (strcmp(t)!=0) set(t);
	return *this;
}

/*! \brief Der String wird anhand einer Regular Expression durchsucht
 *
 * Durchsucht den String anhand einer Perl-Expression
\param[in] expression Ist eine Perl-kompatible Regular Expression, die mit Slash (/) beginnt und
endet. Optional können nach dem schließenden Slash folgende Optionen angegeben werden:
\copydoc pregexpr.dox
\return Liefert \c true(1) zurück, wenn ein Match gefunden wurde, ansonsten \c false(0)
\remarks
Der String wird intern zuerst nach UTF-8 kodiert, bevor die pcre-Funktionen aufgerufen werden.
\note

\copydoc pcrenote.dox
*/
bool String::pregMatch(const String &expression) const
{
	#ifndef HAVE_PCRE
		throw UnsupportedFeatureException("PCRE");
	#else
		if (stringlen==0 || expression.stringlen==0) return false;
		ByteArray expr=expression;
		int flags=PCRE_UTF8|PCRE_NO_UTF8_CHECK;
		flags=0;
		// letzten Slash in regex finden

		const char *options=::strrchr((const char*)expr,'/');
		if (options) {
			expr.set(options-(const char*)expr,0);
			options++;
			if (::strchr(options,'i')) flags|=PCRE_CASELESS;
			if (::strchr(options,'m')) flags|=PCRE_MULTILINE;
			if (::strchr(options,'x')) flags|=PCRE_EXTENDED;
			if (::strchr(options,'s')) flags|=PCRE_DOTALL;
			if (::strchr(options,'a')) flags|=PCRE_ANCHORED;
			if (::strchr(options,'u')) flags|=PCRE_UNGREEDY;
		}
		/*
		printf ("String Dump:\n");
		utf8.hexDump();
		printf ("Expression Dump:\n");
		expr.hexDump();
		*/
		const char *perr;
		int re,erroffset, ovector[32];
		int perrorcode;
		pcre *reg;
		//printf ("expr=>>%s<<, flags=%i\n",((const char*)expr+1),flags);
		reg=pcre_compile2(((const char*)expr+1),flags,&perrorcode,&perr, &erroffset, NULL);
		if (!reg) throw IllegalRegularExpressionException();
		memset(ovector,0,30*sizeof(int));
		//printf ("text=>>%s<<, size=%zi\n",(const char*)utf8,utf8.size());
		if ((re=pcre_exec(reg, NULL, (const char*) ptr,stringlen,0, 0, ovector, 30))>=0) {
			pcre_free(reg);
			return true;
		}
		pcre_free(reg);
		return false;
#endif
}

/*! \brief Der String wird anhand einer Regular Expression durchsucht
 *
 * Durchsucht den String anhand einer Perl-Expression
\param[in] expression Ist eine Perl-kompatible Regular Expression, die mit Slash (/) beginnt und
endet. Optional können nach dem schließenden Slash folgende Optionen angegeben werden:
\copydoc pregexpr.dox
\param[out] matches Array, dass die zu kopierenden Werte aufnimmt.
\param[in] maxmatches Optionaler Parameter, der die maximale Anzahl zu kopierender Werte aufnimmt
(Default=16).
\return Liefert \c true(1) zurück, wenn ein Match gefunden wurde, ansonsten \c false(0)
\remarks
Der String wird intern zuerst nach UTF-8 kodiert, bevor die pcre-Funktionen aufgerufen werden.
\note

\copydoc pcrenote.dox
*/
bool String::pregMatch(const String &expression, Array &matches, size_t maxmatches) const
{
	#ifndef HAVE_PCRE
		throw UnsupportedFeatureException("PCRE");
	#else
		matches.clear();
		if (stringlen==0 || expression.stringlen==0) return false;
		ByteArray expr=expression;
		int flags=PCRE_UTF8|PCRE_NO_UTF8_CHECK;
		// letzten Slash in regex finden
		const char *options=::strrchr((const char*)expr,'/');
		if (options) {
			expr.set(options-(const char*)expr,0);
			options++;
			if (::strchr(options,'i')) flags|=PCRE_CASELESS;
			if (::strchr(options,'m')) flags|=PCRE_MULTILINE;
			if (::strchr(options,'x')) flags|=PCRE_EXTENDED;
			if (::strchr(options,'s')) flags|=PCRE_DOTALL;
			if (::strchr(options,'a')) flags|=PCRE_ANCHORED;
			if (::strchr(options,'u')) flags|=PCRE_UNGREEDY;
		}
		const char *perr;
		if (maxmatches<16) maxmatches=16;
		int re,erroffset;
		int ovectorsize=(maxmatches+1)*2;
		int *ovector=(int*)malloc(ovectorsize*sizeof(int));
		if (!ovector) throw OutOfMemoryException();
		int perrorcode;
		pcre *reg;
		//printf ("r=%s, flags=%i\n",r,flags);
		reg=pcre_compile2(((const char*)expr+1),flags,&perrorcode,&perr, &erroffset, NULL);
		if (!reg) {
			free(ovector);
			throw IllegalRegularExpressionException();
		}
		memset(ovector,0,ovectorsize*sizeof(int));
		if ((re=pcre_exec(reg, NULL, (const char*) ptr,stringlen,0, 0, ovector, ovectorsize))>=0) {
			if (re>0) maxmatches=re;
			else maxmatches=maxmatches*2/3;
			for (size_t i=0;i<maxmatches;i++) {
				const char *tmp=NULL;
				pcre_get_substring((const char*)ptr,ovector,ovectorsize,i,(const char**)&tmp);
				if (tmp) {
					//printf("tmp[%i]=%s\n",i,tmp);
					matches.add(tmp);
					pcre_free_substring(tmp);
				}
			}
			pcre_free(reg);
			free(ovector);
			return true;
		}
		pcre_free(reg);
		free(ovector);
		return false;
#endif
}


/*! \brief Es wird ein Suchen und Ersetzen anhand einer Regular Expression durchgeführt
 *
\param expr is a perl compatible regular expression, starting and ending with slash (/).
\copydoc pregexpr.dox
\param replace ist ein Pointer auf eine Stringklasse, die den Text enthält, der anstelle
des Matches eingesetzt wird. Werden in der Expression Klammern zum capturen verwendet,
können diese Werte mit \c $1 bis \c $9 im Replace-String verwendet werden.
\param maxreplace ist optional. Wenn vorhanden, werden nur soviele Matches ersetzt, wi
mit maxreplace angegeben. Wurden zum Beispiel 10 Matches gefunden, aber maxreplace wurde
mit 5 angegeben, werden nur die ersten 5 Matches ersetzt.
\return Liefert \c true(1) zurück, wenn ein Match gefunden wurde, ansonsten \c false(0)

\copydoc pcrenote.dox
 */
String & String::pregReplace(const String &expression, const String &replacement, int max)
{
#ifndef HAVE_PCRE
	throw UnsupportedFeatureException("PCRE");
#else
	if (stringlen==0 || expression.stringlen==0) return *this;

	String pattern;
	int ret=0;
	char *r=::strdup(expression.ptr+1);
	int flags=PCRE_UTF8|PCRE_NO_UTF8_CHECK;
	char *tmp;
	// letzten Slash in regex finden
	char *options=::strrchr(r,'/');
	if (options) {
		options[0]=0;
		options++;
		if (::strchr(options,'i')) flags|=PCRE_CASELESS;
		if (::strchr(options,'m')) flags|=PCRE_MULTILINE;
		if (::strchr(options,'x')) flags|=PCRE_EXTENDED;
		if (::strchr(options,'s')) flags|=PCRE_DOTALL;
		if (::strchr(options,'a')) flags|=PCRE_ANCHORED;
		if (::strchr(options,'u')) flags|=PCRE_UNGREEDY;
	}

	pattern+=r;
	const char *perr;
	int re,erroffset, ovector[30];
	int perrorcode;
	pcre *reg;
	//printf ("r=%s, flags=%i\n",r,flags);
	String neu;
	String Replace;
	char rep[5];

CString__PregReplace_Restart:
    reg=pcre_compile2(r,flags,&perrorcode,&perr, &erroffset, NULL);
	if (reg) {
		String rest=ptr;
		while (1) {		// Endlosschleife, bis nichts mehr matched
			memset(ovector,0,30*sizeof(int));
			if ((re=pcre_exec(reg, NULL, (const char*) rest,rest.size(),0, 0, ovector, 30))>=0) {
				ret++;
				Replace=replacement;
				for (int i=0;i<14;i++) {
					tmp=NULL;
					pcre_get_substring((const char*)rest,ovector,30,i,(const char**)&tmp);
					if (tmp) {
						//printf("tmp[%i]=%s\n",i,tmp);
						sprintf(rep,"$%i",i);
						Replace.replace(rep,tmp);
						//matches->Set(i,tmp);
						pcre_free_substring(tmp);
					}
				}
				// Erstes Byte steht in ovector[0], letztes in ovecor[1]
				neu.append(rest,ovector[0]);
				neu+=Replace;
				rest.chopLeft(ovector[1]);		// rest abschneiden
				//printf ("Match %i\nNeu: %s\nRest (%i): %s\n",ret,(char*)neu,rest.Len(),(char*)rest);
				if (max>0 && ret>=max) {
					neu+=rest;
					break;
				}
			} else if ((flags&PCRE_UTF8)==PCRE_UTF8 && (re==PCRE_ERROR_BADUTF8 || re==PCRE_ERROR_BADUTF8_OFFSET)) {
				// Wir haben ungültiges UTF_8
				// Vielleicht matched es ohne UTF-8-Flag
				flags-=PCRE_UTF8;
				free(reg);
				goto CString__PregReplace_Restart;

			} else {
				// Kein Match, Schleife beenden
				neu+=rest;
				break;
			}
		}
		free(reg);
	}
	free(r);
	if (ret) set(neu);
	return *this;
#endif
}



/*!\brief Kleiner als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner dem des
 * rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<(const String &str) const
{
	if (::strcmp(ptr,str.ptr)<0) return true;
	return false;
}

/*!\brief Kleiner oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner oder gleich
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<=(const String &str) const
{
	if (strcmp(str)<=0) return true;
	return false;
}

/*!\brief Gleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters mit dem des
 * rechten identisch ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator==(const String &str) const
{
	if (strcmp(str)==0) return true;
	return false;
}

/*!\brief Ungleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters nicht dem des
 * rechten entspricht.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator!=(const String &str) const
{
	if (strcmp(str)==0) return false;
	return true;

}

/*!\brief Größer oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer oder
 * gleich dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>=(const String &str) const
{
	if (strcmp(str)>=0) return true;
	return false;
}

/*!\brief Größer als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>(const String &str) const
{
	if (strcmp(str)>0) return true;
	return false;
}


/*!\brief Kleiner als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner dem des
 * rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<(const char *str) const
{
	if (strcmp(str)<0) return true;
	return false;
}

/*!\brief Kleiner oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters kleiner oder gleich
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator<=(const char *str) const
{
	if (strcmp(str)<=0) return true;
	return false;
}

/*!\brief Gleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters mit dem des
 * rechten identisch ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator==(const char *str) const
{
	if (strcmp(str)==0) return true;
	return false;
}

/*!\brief Ungleich
 *
 * \desc
 * Dieser Operator liefert \c true zurück, wenn der Wert des linken Parameters nicht dem des
 * rechten entspricht.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator!=(const char *str) const
{
	if (strcmp(str)==0) return false;
	return true;

}

/*!\brief Größer oder gleich
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer oder
 * gleich dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>=(const char *str) const
{
	if (strcmp(str)>=0) return true;
	return false;
}

/*!\brief Größer als
 *
 * \desc
 * Dieser Operator liefert true zurück, wenn der Wert des linken Parameters größer
 * dem des rechten ist.
 *
 * @param str Zu vergleichender String
 * @return Liefert \c true oder \c false zurück
 */
bool String::operator>(const char *str) const
{
	if (strcmp(str)>0) return true;
	return false;
}



/*!\brief %Pointer auf den internen C-String
 *
 * \desc
 * Diese Funktion liefert einen %Pointer im Format "const char*" auf den internen
 * C-String der Klasse zurück. Falls der %String leer ist, wird ein
 * %Pointer auf einen leeren %String zurückgegeben. Das Ergebnis kann in \b printf und
 * verwandten Funktionen mit dem Formatstring "%s" verwendet werden.
 *
 * @return %Pointer auf den internen C-String der Klasse
 * \example
 * \code
void PrintString(const ppl7::String &text)
{
	printf ("Der String lautet: %s\n",text.getPtr());
	// oder mittels Operator:
	printf ("Der String lautet: %s\n",(const char*)text);
}
 * \endcode
 * \see
 * Die folgenden Funktionen erfüllen den gleichen Zweck:
 * - const char * String::getPtr() const
 * - const char * String::c_str() const
 * - const char * String::toChar() const
 * - String::operator const char *() const
 */
const char * String::getPtr() const
{
	return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
const char * String::c_str() const
{
	return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
const char * String::toChar() const
{
	return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
String::operator const char *() const
{
	return (const char*)ptr;
}

/*!\brief %Pointer auf den internen C-String
 *
 * \copydetails String::getPtr
 */
String::operator const unsigned char *() const
{
	return (const unsigned char*)ptr;
}


String::operator bool() const
{
	if (isTrue()) return true;
	return false;
}


String::operator int() const
{
	if (!stringlen) return 0;
	return strtol(ptr,NULL,0);

}

String::operator unsigned int() const
{
	if (!stringlen) return 0;
	return strtoul(ptr,NULL,0);
}

String::operator long() const
{
	return toLong();
}

String::operator unsigned long() const
{
	return toUnsignedLong();
}

String::operator long long() const
{
	return toLongLong();
}

String::operator unsigned long long() const
{
	return toUnsignedLongLong();
}

String::operator float() const
{
	return toFloat();
}

String::operator double() const
{
	return toDouble();
}


String::operator std::string() const
{
	return std::string((const char*)ptr,stringlen);
}

String::operator std::wstring() const
{
	if (stringlen==0) return(std::wstring());
	size_t buffersize=(stringlen+1)*sizeof(wchar_t);
	wchar_t * w=(wchar_t*)malloc(buffersize);
	if (!w) throw OutOfMemoryException();
#ifdef HAVE_MBSTOWCS
	size_t wlen=mbstowcs(w, ptr, buffersize);
	if (wlen==(size_t) -1) {
		free(w);
		throw CharacterEncodingException();
	}
	std::wstring ret(w,wlen);
	free(w);
	return(ret);
	/*
#elif HAVE_ICONV
	iconv_t iconvimport=iconv_open(ICONV_UNICODE,GlobalEncoding);
	if ((iconv_t)(-1)==iconvimport) {
		throw UnsupportedCharacterEncodingException();
	}
	char *outbuf=(char*)ptr;
	//HexDump(str,inbytes);
	size_t res=iconv(iconvimport, (ICONV_CONST char **)&str, &inbytes,
				(char**)&outbuf, &outbytes);
	iconv_close(iconvimport);
	if (res==(size_t)(-1)) {
		((wchar_t*)ptr)[0]=0;
		stringlen=0;
		//SetError(289,"%s",strerror(errno));
		throw CharacterEncodingException();
	}
	((wchar_t*)outbuf)[0]=0;
	stringlen=wcslen((wchar_t*)ptr);
	return *this;
	*/
#else
	free(w);
	throw UnsupportedFeatureException();
#endif
}

int String::toInt() const
{
	if (!stringlen) return 0;
	return strtol(ptr,NULL,10);
}

unsigned int String::toUnsignedInt() const
{
	if (!stringlen) return 0;
	return strtoul(ptr,NULL,10);
}

pplint64 String::toInt64() const
{
	if (!stringlen) return 0;
#ifdef HAVE_STRTOLL
	return (pplint64) strtoll(ptr,NULL,10);
#elif defined WIN32
	return (pplint64) _strtoi64(ptr,NULL,10);
#else
	throw TypeConversionException();
#endif
}

ppluint64 String::toUnsignedInt64() const
{
	if (!stringlen) return 0;
#ifdef HAVE_STRTOULL
	return (ppluint64) strtoull(ptr,NULL,10);
#elif defined HAVE_STRTOLL
	return (ppluint64) strtoll(ptr,NULL,10);
#elif defined WIN32
	return (ppluint64) _strtoi64(ptr,NULL,10);
#else
	throw TypeConversionException();
#endif

}

bool String::toBool() const
{
	if (isTrue()) return true;
	return false;
}

long String::toLong() const
{
	if (!stringlen) return 0;
	return strtol(ptr,NULL,10);
}

unsigned long String::toUnsignedLong() const
{
	if (!stringlen) return 0;
	return strtoul(ptr,NULL,10);
}


long long String::toLongLong() const
{
	if (!stringlen) return 0;
#ifdef HAVE_STRTOLL
	return (long long) strtoll(ptr,NULL,10);
#elif defined WIN32
	return (long long) _strtoi64(ptr,NULL,10);
#else
	throw TypeConversionException();
#endif
}

unsigned long long String::toUnsignedLongLong() const
{
	if (!stringlen) return 0;
#ifdef HAVE_STRTOULL
	return (unsigned long long) strtoull(ptr,NULL,10);
#elif defined HAVE_STRTOLL
	return (unsigned long long) strtoll(ptr,NULL,10);
#elif defined WIN32
	return (unsigned long long) _strtoi64(ptr,NULL,10);
#else
	throw TypeConversionException();
#endif
}

float String::toFloat() const
{
	if (!stringlen) return 0.0f;
	return (float)atof(ptr);
}

double String::toDouble() const
{
	if (!stringlen) return 0.0;
	return atof(ptr);
}


/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String &str1, const String& str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const char *str1, const String& str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String &str1, const char *str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const wchar_t *str1, const String& str2)
{
	String s;
	s.set(str1);
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String &str1, const wchar_t *str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const std::string &str1, const String& str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String &str1, const std::string &str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const std::wstring &str1, const String& str2)
{
	String s=str1;
	s.append(str2);
	return s;
}

/*!\brief String addieren
 *
 * \relates ppl7::String
 *
 * \desc
 * Zwei Strings werden zu einem neuen String zusammengefügt.
 *
 * @param[in] str1 Erster String
 * @param[in] str2 Zweiter String
 * @return Neuer String
 */
String operator+(const String &str1, const std::wstring &str2)
{
	String s=str1;
	s.append(str2);
	return s;
}


std::ostream& operator<<(std::ostream& s, const String &str)
{
	return s.write((const char*)str,str.size());
}

} // EOF namespace ppl7


