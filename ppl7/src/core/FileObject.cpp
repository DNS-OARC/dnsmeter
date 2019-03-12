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

/*!\class FileObject
 * \ingroup PPLGroupFileIO
 * \brief Virtuelle Basisklasse für die Klassen File und MemFile
 *
 * \desc
 * Dies ist die virtuelle Basisklasse für die Dateizugriffsklasse File und die
 * Speicherzugriffsklasse MemFile. Sie beinhaltet Funktionen, die unabhängig vom
 * tatsächlichen Speicherort sind, sowie virtuelle Funktionen, deren eigentliche
 * Implementierung in den abgeleiteten Klassen erfolgt.
 * \par
 * FileObject ist eine allgemeine Klasse zum Zugriff auf Dateien, die sich entweder
 * auf einem Datenträger oder im Hauptspeicher des Rechners befinden können. Sie
 * bietet Funktionen, zum Öffnen, Schließen, Lesen und Schreiben, die im Wesentlichen
 * an die Funktionen der Libc angelehnt sind.
 *
 * \see
 * - File
 * - MemFile
 *
 */


/*!\enum FileObject::SeekOrigin
 * \brief Ausgangsbasis für Bewegung des Dateizeigers
 *
 * \desc
 * Diese Enumeration definiert die Ausgahgsbasis für Bewegungen des Dateizeigers
 * miitels der Funktion FileObject::seek
 *
 */

/*!\var FileObject::SeekOrigin FileObject::SEEKCUR
 * \brief Ausgehend von der aktuellen Position des Dateizeigers
 */

/*!\var FileObject::SeekOrigin FileObject::SEEKEND
 * \brief Ausgehend vom Ende der Datei
 */

/*!\var FileObject::SeekOrigin FileObject::SEEKSET
 * \brief Ausgehend vom Anfang der Datei
 */


/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Dies ist der Konstruktor der Klasse. Da es sich um eine Basisklasse handelt, die selbst fast keine
 * Funktionalität hat, gibt es hier im Augenblick nichts zu tun.
 *
 */
FileObject::FileObject()
{
	buffer=NULL;
}

/*!\brief Destruktor der Klasse
 *
 * \desc
 * Destruktor der Klasse
 */
FileObject::~FileObject()
{
	if (buffer) {
		free(buffer);
		buffer=NULL;
	}
}

/*!\brief Dateiname festlegen
 *
 * \header \#include <ppl7.h>
 * \desc
 * Mit dieser Funktion wird der interne Dateiname festgelegt, der z.B. mit
 * GetFilename ausgelesen werden kann. Die Funktion wird intern auch von den
 * Open-Funktionen mit Dateinamen aufgerufen
 *
 * \param[in] filename Ein Formatstring oder der Dateiname
 * \param[in] ... beliebig viele Parameter, sofern \p filename ein Formatstring ist
 */
void FileObject::setFilename(const char *filename)
{
	if (!filename) {
		MyFilename.clear();
		return;
	}
	MyFilename=filename;
}

/*!\brief Dateiname festlegen
 *
 * \header \#include <ppl7.h>
 * \desc
 * Mit dieser Funktion wird der interne Dateiname festgelegt, der z.B. mit
 * GetFilename ausgelesen werden kann. Die Funktion wird intern auch von den
 * Open-Funktionen mit Dateinamen aufgerufen
 *
 * \param[in] filename Ein CString, der den Dateinamen enthält.
 */
void FileObject::setFilename(const String &filename)
{
	MyFilename=filename;
}

/*!\brief Dateiname auslesen
 *
 * \header \#include <ppl7.h>
 * \desc
 * Mit dieser Funktion wird der interne Dateiname ausgelesen. Dieser wird über
 * die Open-Funktionen oder die Funktion SetFilename festgelegt.
 *
 * \return Die Funktion liefert entweder den Dateinamen zurück oder "unknown"
 */
const String& FileObject::filename() const
{
	return MyFilename;
}


/*!\brief Daten schreiben
 *
 * \desc
 * Mit dieser Funktion wird ein beliebiger Speicherbereich auf den Datenträger
 * geschrieben. Die Funktion ist nicht virtuell und existiert nur in der Basisklasse.
 * Sie ruft die virtuellen Funktionen Seek und Fwrite auf, um den
 * eigentlichen Schreibvorgang durchzuführen.
 *
 * @param source Pointer auf den Speicherbereich, der geschrieben werden soll
 * @param bytes Anzahl zu schreibender Bytes
 * @param fileposition Position in der Datei, an der die Daten gespeichert werden solle
 * @return Bei Erfolg liefert die Funktion die Anzahl geschriebener Bytes zurück, im
 * Fehlerfall wird eine Exception geworfen.
 */
size_t FileObject::write (const void * source, size_t bytes, ppluint64 fileposition)
{
	seek(fileposition);
	return fwrite(source,1,bytes);
}

/*!\brief Daten schreiben
 *
 * \desc
 * Mit dieser Funktion wird ein beliebiger Speicherbereich auf den Datenträger
 * geschrieben. Die Funktion ist nicht virtuell und existiert nur in der Basisklasse.
 * Sie ruft die virtuellen Funktionen Seek und Fwrite auf, um den
 * eigentlichen Schreibvorgang durchzuführen.
 *
 * @param source Pointer auf den Speicherbereich, der geschrieben werden soll
 * @param bytes Anzahl zu schreibender Bytes
 * @return Bei Erfolg liefert die Funktion die Anzahl geschriebener Bytes zurück, im
 * Fehlerfall wird eine Exception geworfen.
 */
