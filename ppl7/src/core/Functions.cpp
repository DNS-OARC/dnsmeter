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
#ifdef WIN32
#include <windows.h>
#endif

#include "ppl7.h"

namespace ppl7 {

static int	printdebug=0;

/*!\brief Ausgabe für Debug-Output angeben
 *
 * Mit dieser Funktion wird festgelegt, an welcher Stelle die Textausgabe erfolgen soll.
 * Einige Funktionen (z.B. CAssocArray::List) geben Informationen aus. Standardmäßig werden diese
 * per STDOUT auf die Konsole ausgegeben. Unter Windows gibt es aber manchmal keine Konsole. Hier kann
 * es dann ganz hilfreich sein die Ausgabe im Debugger von Visual Studio zu sehen.
 *
 * \param[in] type 0=STDOUT, 1=VisualStudio Debugger
 */
void SetGlobalOutput(int type)
{
	printdebug=type;
}

/*!\brief Interne Funktion zur Ausgabe von Text
 *
 * Diese Funktion dient als Ersatz für "printf" und wird intern von einigen Funktionen/Klasse zur
 * Ausgabe von Text verwendet. Über die Funktion SetGlobalOutput kann bestimmt werden, ob dieser
 * Text per STDOUT auf die Konsole ausgegeben werden soll oder beispielsweise im Debugger von
 * VisualStudio unter Windows.
 *
 * \param[in] format Formatstring für den Text
 * \param[in] ...    Optionale Parameter, die im Formatstring eingesetzt werden sollen
 */
void PrintDebug(const char *format, ...)
{
	if (!format) return;
	char *buff=NULL;
	va_list args;
	va_start(args, format);
#ifdef HAVE_VASPRINTF
	if (vasprintf (&buff, format, args)<0) {
#else
	if (compat::vasprintf (&buff, format, args)<0) {
#endif
		va_end(args);
		return;
	}
	va_end(args);
	if (!buff) return;
	if (printdebug==1) {
#ifdef PPLVISUALC
		WideString ws;
		ws.set(buff);
		OutputDebugString((LPCWSTR)ws);
#elif defined WIN32
		OutputDebugString(buff);
#endif
	} else {
		printf("%s",buff);
	}
	free(buff);
}

/*!\brief Interne Funktion zur Ausgabe von Text
 *
 * Diese Funktion dient als Ersatz für "printf" und wird intern von einigen Funktionen/Klasse zur
 * Ausgabe von Text verwendet. Über die Funktion SetGlobalOutput kann bestimmt werden, ob dieser
 * Text per STDOUT auf die Konsole ausgegeben werden soll oder beispielsweise im Debugger von
 * VisualStudio unter Windows.
 *
 * \param[in] format Formatstring für den Text
 * \param[in] ...    Optionale Parameter, die im Formatstring eingesetzt werden sollen
 */
void PrintDebugTime(const char *format, ...)
{
	if (!format) return;
	char *buff=NULL;
	va_list args;
	va_start(args, format);
#ifdef HAVE_VASPRINTF
	if (vasprintf (&buff, format, args)<0) {
#else
	if (compat::vasprintf (&buff, format, args)<0) {
#endif
		va_end(args);
		return;
	}
	va_end(args);
	if (!buff) return;
	DateTime now;
	now.setCurrentTime();
	String Time=now.getISO8601withMsec();
	Time+=": ";

	if (printdebug==1) {
#ifdef PPLVISUALC
		WideString ws;
		ws.set(Time);
		OutputDebugString((LPCWSTR)ws);
		ws.set(buff);
		OutputDebugString((LPCWSTR)ws);
#elif defined WIN32
		OutputDebugString((const char*)Time);
		OutputDebugString(buff);
#endif
	} else {
		printf("%s%s",(const char*)Time,buff);
	}
	free(buff);
}

void HexDump(const void *address, size_t bytes, bool skipheader)
{
    char buff[1024], tmp[10], cleartext[20];
    if (!skipheader) {
    	printf ("HEXDUMP: %zu Bytes starting at Address %p:\n",bytes,address);
    }

    const char *_adresse=(const char*)address;
    const char *start_adr=_adresse;
    int spalte=0;
    //sprintf (buff,"%p: ",_adresse);
    buff[0]=0;
    memset(cleartext,0,20);
    for (size_t i=0;i<bytes;i++) {
        sprintf (tmp,"%02X ",(ppluint8)_adresse[i]);
        strcat (buff,tmp);
        if ((ppluint8)_adresse[i]>31 && (ppluint8)_adresse[i]<128)  cleartext[spalte]=(ppluint8)_adresse[i];
        else cleartext[spalte]='.';
        spalte++;
        if (spalte>15) {
            buff[16*3-1]=0;
            printf("%p: %s: %s\n",start_adr, buff, cleartext);
            buff[0]=0;
            memset(cleartext,0,20);
            spalte=0;
            start_adr=_adresse+i+1;
        }
    }

    if (spalte>0) {
        strcat(buff,"                                                               ");
        buff[16*3-1]=0;
        printf("%p: %s: %s\n",start_adr, buff, cleartext);
    }
    if (!skipheader) printf("\n");
}

void HexDump(const void *address, size_t bytes)
{
	HexDump(address,bytes,false);
}




/*!\defgroup PPLGroupPeekPoke Peek und Poke
 * \brief Funktionen zum Zugriff auf den Speicher
 * \ingroup PPLGroupMemory
 *
 * \desc
 * Bei den Peek- und Poke-Funktionen handelt es sich um Funktionen zum Platform-unabhängigem
 * Schreiben und Lesen von Werten im Hauptspeicher. Die Funktionsnamen sind eine Homage an den
 * <a href="http://http://de.wikipedia.org/wiki/Commodore_64">Commodore 64</a>,
 * zu dessen BASIC-Wortschatz Peek und Poke gehörten.
 * \par
 * Bei den Poke-Funktionen handelt es sich um Funktionen zum Schreiben in den Speicher im
 * Little-Endian-Format, was beispielsweise von Intel und AMD verwendet wird. Mit den Peek-Funktionen
 * kann der Wert wieder ausgelesen werden.
 * \par
 * Bei PokeN und PeekN handelt es sich um identische Funktionen, die die Werte jedoch im
 * Big-Endian-Format verarbeiten, was auf vielen RISC-CPUs zu finden ist, z.B. Motorola, Sparc und
 * PowerPC. Da derartige CPUs häufig in Netzwerk-Equipment zu finden ist (Routern), spricht man
 * auch von "Network-Byte-Order", woher auch das "N" im Namen der Funktionen kommt.
 * \par
 * Mehr Informationen zu Big- und Little-Endian sind in der Wikipedia zu finden:
 * http://de.wikipedia.org/wiki/Byte-Reihenfolge
 * \par Verwendung
 * In der Regel wird man mit den durch die Sprache C/C++ bereitgestellten Mitteln
 * auf den Speicher zugreifen, da dies die optimalste und performanteste Art und Weise ist,
 * und man sich um die Architektur des Rechners keine Gedanken machen muss.
 * \par
 * Aber immer dann, wenn man binäre Daten mit anderen Rechnern austauschen muss, deren
 * Architektur man nicht kenn, empfiehlt es sich ein einheitliches Speicherformat zu verwenden.
 * So verwendet beispielsweise die Klasse AssocArray bei ihren Im- und Export-Funktionen
 * PeekN und PokeN zur Darstellung aller Integer-Werte.
 *
 */


/*!\brief 8-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke8 (void *Adresse, ppluint32 Wert)
{
	((ppluint8*)Adresse)[0]=(ppluint8)(Wert & 255);
}

/*!\brief 16-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke16 (void *Adresse, ppluint32 Wert)
{
	#ifdef __BIG_ENDIAN__
		((ppluint8*)Adresse)[0]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >>8)&255);
	#else
		((ppluint16*)Adresse)[0]=(ppluint16)(Wert & 0xffff);
	#endif

}

/*!\brief 24-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 24 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke24 (void *Adresse, ppluint32 Wert)
{
	((ppluint8*)Adresse)[0]=(ppluint8)(Wert & 255);
	((ppluint8*)Adresse)[1]=(ppluint8)((Wert >>8)&255);
	((ppluint8*)Adresse)[2]=(ppluint8)((Wert >>16)&255);
}

/*!\brief 32-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke32 (void *Adresse, ppluint32 Wert)
{
	#ifdef __BIG_ENDIAN__
		((ppluint8*)Adresse)[0]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >>8)&255);
		((ppluint8*)Adresse)[2]=(ppluint8)((Wert >>16)&255);
		((ppluint8*)Adresse)[3]=(ppluint8)((Wert >>24)&255);
	#else
		((ppluint32*)Adresse)[0]=Wert;
	#endif
}

/*!\brief 64-Bit-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit des Wertes werden in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void Poke64 (void *Adresse, ppluint64 Wert)
{
	#ifdef __BIG_ENDIAN__
		((ppluint8*)Adresse)[0]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >>8)&255);
		((ppluint8*)Adresse)[2]=(ppluint8)((Wert >>16)&255);
		((ppluint8*)Adresse)[3]=(ppluint8)((Wert >>24)&255);
		((ppluint8*)Adresse)[4]=(ppluint8)((Wert >>32)&255);
		((ppluint8*)Adresse)[5]=(ppluint8)((Wert >>40)&255);
		((ppluint8*)Adresse)[6]=(ppluint8)((Wert >>48)&255);
		((ppluint8*)Adresse)[7]=(ppluint8)((Wert >>56)&255);
	#else
		((ppluint64*)Adresse)[0]=Wert;
	#endif
}

/*!\brief 32-Bit-Float-Wert schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die Inhalt des Floats \p Wert wird in die angegebene Speicheradresse
 * im Little-Endian-Format geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeFloat(void *Adresse, float Wert)
{
	#ifdef __BIG_ENDIAN__
		((float*)Adresse)[0]=Wert;
	#else
		((float*)Adresse)[0]=Wert;
	#endif
}

/*!\brief 8-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 Peek8 (const void *Adresse)
{
	return (ppluint32)((ppluint8*)Adresse)[0];
}

/*!\brief 16-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 Peek16 (const void * Adresse)
{
	#ifdef __BIG_ENDIAN__
		ppluint8 wert1,wert2;
		wert1=((ppluint8*)Adresse)[0];
		wert2=((ppluint8*)Adresse)[1];
		return((ppluint32) (ppluint32)wert1+((ppluint32)wert2<<8));
	#else
		return (ppluint32) ((ppluint16*)Adresse)[0];
	#endif
}

/*!\brief 24-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 24 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 Peek24 (const void *Adresse)
{
	ppluint8 wert1,wert2,wert3;
	wert1=((ppluint8*)Adresse)[0];
	wert2=((ppluint8*)Adresse)[1];
	wert3=((ppluint8*)Adresse)[2];
	return((ppluint32) wert1+(wert2<<8)+(wert3<<16));
}

/*!\brief 32-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 Peek32 (const void *Adresse)
{
	#ifdef __BIG_ENDIAN__
		ppluint8 wert1,wert2,wert3,wert4;
		wert1=((ppluint8*)Adresse)[0];
		wert2=((ppluint8*)Adresse)[1];
		wert3=((ppluint8*)Adresse)[2];
		wert4=((ppluint8*)Adresse)[3];

		return((ppluint32) (ppluint32)wert1+((ppluint32)wert2<<8)+((ppluint32)wert3<<16)+((ppluint32)wert4<<24));
	#else
		return (ppluint32) ((ppluint32*)Adresse)[0];
	#endif
}

/*!\brief 64-Bit-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint64 Peek64 (const void *Adresse)
{
	#ifdef __BIG_ENDIAN__
		ppluint8 wert1,wert2,wert3,wert4,wert5,wert6,wert7,wert8;
		wert1=((ppluint8*)Adresse)[0];
		wert2=((ppluint8*)Adresse)[1];
		wert3=((ppluint8*)Adresse)[2];
		wert4=((ppluint8*)Adresse)[3];
		wert5=((ppluint8*)Adresse)[4];
		wert6=((ppluint8*)Adresse)[5];
		wert7=((ppluint8*)Adresse)[6];
		wert8=((ppluint8*)Adresse)[7];

		return((ppluint64) (ppluint64)wert1+((ppluint64)wert2<<8)+((ppluint64)wert3<<16)+((ppluint64)wert4<<24)+((ppluint64)wert5<<32)+((ppluint64)wert6<<40)+((ppluint64)wert7<<48)+((ppluint64)wert8<<56));
	#else
		return (ppluint64) ((ppluint64*)Adresse)[0];
	#endif
}

/*!\brief 32-Bit-Float-Wert auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit der angegebenen Adresse werden im Little-Endian-Format
 * ausgelesen und als Float-Wert zurückgegeben.
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
float PeekFloat(const void *Adresse)
{
	#ifdef __BIG_ENDIAN__
		return (float)((float*)Adresse)[0];
	#else
		return (float)((float*)Adresse)[0];
	#endif
}

/*!\brief 8-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN8 (void *Adresse, ppluint32 Wert)
{
	((ppluint8*)Adresse)[0]=(ppluint8)(Wert & 255);
}

/*!\brief 16-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN16 (void *Adresse, ppluint32 Wert)
{
	#ifdef __BIG_ENDIAN__
		// Direktes schreiben löst bei unalignten Adressen unter Solaris einen Bus-Error aus
		//((ppluint16*)Adresse)[0]=(ppluint16)(Wert & 0xffff);
		((ppluint8*)Adresse)[1]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[0]=(ppluint8)((Wert >> 8)&255);
	#else
		((ppluint8*)Adresse)[1]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[0]=(ppluint8)((Wert >> 8)&255);
	#endif

}

/*!\brief 32-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN32 (void *Adresse, ppluint32 Wert)
{
	#ifdef __BIG_ENDIAN__
		// Direktes schreiben löst bei unalignten Adressen unter Solaris einen Bus-Error aus
		//((ppluint32*)Adresse)[0]=Wert;
		((ppluint8*)Adresse)[3]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[2]=(ppluint8)((Wert >> 8)&255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >> 16)&255);
		((ppluint8*)Adresse)[0]=(ppluint8)((Wert >> 24)&255);
	#else
		/*
#ifdef HAVE_X86_ASSEMBLER
#if __GNUC__ > 3
		asm(".intel_syntax noprefix\n"
			"bswap eax\n"
			": : \"eax\"(Wert) :"
			".att_syntax prefix\n"

				);
#endif
#endif
*/
		((ppluint8*)Adresse)[3]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[2]=(ppluint8)((Wert >> 8)&255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >> 16)&255);
		((ppluint8*)Adresse)[0]=(ppluint8)((Wert >> 24)&255);
	#endif

}

/*!\brief 64-Bit-Wert in Network-Byteorder schreiben
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit des Wertes werden in die angegebene Speicheradresse
 * im Big-Endian-Format (Network-Byteorder) geschrieben. Es spielt keine Rolle, ob die CPU des
 * Rechners mit Little- oder Big-Endian arbeitet.
 *
 * @param Adresse Speicheradresse, in die geschrieben werden soll
 * @param Wert Wert, der gespeichert werden soll
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
void PokeN64 (void *Adresse, ppluint64 Wert)
{
	#ifdef __BIG_ENDIAN__
		// Direktes schreiben löst bei unalignten Adressen unter Solaris einen Bus-Error aus
		//((ppluint64*)Adresse)[0]=Wert;
		((ppluint8*)Adresse)[7]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[6]=(ppluint8)((Wert >>8)&255);
		((ppluint8*)Adresse)[5]=(ppluint8)((Wert >>16)&255);
		((ppluint8*)Adresse)[4]=(ppluint8)((Wert >>24)&255);
		((ppluint8*)Adresse)[3]=(ppluint8)((Wert >>32)&255);
		((ppluint8*)Adresse)[2]=(ppluint8)((Wert >>40)&255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >>48)&255);
		((ppluint8*)Adresse)[0]=(ppluint8)((Wert >>56)&255);
	#else
		((ppluint8*)Adresse)[7]=(ppluint8)(Wert & 255);
		((ppluint8*)Adresse)[6]=(ppluint8)((Wert >>8)&255);
		((ppluint8*)Adresse)[5]=(ppluint8)((Wert >>16)&255);
		((ppluint8*)Adresse)[4]=(ppluint8)((Wert >>24)&255);
		((ppluint8*)Adresse)[3]=(ppluint8)((Wert >>32)&255);
		((ppluint8*)Adresse)[2]=(ppluint8)((Wert >>40)&255);
		((ppluint8*)Adresse)[1]=(ppluint8)((Wert >>48)&255);
		((ppluint8*)Adresse)[0]=(ppluint8)((Wert >>56)&255);
	#endif
}

/*!\brief 8-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 8 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 PeekN8 (const void *Adresse)
{
	return (ppluint32)((ppluint8*)Adresse)[0];
}

/*!\brief 16-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 16 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 PeekN16 (const void * Adresse)
{
	#ifdef __BIG_ENDIAN__
		ppluint8 wert1,wert2;
		wert1=((ppluint8*)Adresse)[1];
		wert2=((ppluint8*)Adresse)[0];
		return((ppluint32) (ppluint32)wert1+((ppluint32)wert2<<8));

	#else
		ppluint8 wert1,wert2;
		wert1=((ppluint8*)Adresse)[1];
		wert2=((ppluint8*)Adresse)[0];
		return((ppluint32) (ppluint32)wert1+((ppluint32)wert2<<8));
	#endif
}

/*!\brief 32-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 32 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint32 PeekN32 (const void *Adresse)
{
	#ifdef __BIG_ENDIAN__
		ppluint8 wert1,wert2,wert3,wert4;
		wert1=((ppluint8*)Adresse)[3];
		wert2=((ppluint8*)Adresse)[2];
		wert3=((ppluint8*)Adresse)[1];
		wert4=((ppluint8*)Adresse)[0];
		return((ppluint32) (ppluint32)wert1+((ppluint32)wert2<<8)+((ppluint32)wert3<<16)+((ppluint32)wert4<<24));

	#else
		ppluint8 wert1,wert2,wert3,wert4;
		wert1=((ppluint8*)Adresse)[3];
		wert2=((ppluint8*)Adresse)[2];
		wert3=((ppluint8*)Adresse)[1];
		wert4=((ppluint8*)Adresse)[0];
 		return((ppluint32) (ppluint32)wert1+((ppluint32)wert2<<8)+((ppluint32)wert3<<16)+((ppluint32)wert4<<24));
	#endif
}

/*!\brief 64-Bit-Wert in Network-Byteorder auslesen
 * \ingroup PPLGroupPeekPoke
 *
 * \desc
 * Die ersten 64 Bit der angegebenen Adresse werden im Big-Endian-Format (Network-Byteorder)
 * ausgelesen und als Wert zurückgegeben
 *
 * @param Adresse Speicheradresse, aus der gelesen werden soll
 * @return Ausgelesener Wert
 * @see Beschreibung von \ref PPLGroupPeekPoke
 */
ppluint64 PeekN64 (const void *Adresse)
{
	#ifdef __BIG_ENDIAN__
		ppluint8 wert1,wert2,wert3,wert4,wert5,wert6,wert7,wert8;
		wert1=((ppluint8*)Adresse)[7];
		wert2=((ppluint8*)Adresse)[6];
		wert3=((ppluint8*)Adresse)[5];
		wert4=((ppluint8*)Adresse)[4];
		wert5=((ppluint8*)Adresse)[3];
		wert6=((ppluint8*)Adresse)[2];
		wert7=((ppluint8*)Adresse)[1];
		wert8=((ppluint8*)Adresse)[0];
		return((ppluint64) (ppluint64)wert1+((ppluint64)wert2<<8)+((ppluint64)wert3<<16)+((ppluint64)wert4<<24)+((ppluint64)wert5<<32)+((ppluint64)wert6<<40)+((ppluint64)wert7<<48)+((ppluint64)wert8<<56));

	#else
		ppluint8 wert1,wert2,wert3,wert4,wert5,wert6,wert7,wert8;
		wert1=((ppluint8*)Adresse)[7];
		wert2=((ppluint8*)Adresse)[6];
		wert3=((ppluint8*)Adresse)[5];
		wert4=((ppluint8*)Adresse)[4];
		wert5=((ppluint8*)Adresse)[3];
		wert6=((ppluint8*)Adresse)[2];
		wert7=((ppluint8*)Adresse)[1];
		wert8=((ppluint8*)Adresse)[0];
 		return((ppluint64) (ppluint64)wert1+((ppluint64)wert2<<8)+((ppluint64)wert3<<16)+((ppluint64)wert4<<24)+((ppluint64)wert5<<32)+((ppluint64)wert6<<40)+((ppluint64)wert7<<48)+((ppluint64)wert8<<56));
	#endif
}


String GetArgv (int argc, char * argv[], const String &argument)
{
	if (argc>1) {
		size_t argl=strlen(argument);
		for (int i=1;i<argc;i++) {
			if (strncmp(argv[i],argument,argl) == 0) {
				size_t l=strlen(argv[i]);
				if (l>argl || argv[i+1]==NULL) {
					const char *ret=(argv[i]+argl);
					//if (ret[0]=='-') return (char*)"";
					//if (ret[0]=='\\' && ret[1]=='-') return ret+1;
					return String(ret);
				} else {
					const char * ret=(argv[i+1]);
					if (ret[0]=='-') return (char*)"";
					if (ret[0]=='\\' && ret[1]=='-') return ret+1;
					return String(ret);
				}
			}
		}
	}
	return String();
}

bool HaveArgv (int argc, char * argv[], const String &argument)
{
	if (argc>1) {
		size_t argl=strlen(argument);
		for (int i=1;i<argc;i++) {
			if (strncmp(argv[i],argument,argl) == 0) {
				size_t l=strlen(argv[i]);
				if (l>argl || argv[i+1]==NULL) {
					//const char *ret=(argv[i]+argl);
					//if (ret[0]=='-') return (char*)"";
					//if (ret[0]=='\\' && ret[1]=='-') return ret+1;
					return true;
				} else {
					const char * ret=(argv[i+1]);
					if (ret[0]=='-') return (char*)"";
					if (ret[0]=='\\' && ret[1]=='-') return ret+1;
					return true;
				}
			}
		}
	}
	return false;
}


} // EOF namespace ppl7


