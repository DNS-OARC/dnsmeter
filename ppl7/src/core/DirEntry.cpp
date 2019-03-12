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
#ifdef HAVE_WCHAR_H
	#include <wchar.h>
#endif
#include <time.h>
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef HAVE_SYS_MMAN_H
	#include <sys/mman.h>
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
#include <windows.h>

#endif
#include "ppl7.h"

namespace ppl7 {


/*!\class DirEntry
 * \ingroup PPLGroupFileIO
 * \brief Dateiverzeichnis-Eintrag
 *
 *
 * Die Klasse DirEntry wird zum Abbilden von Dateien innerhalb eines Verzeichnisses
 * verwendet und ist die Basisklasse für alle Directory-Funktionen. Die Klasse CDir
 * setzt darauf auf.
 */


/*!\var DirEntry::Filename
 * \brief Beinhaltet den Dateinamen
 *
 * Diese Variable beinhaltet nur den Dateinamen, ohne Pfadangaben
 */

/*!\var DirEntry::File
 * \brief Beinhaltet Pfad und Dateinamen
 *
 * Diese Variable beinhaltet den vollständigen Pfad und Dateinamen.
 */

/*!\var DirEntry::Path
 * \brief Beinhaltet nur den Pfad
 *
 * Diese Variable beinhaltet den vollständigen Pfad, aber nicht den Dateinamen.
 */

/*!\var DirEntry::Size
 * \brief Größe der Datei in Bytes
 *
 * Größe der Datei in Bytes als vorzeichenloser 64-Bit-Wert (ppluint64).
 */

/*!\var DirEntry::Attrib
 * \brief Attribute der Datei
 *
 * Enthält die Dateiattribute:
 * - CPPLDIR_NORMAL
 * - CPPLDIR_READONLY
 * - CPPLDIR_DIR
 * - CPPLDIR_LINK
 * - CPPLDIR_HIDDEN
 * - CPPLDIR_SYSTEM
 * - CPPLDIR_ARCHIV
 * - CPPLDIR_FILE
 *
 * Die Attribute können auch einzeln mit den Memberfunktionen IsDir, IsFile,
 * IsLink, IsHidden, IsReadOnly, IsArchiv und IsSystem abgefragt werden.
 *
 */

/*!\var DirEntry::AttrStr
 * \brief Lesbare Darstellung der Datei-Attribute und Rechte
 *
 * Dieser String enthält eine lesbare Darstellung der Datei-Attribute und
 * Zugriffsrechte, wie man es vom UNIX-Befehl "ls -l" kennt.
 */

/*!\var DirEntry::ATime
 * \brief Zeit des letzten Dateizugriffs
 *
 * Enthält das Datum des letzten Dateizugriffs in UNIX-Time.
 */

/*!\var DirEntry::CTime
 * \brief Zeit der Datei-Erstellung
 *
 * Enthält das Datum der Erstellung der Datei in UNIX-Time.
 */

/*!\var DirEntry::MTime
 * \brief Zeit der letzten Modifizierung
 *
 * Enthält das Datum der letzten Modifizierung der Datei in UNIX-Time.
 */



/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Dies ist der Konstruktor der Klasse.
 * \par
 * Hier werden alle Variablen der Klasse in den Ausgangszustand gebracht. Die
 * Strings werden geleert, alle Integer-Variablen werden auf 0 gesetzt. Da die
 * Klasse selbst keinen Speicher allokiert, gibt es keinen Destruktor.
 */
DirEntry::DirEntry()
{
	Size=0;
	Attrib=FileAttr::NONE;
	Uid=0;
	Gid=0;
	Blocks=0;
	BlockSize=0;
	NumLinks=0;
}

/*!\brief Copy-Konstruktor der Klasse
 *
 * \desc
 * Dies ist der Copy-Konstruktor der Klasse. Alle Daten werden von \p other übernommen.
 */
DirEntry::DirEntry(const DirEntry& other)
{
	Filename=other.Filename;
	Path=other.Path;
	File=other.File;
	Size=other.Size;
	Attrib=other.Attrib;
	Uid=other.Uid;
	Gid=other.Gid;
	Blocks=other.Blocks;
	BlockSize=other.BlockSize;
	NumLinks=other.NumLinks;
	AttrStr=other.AttrStr;
	ATime=other.ATime;
	CTime=other.CTime;
	MTime=other.MTime;
}


/*!\brief Kopieroperator
 *
 * \desc
 * Kopiert den Inhalt der anderen DirEntry-Instanz \p other.
 */
DirEntry& DirEntry::operator=(const DirEntry& other)
{
	Filename=other.Filename;
	Path=other.Path;
	File=other.File;
	Size=other.Size;
	Attrib=other.Attrib;
	Uid=other.Uid;
	Gid=other.Gid;
	Blocks=other.Blocks;
	BlockSize=other.BlockSize;
	NumLinks=other.NumLinks;
	AttrStr=other.AttrStr;
	ATime=other.ATime;
	CTime=other.CTime;
	MTime=other.MTime;
	return *this;
}



bool DirEntry::isDir()
/*!\brief Ist aktueller Eintrag ein Verzeichnis?
 *
 * Die Funktion prüft, ob der aktuelle Directory-Eintrag ein Verzeichnis ist.
 *
 * \return Liefert true (1) oder false (0) zurück.
 */
{
	if (Attrib&FileAttr::IFDIR) return true;
	return false;
}

bool DirEntry::isFile()
/*!\brief Ist aktueller Eintrag eine Datei?
 *
 * Die Funktion prüft, ob der aktuelle Directory-Eintrag eine Datei ist.
 *
 * \return Liefert true (1) oder false (0) zurück.
 */
{
	if (Attrib&FileAttr::IFFILE) return true;
	return 0;
}

/*!\brief Ist aktueller Eintrag ein Link?
 *
 * Die Funktion prüft, ob der aktuelle Directory-Eintrag ein Link ist.
 *
 * \return Liefert true (1) oder false (0) zurück.
 */
bool DirEntry::isLink()
{
	if (Attrib&FileAttr::IFLINK) return true;
	return 0;
}

bool DirEntry::isReadable()
{
	if (Attrib&FileAttr::USR_READ) return true;
	return 0;
}

bool DirEntry::isWritable()
{
	if (Attrib&FileAttr::USR_WRITE) return true;
	return 0;
}

bool DirEntry::isExecutable()
{
	if (Attrib&FileAttr::USR_EXECUTE) return true;
	return 0;
}



void DirEntry::toArray(AssocArray &a) const
/*!\brief Eintrag in einem Array speichern
 *
 * Mit dieser Funktion können alle Informationen des aktuellen Verzeichnis-Eintrags
 * in einem Assoziativen Array gespeichert werden.
 *
 * \param a Pointer auf das Assoziative Array, in dem die Daten gespeichert werden
 * sollen.
 *
 * \return Liefert true (1) zurück, wenn die Daten erfolgreich gespeichert wurden,
 * ansonsten false (0).
 */
{
	a.set("filename",Filename);
	a.set("path",Path);
	a.set("file",File);
	a.setf("size","%llu",Size);
	a.setf("attrib","%u",Attrib);
	a.set("attribstr",AttrStr);
	a.setf("uid","%u",Uid);
	a.setf("gid","%u",Gid);
	a.setf("blocks","%u",Blocks);
	a.setf("blocksize","%u",BlockSize);
	a.setf("numlinks","%u",NumLinks);
	a.set("atime",ATime);
	a.set("ctime",CTime);
	a.set("mtime",MTime);
}

/*!\brief Verzeichniseintrag ausgeben
 *
 * Gibt den Inhalt des aktuellen Verzeichnis-Eintrags auf STDOUT aus.
 *
 * \param label ist ein optionaler String, der bei der Ausgabe jeder Zeile
 * vorangestellt wird.
 */
void DirEntry::print(const char *label)
{
	if (!label) label=(const char*)"DirEntry";
	AssocArray a;
	toArray(a);
	a.list(label);
}



} // EOF namespace ppl7