size_t FileObject::write (const void * source, size_t bytes)
{
	return fwrite(source,1,bytes);
}

/*!\brief Daten eines von Variant abgeleiteten Objekts schreiben
 *
 * \desc
 * Mit dieser Funktion wird der Speicherinhalt eines Variant-Objekts auf den Datenträger
 * geschrieben. Die Funktion ist nicht virtuell und existiert nur in der Basisklasse.
 * Sie ruft die virtuellen Funktionen Seek und fwrite auf, um den
 * eigentlichen Schreibvorgang durchzuführen.
 *
 * @param object Das zu speichernde Variant Objekt
 * @param bytes Anzahl zu schreibender Bytes
 * @return Bei Erfolg liefert die Funktion die Anzahl geschriebener Bytes zurück, im
 * Fehlerfall wird eine Exception geworfen.
 *
 */
size_t FileObject::write (const ByteArrayPtr &object, size_t bytes)
{
	if (bytes==0 || bytes>object.size()) bytes=object.size();
	return fwrite(object.ptr(),1,bytes);
}


/*!\brief Daten lesen
 *
 * \desc
 * Mit dieser Funktion wird beliebiger Bereich der geöffneten Datei in den Hauptspeicher
 * geladen. Die Funktion ist nicht virtuell und existiert nur in der Basisklasse.
 * Sie ruft die virtuellen Funktionen Seek und Fread auf, um den eigentlichen
 * Lesevorgang durchzuführen.
 *
 * @param target Pointer auf den Speicherbereich, in den die gelesenen Daten geschrieben werden sollen.
 * Dieser muss zuvor vom Aufrufer allokiert worden sein und mindestens \p bytes gross sein.
 * @param bytes Anzahl zu lesender Bytes
 * @param fileposition Position in der Datei, an der die Daten gelesen werden sollen
 * @return Bei Erfolg liefert die Funktion die Anzahl gelesender Bytes zurück.
 * Wenn  ein Fehler  auftritt  oder  das
 * Dateiende erreicht ist, wird eine Exception geworfen.
 */
size_t FileObject::read (void * target, size_t bytes, ppluint64 fileposition)
{
	seek(fileposition);
	return fread(target,1,bytes);
}

/*!\brief Daten lesen
 *
 * \desc
 * Diese Funktion liest \p bytes Bytes ab der aktuellen Position des Dateistroms
 * und speichert sie im Hauptspeicher an der duch \p target bestimmten Position.
 * Die Funktion ist nicht virtuell und existiert nur in der Basisklasse.
 * Sie ruft die virtuellen Funktionen Seek und Fread auf, um den eigentlichen
 * Lesevorgang durchzuführen.
 *
 * @param target Pointer auf den Speicherbereich, in den die gelesenen Daten geschrieben werden sollen.
 * Dieser muss zuvor vom Aufrufer allokiert worden sein und mindestens \p bytes gross sein.
 * @param bytes Anzahl zu lesender Bytes
 * @return Bei Erfolg liefert die Funktion die Anzahl gelesender Bytes zurück.
 * Wenn  ein Fehler  auftritt  oder  das
 * Dateiende erreicht ist, wird eine Exception geworfen.
 */
size_t FileObject::read (void * target, size_t bytes)
{
	return fread(target,1,bytes);
}

/*!\brief Daten in ein Objekt einlesen
 *
 * \desc
 * Diese Funktion liest \p bytes Bytes ab der aktuellen Position des Dateistroms
 * und speichert sie im Objekt \p target.
 *
 * Die Funktion ist nicht virtuell und existiert nur in der Basisklasse.
 * Sie ruft die virtuellen Funktionen Seek und Fread auf, um den eigentlichen
 * Lesevorgang durchzuführen.
 *
 * @param target Das Objekt, in dem die gelesenen Daten gespeichert werden
 * sollen.
 * @param bytes Anzahl zu lesender Bytes
 * @return Bei Erfolg liefert die Funktion die Anzahl gelesender Bytes zurück.
 * Wenn  ein Fehler  auftritt  oder  das
 * Dateiende erreicht ist, wird eine Exception geworfen.
 *
 */
size_t FileObject::read (ByteArray &target, size_t bytes)
{
	if (!bytes) throw IllegalArgumentException();
	target.free();
	target.malloc(bytes+4);
	return fread((void*)target.ptr(),1,bytes);
}

/*!\brief Daten aus einer anderen Datei kopieren
 *
 * \desc
 * Mit dieser Funktion kann ein Datenblock aus einer anderen Datei in diese
 * hineinkopiert werden. Dabei werden \p bytes Bytes ab der Position \p quelloffset der
 * Quelldatei \p quellfile gelesen an die Position \p zieloffset dieser Datei kopiert.
 *
 * @param quellfile Das Dateiobjekt, aus dem gelesen werden soll
 * @param quelloffset Position innerhalb der Quelldatei, ab der die Daten gelesen werden sollen
 * @param bytes Anzahl zu kopierender Bytes
 * @param zieloffset Position in dieser Datei, an die die Daten geschrieben werden sollen
 * @return Bei Erfolg liefert die Funktion die Anzahl kopierter Bytes zurück.
 * Im Fehlerfall wird eine Exception geworfen.
 *
 * \note Die Funktion verwendet einen internen Buffer zum Zwischenspeichern
 * der gelesenen Daten.
 */
