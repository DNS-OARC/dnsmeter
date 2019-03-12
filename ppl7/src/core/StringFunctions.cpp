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

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#include "ppl7.h"


namespace ppl7 {

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
static void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*
** encode
**
** base64 encode a stream adding padding and line breaks as per spec.
*/
String ToBase64(const ByteArrayPtr &bin)
{
	String res;
    unsigned char in[3], out[4];
    size_t p=0, filelen=bin.size();

    while( p<filelen ) {
        int len = 0;
        for(int i = 0; i < 3; i++ ) {
        	if (p<filelen) {
        		in[i] = (unsigned char) bin.get(p++);
                len++;
            }
        }
        if( len ) {
            encodeblock( in, out, len );
            for(int i = 0; i < 4; i++ ) {
            	res.appendf("%c",out[i]);
            }
        }
    }
    return res;
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
static void decodeblock( unsigned char in[4], unsigned char out[3] )
{
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
*/
ByteArray FromBase64(const String &str)
{
	ByteArray res;
    unsigned char in[4], out[3], v;
    int i, len;
    size_t p=0, filelen=str.len();
    while( p<filelen) {
        for( len = 0, i = 0; i < 4 && p<filelen; i++ ) {
            v = 0;
            while( p<filelen && v == 0 ) {
                v = str.get(p++);
                v = ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) {
                    v = ((v == '$') ? 0 : v - 61);
                }
            }
            if( p<filelen+1 ) {
                if( v ) {
                	len++;
                    in[ i ] = (unsigned char) (v - 1);
                }
            }
            else {
                in[i] = 0;
            }
        }
        if( len) {
            decodeblock( in, out );
            res.append(out,len-1);
            len=0;
        }
    }
    return res;
}

String StripSlashes(const String &str)
{
	String ret=str;
	ret.stripSlashes();
	return ret;
}

/*!\brief Schneidet Leerzeichen, Tabs Returns und Linefeeds am Anfang und Ende des Strings ab
 * \relates String
 *
 * \desc
 * Schneidet Leerzeichen, Tabs Returns und Linefeeds am Anfang und Ende des Strings ab
 * @param str Eingabe-String
 * @return Neuer String
 */
String Trim(const String &str)
{
	String ret=str;
	ret.trim();
	return ret;
}

String UpperCase(const String &str)
{
	String ret=str;
	ret.upperCase();
	return ret;
}

String LowerCase(const String &str)
{
	String ret=str;
	ret.lowerCase();
	return ret;
}

int StrCmp(const String &s1, const String &s2)
{
	int cmp=s1.strcmp(s2);
	if (cmp<0) return -1;
	if (cmp>0) return 1;
	return 0;
}

int StrCaseCmp(const String &s1, const String &s2)
{
	int cmp=s1.strCaseCmp(s2);
	if (cmp<0) return -1;
	if (cmp>0) return 1;
	return 0;
}


/*!\brief Sucht nach Zeichen in einem String
 * \relates String
 *
 * \code
ssize_t Instr (const char * haystack, const char * needle, size_t start);
ssize_t Instr (const wchar_t * haystack, const wchar_t * needle, size_t start);
ssize_t Instr (const String &haystack, const String &needle, size_t start);
ssize_t Instrcase (const char * haystack, const char * needle, size_t start);
ssize_t Instrcase (const wchar_t * haystack, const wchar_t * needle, size_t start);
ssize_t Instrcase (const String &haystack, const String &needle, size_t start);
\endcode
 * \desc
 * Diese Funktionen suchen nach einer Zeichenkette innerhalb eines Strings, wobei
 * die Funktion \c Instr zwischen Gross- und Kleinschreibung unterschiedet, und
 * die Funktion \c Instrcase nicht.
 *
 * \param haystack ist ein Pointer auf einen Null-terminierten String, der
 * den zu durchsuchenden String enthält.
 * \param needle ist ein Pointer auf einen Null-terminierten String, der
 * den zu suchenden String enthält.
 * \param start ist ein optionaler Parameter, der die Startposition innerhalb des Suchstrings angibt.
 * Der Default ist 0, das heißt der String wurd vom Anfang an durchsucht.
 * \return Wird der String \p needle gefunden, wird seine Position zurückgegeben. Befindet sich der
 * String gleich an erster Stelle des Suchstrings, wird 0 zurückgegeben. Wird der String nicht gefunden,
 * wird -1 zurückgegeben.
 *
 */
ssize_t Instr (const char * haystack, const char * needle, size_t start)
{
	if (!haystack) return -1;
	if (!needle) return -1;
	if (start<strlen(haystack)) {
		const char *_t=strstr((haystack+start),needle);
		if (_t!=NULL) {
			return ((ssize_t)(_t-haystack));
		}
	}
	return (-1);
}

#ifndef HAVE_STRCASESTR
static const char *mystrcasestr(const char *haystack, const char *needle)
{
    char c;
    if ((c = *needle++) != 0) {
        c = tolower((unsigned char)c);
        size_t len = strlen(needle);
        do {
        	char sc;
            do {
                if ((sc = *haystack++) == 0)
                    return (NULL);
            } while ((char)tolower((unsigned char)sc) != c);
        } while (strncasecmp(haystack, needle, len) != 0);
        haystack--;
    }
    return ((char *)haystack);
}
#endif


/*!\brief Sucht nach Zeichen in einem String und ignoriert Gross-/Kleinschreibung
 * \relates String
 *
 * \copydoc Instr(const char * haystack, const char * needle, size_t start)
 */
ssize_t Instrcase (const char * haystack, const char * needle, size_t start)
{
	if (!haystack) return -1;
	if (!needle) return -1;
	if (start<strlen(haystack)) {
		const char * _t;
#ifdef HAVE_STRCASESTR
		_t=strcasestr((haystack+start),needle);
#else
		_t=mystrcasestr((haystack+start),needle);
#endif
		if (_t!=NULL) {
			return ((long)(_t-haystack));
		}
	}
	return (-1);
}

/*!\brief Sucht nach Zeichen in einem String
 * \relates String
 *
 * \copydoc Instr(const char * haystack, const char * needle, size_t start)
 */
ssize_t Instr (const wchar_t * haystack, const wchar_t * needle, size_t start)
{
	if (!haystack) return -1;
	if (!needle) return -1;
	if (start<wcslen(haystack)) {
		const wchar_t * _t;
		_t=wcsstr((haystack+start),needle);
		if (_t!=NULL) {
			return ((ssize_t)(_t-haystack));
		}
	}
	return (-1);
}

/*!\brief Sucht nach Zeichen in einem String und ignoriert Gross-/Kleinschreibung
 * \relates String
 *
 * \copydoc Instr(const char * haystack, const char * needle, size_t start)
 */
ssize_t Instrcase (const wchar_t * haystack, const wchar_t * needle, size_t start)
{
	if (!haystack) return -1;
	if (!needle) return -1;
	wchar_t * myHaystack=wcsdup(haystack);
	if (!myHaystack) throw OutOfMemoryException();

	wchar_t * myNeedle=wcsdup(needle);
	if (!myNeedle) throw OutOfMemoryException();

	size_t len=wcslen(myHaystack);
	if (start<len) {
		// String in Kleinbuchstaben umwandeln
		wchar_t wc;
		for (size_t i=0;i<len;i++) {
			wc=myHaystack[i];
			wc=towlower(wc);
			if (wc!=(wchar_t)WEOF) {
				myHaystack[i]=wc;
			}
		}
		// Needle in Kleinbuchstaben umwandeln
		len=wcslen(myNeedle);
		for (size_t i=0;i<len;i++) {
			wc=myNeedle[i];
			wc=towlower(wc);
			if (wc!=(wchar_t)WEOF) {
				myNeedle[i]=wc;
			}
		}

		const wchar_t * _t;
		_t=wcsstr((myHaystack+start),myNeedle);
		if (_t!=NULL) {
			free(myHaystack);
			free(myNeedle);
			return ((ssize_t)(_t-myHaystack));
		}
	}
	free(myHaystack);
	free(myNeedle);
	return (-1);
}

/*!\brief Sucht nach Zeichen in einem String
 * \relates String
 *
 * \copydoc Instr(const char * haystack, const char * needle, size_t start)
 */
ssize_t Instr (const String &haystack, const String &needle, size_t start)
{
	return haystack.instr(needle,start);
}

/*!\brief Sucht nach Zeichen in einem String und ignoriert Gross-/Kleinschreibung
 * \relates String
 *
 * \copydoc Instr(const char * haystack, const char * needle, size_t start)
 */
ssize_t InstrCase (const String &haystack, const String &needle, size_t start)
{
	return haystack.instrCase(needle,start);
}

String Left(const String &str, size_t num)
{
	return str.left(num);
}

String Right(const String &str, size_t num)
{
	return str.right(num);
}

String Mid(const String &str, size_t start, size_t num)
{
	return str.mid(start,num);
}

String SubStr(const String &str, size_t start, size_t num)
{
	return str.substr(start,num);
}

String ToString(const char *fmt, ...)
{
	String str;
	va_list args;
	va_start(args, fmt);
	str.vasprintf(fmt,args);
	va_end(args);
	return str;
}

String Replace(const String &string, const String &search, const String &replace)
{
	String Tmp=string;
	Tmp.replace(search,replace);
	return Tmp;
}

bool IsTrue(const String &str)
{
	return str.isTrue();
}


/*!\brief Der String wird anhand einer Regular Expression durchsucht
 * \relates String
 *
 * \desc
 * Durchsucht den String \p subject anhand der Perl-Expression \p expression
 * und gibt \c true zurück, wenn die Expression auf den String matched.
 *
 * \param[in] expression Ist eine Perl-kompatible Regular Expression, die mit Slash (/) beginnt und
 * endet. Optional können nach dem schließenden Slash folgende Optionen angegeben werden:
 * \copydoc pregexpr.dox
 * \param[in] subject Der String, auf den die Regular Expression angewendet werden soll
 * \return Liefert \c true zurück, wenn ein Match gefunden wurde, ansonsten \c false
 * \remarks
 * Der String wird intern zuerst nach UTF-8 kodiert, bevor die pcre-Funktionen aufgerufen werden.
 * \note
 * \copydoc pcrenote.dox
 */
bool PregMatch(const String &expression, const String &subject)
{
	return subject.pregMatch(expression);
}

/*!\brief Der String wird anhand einer Regular Expression durchsucht
 * \relates String
 *
 * \desc
 * Durchsucht den String anhand einer Perl-Expression und liefert die die zu kopierenden Werte (Capture)
 * in einem Array zurück.
 *
 * \param[in] expression Ist eine Perl-kompatible Regular Expression, die mit Slash (/) beginnt und
 * endet. Optional können nach dem schließenden Slash folgende Optionen angegeben werden:
 * \copydoc pregexpr.dox
 * \param[in] subject Der String, auf den die Regular Expression angewendet werden soll
 * \param[out] matches Array, dass die zu kopierenden Werte aufnimmt.
 * \param[in] maxmatches Optionaler Parameter, der die maximale Anzahl zu kopierender Werte aufnimmt
 * (Default=16).
 * \return Liefert \c true(1) zurück, wenn ein Match gefunden wurde, ansonsten \c false(0)
 * \remarks
 * Der String wird intern zuerst nach UTF-8 kodiert, bevor die pcre-Funktionen aufgerufen werden.
 * \note
 * \copydoc pcrenote.dox
 */
bool PregMatch(const String &expression, const String &subject, Array &matches, size_t maxmatches)
{
	return subject.pregMatch(expression,matches, maxmatches);
}


/*!\brief String anhand eines Trennzeichens zerlegen
 *
 * \desc
 * Die StrTok-Funktion zerlegt den String \p string in mehrere Teile, wobei
 * \div als Trenner verwendet und das Ergebnis als Array zurückgegeben wird
 * Der Trenner \p div kann aus einem oder mehreren Zeichen bestehen und
 * wird nicht im Ergebnis übernommen. Eine Sequenz von mehreren Trennern
 * hintereinander wird als ein Trenner interpretiert. Trenner am Anfang und Ende
 * des Strings werden ignoriert. Mit anderen Worten: im Ergebnis gibt es keine
 * leeren Strings.
 * \note
 * Das Verhalten der Funktion entspricht dem Verhalten der C-Funktion strtok
 *
 * @param[in] string String, der zerlegt werden soll
 * @param[in] div String, der als Trenner verwendet wird
 * @return Array mit den Bestandteilen des zerlegten Strings
 */
Array StrTok(const String &string, const String &div)
{
	Array ret;
	StrTok(ret, string, div);
	return ret;
}

/*!\brief String anhand eines Trennzeichens zerlegen
 *
 * \desc
 * Die StrTok-Funktion zerlegt den String \p string in mehrere Teile, wobei
 * \div als Trenner verwendet und das Ergebnis im Array \p result gespeichert
 * wird. Der Trenner \p div kann aus einem oder mehreren Zeichen bestehen und
 * wird nicht im Ergebnis übernommen. Eine Sequenz von mehreren Trennern
 * hintereinander wird als ein Trenner interpretiert. Trenner am Anfang und Ende
 * des Strings werden ignoriert. Mit anderen Worten: im Ergebnis gibt es keine
 * leeren Strings.
 * \note
 * Das Verhalten der Funktion entspricht dem Verhalten der C-Funktion strtok
 *
 * @param[out] result Array, in dem die Ergebnisstrings gespeichert werden
 * @param[in] string String, der zerlegt werden soll
 * @param[in] div String, der als Trenner verwendet wird. Default=Newline
 */
void StrTok(Array &result, const String &string, const String &div)
{
	result.clear();
	if (string.isEmpty()) return;
	String Line;
	Array a;
	if (div.isEmpty()) a.explode(string,"\n");
	else a.explode(string,div);
	//printf ("StrTok: a.size=%ti\n",a.size());
	for (size_t i=0;i<a.size();i++) {
		Line=a[i];
		if (Line.notEmpty()) result.add(Line);
	}
}


String EscapeHTMLTags(const String &html)
{
	String s;
	s=html;
	s.replace("&","&amp;");
	s.replace("<","&lt;");
	s.replace(">","&gt;");
	return s;
}

String UnescapeHTMLTags(const String &html)
{
	String s;
	s=html;
	s.replace("&amp;","&");
	s.replace("&lt;","<");
	s.replace("&gt;",">");
	return s;
}

ByteArray Hex2ByteArray(const String &hex)
{
	ByteArray b;
	b.fromHex(hex);
	return b;
}

String ToHex(const ByteArrayPtr &bin)
{
	return bin.toHex();
}

/*!\brief String zur Verwendung in einer URL umwandeln
 *
 * \desc
 * Mit dieser Funktion kann ein beliebiger String so umkodiert werden, dass er als
 * Parameter in einer URL verwendet werden kann. Dabei werden alle Spaces durch "+" ersetzt
 * und alle nicht alphanummerischen Zeichen mit Ausnahme von "-_.!~*'()" in ihre Hex-Werte
 * mit vorangestelltem Prozentzeichen umgewandelt.
 *
 * @param text Der zu kodierende Text
 * @return Der URL-kodierte Text
 *
 * \example
 * \code
 * ppl7::String text=L"Hallo Welt! 1+1=2";
 * printf("%s\n",(const char*)ppl7::UrlEncode(text));
 * \endcode
 * ergibt:
 * \verbatim
Hallo+Welt!+1%2B1%3D2
\endverbatim
 * \see
 * Mit UrlDecode kann der Kodierte String wieder dekodiert werden
 */
String UrlEncode(const String &text)
{
	const char *source=text.getPtr();
	String ret;
	static const char *digits = "0123456789ABCDEF";
	while (*source)
	{
		unsigned char ch = (unsigned char)*source;
		if (*source == ' ') {
			ret+=L"+";
		}
		else if (
				(ch>='a' && ch<='z')
				|| (ch>='A' && ch<='Z')
				|| (ch>='0' && ch<='9')
				|| (strchr("-_.!~*'()", ch))
				) {
			ret+=ch;
		}
		else {
			ret+="%";
			ret+= digits[(ch >> 4) & 0x0F];
			ret+= digits[       ch & 0x0F];
		}
		source++;
	}
	return ret;
}

static char HexPairValue(const char * code) {
	char value = 0;
	const char * pch = code;
	for (;;) {
		int digit = *pch++;
		if (digit >= '0' && digit <= '9') {
			value += digit - '0';
		}
		else if (digit >= 'A' && digit <= 'F') {
			value += digit - 'A' + 10;
		}
		else if (digit >= 'a' && digit <= 'f') {
			value += digit - 'a' + 10;
		}
		else {
			return -1;
		}
		if (pch == code + 2)
			return value;
		value <<= 4;
	}
	return 0;
}

/*!\brief URL-kodierten String dekodieren
 *
 * \desc
 * Mit dieser statischen Funktion kann ein URL-kodierter String dekodiert werden.
 *
 * @param text Der zu URL-kodierte String
 * @return Der dekodierte String
 *
 * \example
 * \code
 * ppl7::String text=L"Hallo+Welt!+1%2B1%3D2";
 * printf("%s\n",(const char*)ppl7::UrlDecode(text));
 * \endcode
 * ergibt:
 * \verbatim
Hallo Welt! 1+1=2";
\endverbatim
 * \see
 * Mit UrlEncode kann ein unkodierter String kodiert werden.
 */
String UrlDecode(const String &text)
{
	const char *source=text.getPtr();
	String ret;

	while (*source) {
		switch (*source) {
			case '+':
				ret+=" ";
				break;
			case '%':
				if (source[1] && source[2]) {
					char value = HexPairValue(source + 1);
					if (value >= 0) {
						ret+=value;
						source += 2;
					}
					else {
						ret+=L"?";
					}
				}
				else {
					ret+="?";
				}
				break;
			default:
				ret+=*source;
				break;
		}
		source++;
	}
	return ret;
}

String Transcode(const char * str, size_t size, const String &fromEncoding, const String &toEncoding)
{
#ifndef HAVE_ICONV
	throw UnsupportedFeatureException("Iconv");
#else
	ppl7::Iconv iconv(fromEncoding,toEncoding);
	ppl7::ByteArrayPtr source(str,size);
	ppl7::ByteArray target;
	iconv.transcode(source,target);
	return String((const char*)target.ptr(),target.size());
#endif
}

String Transcode(const String &str, const String &fromEncoding, const String &toEncoding)
{
#ifndef HAVE_ICONV
	throw UnsupportedFeatureException("Iconv");
#else
	ppl7::Iconv iconv(fromEncoding,toEncoding);
	String to;
	iconv.transcode(str,to);
	return to;
#endif

}

} // EOF namespace ppl7