ppluint64 FileObject::copyFrom (FileObject &quellfile, ppluint64 quelloffset, ppluint64 bytes, ppluint64 zieloffset)
{
	quellfile.seek(quelloffset);
	seek(zieloffset);
	return FileObject::copyFrom(quellfile,bytes);
}

/*!\brief Daten aus einer anderen Datei kopieren
 *
 * \desc
 * Mit dieser Funktion kann ein Datenblock aus einer anderen Datei in diese
 * hineinkopiert werden. Die Daten werden dabei ab dem aktuellen Dateipositionszeiger
 * des \p quellfile an den aktuellen Zeiger dieser Datei kopiert.
 *
 * @param quellfile Das Dateiobjekt, aus dem gelesen werden soll
 * @param bytes Anzahl zu kopierender Bytes
 * @return Bei Erfolg liefert die Funktion die Anzahl kopierter Bytes zurück.
 * Im Fehlerfall wird eine Exception geworfen.
 *
 * \note Die Funktion verwendet einen internen Buffer zum Zwischenspeichern
 * der gelesenen Daten.
 */
ppluint64 FileObject::copyFrom (FileObject &quellfile, ppluint64 bytes)
{
	if (buffer==NULL) {
		buffer=(char *)malloc(COPYBYTES_BUFFERSIZE);
		if (buffer==NULL) throw OutOfMemoryException();
	}
	if (quellfile.size()>quellfile.tell()) {
		if ((quellfile.tell()+(ppluint64)bytes)>quellfile.size()) {
			bytes=quellfile.size()-quellfile.tell();
		}
		ppluint64 rest=bytes;
		while (rest>0) {
			ppluint64 by=rest;
			if (by>COPYBYTES_BUFFERSIZE) by=COPYBYTES_BUFFERSIZE;
			by=quellfile.read (buffer,(size_t)by);
			write (buffer,(size_t)by);
			rest-=by;
		}
	}
	return bytes;
}

/*!\brief String lesen
 *
 * \desc
 * Gets liest höchstens \p num minus ein Zeichen aus der Datei und speichert
 * sie im String-Objekt \p buffer. Das Lesen stoppt nach einem
 * EOF oder Zeilenvorschub. Wenn ein Zeilenvorschub gelesen wird, wird
 * er in dem Puffer gespeichert. Am Ende der gelesenen Daten wird ein
 * 0-Byte angehangen.
 *
 * @param buffer String-Objekt, in dem die gelesenen Daten gespeichert werden
 * sollen.
 * @param num Anzahl zu lesender Zeichen
 * @return Bei Erfolg wird 1 zurückgegeben, bei Erreichen des Dateiende 0.
 * Im Fehlerfall wird eine Exception geworfen.
 * Der Inhalt von \p buffer ist im Fehlerfall undefiniert.
 */
int FileObject::gets (String &buffer, size_t num)
{
	if (!num) throw IllegalArgumentException();
	char *b=(char*)malloc(num+1);
	if (!b) throw OutOfMemoryException();
	char *ret;
	try {
		ret=fgets(b,num);
	} catch (...) {
		free(b);
		throw;
	}
	if (ret==NULL) {
		free(b);
		return 0;
	}
	buffer.set(b);
	free(b);
	return 1;
}

/*!\brief String lesen
 *
 * \desc
 * Gets liest höchstens \p num minus ein Zeichen aus der Datei und liefert deren
 * Inhalt als String-Objekt zurück. Das Lesen stoppt nach einem
 * EOF oder Zeilenvorschub. Wenn ein Zeilenvorschub gelesen wird, wird
 * er in dem Puffer gespeichert. Am Ende der gelesenen Daten wird ein
 * 0-Byte angehangen.
 *
 * @param num Anzahl zu lesender Zeichen
 * @return Die Funktion gibt ein String-Objekt mit den gelesenen Daten zurück.
 * Im Fehlerfall (auch bei Dateiende) wird eine Exception geworfen.
 */
String FileObject::gets (size_t num)
{
	String s;
	if (!gets(s,num)) throw EndOfFileException();
	return s;
}


/*!\brief Wide-Character String lesen
 *
 * \desc
 * Gets liest höchstens \p num minus ein Zeichen aus der Datei und speichert
 * sie im Wide-Character-String-Objekt \p buffer. Das Lesen stoppt nach einem
 * WEOF oder Zeilenvorschub. Wenn ein Zeilenvorschub gelesen wird, wird
 * er in dem Puffer gespeichert. Am Ende der gelesenen Daten wird ein
 * 0-Byte angehangen.
 *
 * @param buffer Wide-Character-String-Objekt, in dem die gelesenen Daten gespeichert werden
 * sollen.
 * @param num Anzahl zu lesender Zeichen. Hierbei handelt es sich tatsächlich um
 * Zeichen, nicht um Bytes. Die Anzahl zu lesender Bytes wird intern mit der Formel
 * \p num * \c sizeof(wchar_t) errechnet.
 * @return Bei Erfolg wird 1 zurückgegeben, bei Erreichen des Dateiende 0.
 * Im Fehlerfall wird eine Exception geworfen.
 * Der Inhalt von \p buffer ist im Fehlerfall undefiniert.
 */
int FileObject::getws (String &buffer, size_t num)
{
	if (!num) throw IllegalArgumentException();
	wchar_t *b=(wchar_t*)malloc((num+1)*sizeof(wchar_t));
	if (!b) throw OutOfMemoryException();
	wchar_t *ret;
	try {
		ret=fgetws(b,num);
	} catch (...) {
		free(b);
		throw;
	}
	if (ret==NULL) {
		free(b);
		return 0;
	}
	buffer.set(b);
	free(b);
	return 1;
}

/*!\brief Wide-Character String lesen
 *
 * \desc
 * Gets liest höchstens \p num minus ein Zeichen aus der Datei liefert sie als
 * Wide-Character-String-Objekt zurück. Das Lesen stoppt nach einem
 * WEOF oder Zeilenvorschub. Wenn ein Zeilenvorschub gelesen wird, wird
 * er in dem Puffer gespeichert. Am Ende der gelesenen Daten wird ein
 * 0-Byte angehangen.
 *
 * @param num Anzahl zu lesender Zeichen. Hierbei handelt es sich tatsächlich um
 * Zeichen, nicht um Bytes. Die Anzahl zu lesender Bytes wird intern mit der Formel
 * \p num * \c sizeof(wchar_t) errechnet.
 * @return Die Funktion gibt ein String-Objekt mit den gelesenen Daten zurück.
 * Im Fehlerfall (auch bei Dateiende) wird eine Exception geworfen.
 */
String FileObject::getws (size_t num)
{
	String s;
	if (!getws(s,num)) throw EndOfFileException();
	return s;
}


/*!\brief Formatierten String schreiben
 *
 * \desc
 * Putsf schreibt das Ergebnis nach Kontrolle des Formatierungsstrings \p fmt und
 * Einsetzen der optionalen Parameter ohne sein nachfolgendes 0-Byte in den Ausgabestrom.
 *
 * @param fmt Pointer auf den Formatierungsstring
 * @param ... Optionale Parameter, die im Formatierungsstring verwendet werden.
 * @return Kein Rückgabeparameter, im Fehlerfall wirft die Funktion eine Exception
 */
void FileObject::putsf (const char *fmt, ... )
{
	if (!fmt) throw IllegalArgumentException();
	String str;
	va_list args;
	va_start(args, fmt);
	str.vasprintf(fmt, args);
	va_end(args);
	fputs(str);
}

/*!\brief String schreiben
 *
 * \desc
 * Diese Funktion schreibt den Inhalt des String-Objekts \p str
 * ohne sein nachfolgendes 0-Byte in den Ausgabestrom.
 *
 * @param str String-Objekt mit den zu schreibenden Daten
 * @return Kein Rückgabeparameter, im Fehlerfall wirft die Funktion eine Exception
 */
void FileObject::puts (const String &str)
{
	return fputs((const char*)str);
}

/*!\brief Wide-Character-String schreiben
 *
 * \desc
 * Diese Funktion schreibt den Inhalt des Wide-Character-String-Objekts \p str
 * ohne sein nachfolgendes 0-Byte in den Ausgabestrom.
 *
 * @param str String-Objekt mit den zu schreibenden Daten
 * @return Kein Rückgabeparameter, im Fehlerfall wirft die Funktion eine Exception
 */
void FileObject::putws (const WideString &str)
{
	return fputws((const wchar_t*)str);
}

/*!\brief Datei in den Speicher mappen
 *
 * \desc
 * Mit dieser Funktion wird der komplette Inhalt der Datei in den Speicher gemapped.
 * Falls das Betriebssystem <a href="http://en.wikipedia.org/wiki/Mmap">mmap</a> versteht,
 * wird dieser verwendet. Dabei wird die Datei nicht sofort komplett in den Speicher
 * geladen, sondern nur die Teile, die gerade benötigt werden. Steht \c mmap nicht
 * zur Verfügung, wird die Datei in den Hauptspeicher geladen. Die File-Klasse kümmert
 * sich selbst daraum, dass der Speicher auch wieder freigegeben wird.
 * \par
 * Ein mit Map gemappter Speicher darf nur gelesen, aber nicht beschrieben werden!
 *
 * @return Bei Erfolg gibt die Funktion einen Pointer auf den Speicherbereich zurück,
 * in dem sich die Datei befindet, im Fehlerfall wirft die Funktion eine Exception
 */
const char *FileObject::map()
{
	return map(0,(size_t)size());
}

/*!\brief Den kompletten Inhalt der Datei laden
 *
 * \desc
 * Mit dieser Funktion wird der komplette Inhalt der geöffneten Datei in den
 * Hauptspeicher geladen. Der benötigte Speicher wird von der Funktion
 * automatisch allokiert und muss vom Aufrufer nach Gebrauch mit \c free wieder
 * freigegeben werden.
 *
 * @return Pointer auf den Speicherbereich mit dem Inhalt der Datei. Dieser muss
 * vom Aufrufer nach Gebrauch mit \c free selbst wieder freigegeben werden.
 * Im Fehlerfall wird eine Exception geworfen.
 */
char *FileObject::load()
{
	ppluint64 s=size();
	char *b=(char*)malloc((size_t)s+1);
	if (!b) throw OutOfMemoryException();
	ppluint64 r=0;
	try {
		r=read(b,(size_t)s,0);
	} catch (...) {
		free(b);
		throw;
	}
	if (r!=s) {
		free(b);
		return NULL;
	}
	b[s]=0;
	return b;
}

/*!\brief Den kompletten Inhalt der Datei in ein Objekt laden
 *
 * \desc
 * Mit dieser Funktion wird der komplette Inhalt der geöffneten Datei in das
 * angegebene ByteArray \p object geladen.
 *
 * @param[out] object Das gewünschte Zielobjekt
 * @return Liefert 1 zurück, wenn der Inhalt geladen werden konnte, sonst 0.
 */
int FileObject::load(ByteArray &object)
{
	if (!isOpen()) throw FileNotOpenException();
	ppluint64 mysize=size();
	seek(0);
	char *buffer=(char*)malloc((size_t)mysize+1);
	if (!buffer) throw OutOfMemoryException();
	size_t by=0;
	try {
		by=fread(buffer,1,(size_t)mysize);
	} catch (...) {
		free(buffer);
		throw;
	}
	if (by!=mysize) {
		free(buffer);
		return 0;
	}
	buffer[by]=0;
	object.clear();
	object.use(buffer,mysize);
	return 1;
}


// Virtuelle Funktionen

/*!\brief Datei schließen
 *
 * \desc
 * Diese Funktion schließt die aktuell geöffnete Datei. Sie wird automatisch vom Destruktor der
 * Klasse aufgerufen, so dass ihr expliziter Aufruf nicht erforderlich ist.
 * \par
 * Wenn  der  Stream  zur  Ausgabe  eingerichtet  war,  werden  gepufferte  Daten  zuerst  durch FileObject::Flush
 * geschrieben. Der zugeordnete Datei-Deskriptor wird geschlossen.
 *
 */
void FileObject::close ()
{
	throw UnimplementedVirtualFunctionException();
}


/*!\brief Dateizeiger an den Anfang der Datei bringen
 *
 * \desc
 * Diese Funktion bewegt den internen Dateizeiger an den Anfang der Datei
 *
 */
void FileObject::rewind ()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Dateizeiger auf gewünschte Stelle bringen
 *
 * \desc
 * Diese Funktion bewegt den internen Dateizeiger auf die gewünschte Stelle
 *
 * \param[in] position Gewünschte Position innerhalb der Datei
 * \exception diverse
 */
void FileObject::seek (ppluint64 position)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Dateizeiger auf gewünschte Stelle bringen
 *
 * \desc
 * Die Funktion %seek setzt den Dateipositionszeiger für den Stream. Die neue Position,
 * gemessen in Byte, wird erreicht durch addieren von  \p offset  zu  der  Position,  die  durch  \p origin
 * angegeben  ist. Wenn \p origin auf SEEK_SET, SEEK_CUR, oder SEEK_END, gesetzt ist, ist der Offset relativ
 * zum Dateianfang, der aktuellen Position, oder dem Dateiende.
 *
 * Ein  erfolgreicher  Aufruf  der  Funktion fseek  löscht  den Dateiendezeiger für den Stream.
 *
 * @param offset Anzahl Bytes, die gesprungen werden soll.
 * @param origin Gibt die Richtung an, in die der Dateizeiger bewegt werden soll. Es kann einen
 * der folgenden Werte annehmen:
 * - SEEKSET \p offset wird vom Beginn der Datei berechnet
 * - SEEKCUR \p offset wird von der aktuellen Dateiposition gerechnet
 * - SEEKEND \p offset wird vom Ende der Datei aus nach vorne berechnet
 * \return Liefert die neue Position zurück, wenn der Dateizeiger erfolgreich auf
 * die gewünschte Position bewegt werden konnte.
 * Im Fehlerfall wird eine Exception geworfen. Die Position des Schreib-/Lesezeigers
 * ist in diesem Fall undefiniert und sollte mittels FileObject::ftell verifiziert
 * werden.
 */
ppluint64 FileObject::seek (pplint64 offset, SeekOrigin origin)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Aktuelle Dateiposition ermitteln
 *
 * \desc
 * Die Funktion %tell liefert den aktuellen Wert des Dateipositionszeigers für  den  Stream zurück.
 *
 * @return Position des Zeigers innerhalb der Datei. Im Fehlerfall wird eine
 * Exception geworfen
 */
ppluint64 FileObject::tell()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Lesen eines Datenstroms
 *
 * \desc
 * Die  Funktion  %fread  liest \p nmemb Datenelemente vom Dateistrom und speichert
 * es an  der  Speicherposition,  die  durch \p ptr bestimmt ist.  Jedes davon ist
 * \ size Byte lang.
 *
 * @param[out] ptr Pointer auf den Speicherbereich, in den die gelesenen Daten
 * abgelegt werden sollen. Der Aufrufer muss vorher mindestens \p size * \p nmemb
 * Bytes Speicher reserviert haben.
 * @param[in] size Größe der zu lesenden Datenelemente
 * @param[in] nmemb Anzahl zu lesender Datenelemente
 * @return %fread  gibt die Anzahl der erfolgreich gelesenen Elemente zurück
 * (nicht die Anzahl  der  Zeichen).  Wenn  ein Fehler  auftritt  oder  das
 * Dateiende erreicht ist, wird eine Exception geworfen.
 * \exception EndOfFileException: Wird geworfen, wenn das Dateiende erreicht wurde
 */
size_t FileObject::fread(void * ptr, size_t size, size_t nmemb)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Schreiben eines Datenstroms
 *
 * \desc
 * Die Funktion %fwrite schreibt \p nmemb Datenelemente der Größe \p size Bytes,
 * in  den  Dateistrom. Sie werden von der Speicherstelle, die durch \p ptr angegeben ist, gelesen.
 *
 * @param ptr Pointer auf den Beginn des zu schreibenden Speicherbereiches.
 * @param size Größe der zu schreibenden Datenelemente
 * @param nmemb Anzahl zu schreibender Datenelemente
 * @return %fwrite gibt die Anzahl der erfolgreich geschriebenen Elemente zurück (nicht die
 * Anzahl der Zeichen). Wenn ein Fehler auftritt, wird eine Exception geworfen.
 *
 */
size_t FileObject::fwrite(const void * ptr, size_t size, size_t nmemb)
{
	throw UnimplementedVirtualFunctionException();
}


/*!\brief String lesen
 *
 * \desc
 * %fgets liest höchstens \p num minus ein Zeichen aus der Datei und speichert
 * sie in dem Puffer, auf den \p buffer zeigt. Das Lesen stoppt nach einem
 * EOF oder Zeilenvorschub. Wenn ein Zeilenvorschub gelesen wird, wird
 * er in dem Puffer gespeichert. Am Ende der gelesenen Daten wird ein
 * 0-Byte angehangen.
 *
 *
 * @param buffer Pointer auf den Speicherbereich, in den die gelesenen Daten
 * geschrieben werden sollen. Dieser muss vorher vom Aufrufer allokiert worden
 * sein und mindestens \p num Bytes groß sein.
 * @param num Anzahl zu lesender Zeichen
 * @return Bei Erfolg wird \p buffer zurückgegeben, bei Dateiende wird NULL
 * zurückgegeben. Im Fehlerfall wird eine Exception geworfen.
 */
char *FileObject::fgets (char *buffer, size_t num)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Wide-Character String lesen
 *
 * \desc
 * %fgwets liest höchstens \p num minus ein Zeichen (nicht Bytes)
 * eines Wide-Character-Strings aus der Datei
 * und speichert sie in dem Puffer, auf den \p buffer zeigt. Das Lesen stoppt
 * nach einem EOF oder Zeilenvorschub. Wenn ein Zeilenvorschub gelesen wird,
 * wird er in dem Puffer gespeichert. Am Ende der gelesenen Daten wird ein
 * 0-Byte angehangen.
 *
 * @param buffer Pointer auf den Speicherbereich, in den die gelesenen Daten
 * geschrieben werden sollen. Dieser muss vorher vom Aufrufer allokiert worden
 * sein und mindestens \p num * \c sizeof(wchar_t) Bytes groß sein.
 * @param num Anzahl zu lesender Zeichen
 * @return Bei Erfolg wird \p buffer zurückgegeben, bei Dateiende wird NULL
 * zurückgegeben. Im Fehlerfall wird eine Exception geworfen.
 *
 * \note Die Funktion ist unter Umständen nicht auf jedem Betriebssystem
 * verfügbar. In diesem Fall wird eine \exception UnimplementedVirtualFunctionException
 * geworfen.
 */
wchar_t *FileObject::fgetws (wchar_t *buffer, size_t num)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief String schreiben
 *
 * \desc
 * %fputs schreibt die Zeichenkette \p str ohne sein nachfolgendes 0-Byte in
 * den Ausgabestrom.
 *
 * @param str Pointer auf den zu schreibenden String
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 */
void FileObject::fputs (const char *str)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Wide-Character String schreiben
 *
 * \desc
 * %fputs schreibt die Zeichenkette \p str ohne sein nachfolgendes 0-Byte in
 * den Ausgabestrom.
 *
 * @param str Pointer auf den zu schreibenden String
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 *
 * \note Die Funktion ist unter Umständen nicht auf jedem Betriebssystem
 * verfügbar. In diesem Fall wird Fehlercode 246 zurückgegeben.
 *
 */
void FileObject::fputws (const wchar_t *str)
{
	throw UnimplementedVirtualFunctionException();
}


/*!\brief Zeichen schreiben
 *
 * \desc
 * %fputc schreibt das Zeichen \p c, umgesetzt in ein unsigned char,
 * in den Ausgabestrom.
 * @param c Zu schreibendes Zeichen
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 */
void FileObject::fputc (int c)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Zeichen lesen
 *
 * \desc
 * %fgetc liest das  nächste Zeichen aus der Datei und gibt seinen unsigned char Wert gecastet
 * in einem int zurück.
 * @return Bei Erfolg wird der Wert des gelesenen Zeichens zurückgegeben, im
 * Fehlerfall wird eine Exception geworfen.
 */
int FileObject::fgetc()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Wide-Character Zeichen schreiben
 *
 * \desc
 * %fputwc schreibt das Wide-Character Zeichen \p c in den Ausgabestrom.
 * @param c Zu schreibendes Zeichen
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 *
 * \note Die Funktion ist unter Umständen nicht auf jedem Betriebssystem
 * verfügbar.
 */
void FileObject::fputwc (wchar_t c)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Wide-Character Zeichen lesen
 *
 * \desc
 * %fgetwc liest das nächste Wide-Character Zeichen aus der Datei und gibt seinen Wert als Integer
 * zurück.
 * @return Bei Erfolg wird das gelesene Zeichen als Integer Wert zurückgegeben,
 * im Fehlerfall wird eine Exception geworfen.
 *
 * \note Die Funktion ist unter Umständen nicht auf jedem Betriebssystem
 * verfügbar.
 */
wchar_t FileObject::fgetwc()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Prüfen, ob Dateiende erreicht ist
 *
 * \desc
 * Die Funktion prüft, ob das Dateiende erreicht wurde
 *
 * @return Liefert \c true zurück, wenn das Dateiende erreicht wurde, sonst \c false
 * Falls die Datei nicht geöffnet war, wird wird eine Exception geworfen.
 */
bool FileObject::eof() const
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Größe der geöffneten Datei
 *
 * \desc
 * Diese Funktion liefert die Größe der geöffneten Datei in Bytes zurück.
 * \return Größe der Datei in Bytes. Falls Fehler auftreten, wird eine Exception geworfen.
 *
 */
ppluint64 FileObject::size() const
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Filenummer der Datei
 *
 * \desc
 * Die Funktion liefert den Dateideskriptor als Integer zurück, wie er
 * von den Systemfunktionen open , read , write und close genutzt wird.
 *
 * @return Liefert die Filenummer zurück oder wirft eine Exception,
 * wenn die Datei nicht geöffnet war.
 */
int FileObject::getFileNo() const
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Gepufferte Daten schreiben
 *
 * \desc
 * Die Funktion Flush bewirkt, dass alle gepufferten Daten des aktuellen Streams
 * mittels der zugrundeliegenden write-Funktion geschrieben werden. Der Status
 * des Streams wird dabei nicht berührt. Die Daten werden nicht zwangsweise auch
 * physikalisch auf die Platte geschrieben, sie können noch immer aus Performancegründen
 * vom Kernel oder Treiber gecached werden. Um 100 Prozent sicher zu gehen, kann man
 * die Funktion FileObject::sync verwenden.
 *
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 */
void FileObject::flush()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Dateiänderungen sofort auf die Platte schreiben
 *
 * \desc
 * Für gewöhnlich cached das Betriebssysteme Schreibzugriffe auf die Festplatte, um die Performance
 * zu steigern. Je nach Filesystem und Betriebssystem können zwischen Schreibzugriff der Software bis zum
 * tatsächlichen Schreiben auf die Festplatte zwischen einigen wenigen Sekunden bis zu einer Minute vergehen!
 * Tritt in diesem Zeitraum ein System-Crash oder Stromausfall auf, führt dies unweigerlich zu Datenverlust.
 *
 * Ein Aufruf dieser Funktion bewirkt, dass alle Dateiänderungen sofort auf die Festplatte
 * geschrieben werden. Sie sollte daher vor dem Schließen einer kritischen Datei mit CFile::Close aufgerufen
 * werden, unter Umständen sogar nach jedem Schreibzugriff.
 *
 * @return Die Funktion kehrt erst zurück, wenn alle Daten vollständig geschrieben wurden und liefert dann true (1)
 * zurück. Können die Daten nicht geschrieben werden, wird eine Exception
 * geworfen.
 */
void FileObject::sync()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Datei abschneiden
 *
 * \desc
 * Die Funktionen Truncate bewirkt, dass die aktuell geöffnete Datei auf eine Größe von
 * exakt \p length Bytes abgeschnitten wird.
 * \par
 * Wenn die Datei vorher größer war, gehen überschüssige Daten verloren. Wenn die Datei
 * vorher kleiner war, wird sie vergrößert und die zusätzlichen Bytes werden als Nullen geschrieben.
 * \par
 * Der Dateizeiger wird nicht verändert. Die Datei muss zum Schreiben geöffnet sein.
 *
 * @param length Position, an der die Datei abgeschnitten werden soll.
 * \return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 */
void FileObject::truncate(ppluint64 length)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Prüfen, ob eine Datei geöffnet ist
 *
 * \header \#include <ppl7.h>
 * \desc
 * Mit dieser Funktion kann geprüft werden, ob die mit diesem Objekt
 * assoziierte Datei gerade geöffnet ist.
 * \return Die Funktion liefert \p true zurück, wenn die Datei offen ist, ansonsten \p false.
 */
bool FileObject::isOpen() const
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Datei zum Lesen sperren
 *
 * \desc
 * Mit LockShared wird die Datei zum Lesen gesperrt. Andere Prozesse können weiterhin
 * auf die Datei zugreifen, allerdings ebenfalls nur lesend.
 *
 * @param block Gibt an, ob die Funktion warten soll (blocken), bis die Datei
 * gesperrt werden kann (block=true) oder sofort mit einer Fehlermeldung
 * zurückkehren soll (block=false).
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 *
 * \see Siehe auch File::LockExclusive und File::Unlock
 */
void FileObject::lockShared(bool block)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Datei exklusiv sperren
 *
 * \desc
 * Mit LockExclusive wird die Datei exklusiv zum Schreiben gesperrt. Andere
 * Prozesse können nicht auf die Datei zugreifen, solange die Sperre besteht.
 *
 * @param block Gibt an, ob die Funktion warten soll (blocken), bis die Datei
 * gesperrt werden kann (block=true) oder sofort mit einer Fehlermeldung
 * zurückkehren soll (block=false).
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 *
* \see Siehe auch File::LockShared und File::Unlock
*/
void FileObject::lockExclusive(bool block)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Dateisperre aufheben
 *
 * \desc
 * Mit Unlock wird eine mit lockShared oder lockExclusive eingerichtete
 * Sperre wieder aufgehoben, so dass auch andere Prozesse wieder uneingeschränkt
 * auf die Datei zugreifen können.
 *
 * @return Kein Rückgabewert, im Fehlerfall wird eine Exception geworfen.
 *
 * \see Siehe auch FileObject::lockShared und FileObject::lockExclusive
 */
void FileObject::unlock()
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Minimalgröße des Speicherblocks bei Zugriffen mit FileObject::Map
 *
 * \desc
 * Falls mit Map viele aufeinanderfolgende kleine Speicherblöcke gemapped werden,
 * ist es sinnvoll größere Blöcke zu laden, die dann bereits im Cache bzw. Hauptspeicher
 * liegen, wenn sie gebraucht werden. Mit dieser Funktion kann bestimmt werden, wie
 * viele Daten im Voraus gemapped werden sollen.
 *
 * @param bytes Anzahl Bytes, die im Voraus gemapped werden sollen.
 */
void FileObject::setMapReadAhead(size_t bytes)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Datei Read-Only in den Speicher mappen
 *
 * \desc
 * Mit dieser Funktion wird ein Teil der Datei in den Speicher gemapped.
 * Falls das Betriebssystem <a href="http://en.wikipedia.org/wiki/Mmap">mmap</a> versteht,
 * wird dieser verwendet. Dabei wird der gewünschte Datenblock nicht sofort komplett
 * in den Speicher geladen, sondern nur der Teil, auf den gerade zugegriffen wird.
 * Steht \c mmap nicht zur Verfügung, wird die Datei in den Hauptspeicher geladen.
 * Die File-Klasse kümmert sich selbst daraum, dass der Speicher auch wieder freigegeben
 * wird.
 * \par
 * Ein mit Map gemappter Speicher darf nur gelesen, aber nicht beschrieben werden! Falls
 * auch geschrieben werden soll, ist die Funktion FileObject::MapRW zu verwenden.
 *
 * @param[in] position Die gewünschte Startposition innerhalb der Datei
 * @param[in] size Die Anzahl Bytes, die gemapped werden sollen.
 * @return Bei Erfolg gibt die Funktion einen Pointer auf den Speicherbereich zurück,
 * in dem sich die Datei befindet, im Fehlerfall wird eine Exception geworfen.
 */
const char *FileObject::map(ppluint64 position, size_t size)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Datei Les- und Schreibbar in den Speicher mappen
 *
 * \desc
 * Mit dieser Funktion wird ein Teil der Datei in den Speicher gemapped.
 * Falls das Betriebssystem <a href="http://en.wikipedia.org/wiki/Mmap">mmap</a> versteht,
 * wird dieser verwendet. Dabei wird der gewünschte Datenblock nicht sofort komplett
 * in den Speicher geladen, sondern nur der Teil, auf den gerade zugegriffen wird.
 * Steht \c mmap nicht zur Verfügung, wird die Datei in den Hauptspeicher geladen.
 * Die File-Klasse kümmert sich selbst daraum, dass der Speicher nach Gebrauch wieder
 * zurück in die Datei geschrieben und freigegeben wird.
 * \par
 * Ein mit %mapRW gemappter Speicher darf sowohl gelesen als auch beschrieben werden!
 * Bevor mit anderen Funktionen auf den gleichen Speicher zugegriffen werden soll
 * (insbesondere schreibend), muss die Funktion FileObject::Unmap aufgerufen werden.
 *
 * @param[in] position Die gewünschte Startposition innerhalb der Datei
 * @param[in] size Die Anzahl Bytes, die gemapped werden sollen.
 * @return Bei Erfolg gibt die Funktion einen Pointer auf den Speicherbereich zurück,
 * in dem sich die Datei befindet, im Fehlerfall wird eine Exception geworfen.
 */
char *FileObject::mapRW(ppluint64 position, size_t size)
{
	throw UnimplementedVirtualFunctionException();
}

/*!\brief Mapping aufheben
 *
 * \desc
 * Ein mit map oder mapRW eingerichtetes Mapping einer Datei in den Hauptspeicher
 * wird wieder aufgehoben.
 *
 */
void FileObject::unmap()
{
	throw UnimplementedVirtualFunctionException();
}


} // end of namespace ppl7
