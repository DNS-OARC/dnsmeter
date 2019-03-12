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
#ifdef HAVE_TIME_H
#include <time.h>
#endif
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

#ifdef HAVE_DIRENT_H
#include <dirent.h>
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

#include <map>

#ifdef _WIN32
#include <io.h>
#define WIN32_LEAN_AND_MEAN		// Keine MFCs
#include <windows.h>
#include <direct.h>
#endif
#include "ppl7.h"

namespace ppl7 {

/*!\class Dir
 * \ingroup PPLGroupFileIO
 * \brief Klasse zum Durchsuchen von Verzeichnissen
 *
 * \desc
 * Die Klasse Dir wird zum Lesen von Verzeichnissen verwendet. Dazu muss man zunächst mit
 * Dir::open ein Verzeichnis öffnen oder das gewünschte Verzeichnis gleich im Konstruktor
 * angeben. Anschließend kann man mit den Funktionen Dir::getFirst und Dir::getNext die
 * einzelnen Dateien auslesen.
 * \par
 * Sofern man beim Öffnen des Verzeichnis nicht explizit eine Sortiermethode angegeben hat,
 * sind die Dateien unsortiert, bzw. in der Reihenfolge, wie das Betriebssystem sie
 * zurückgegeben hat. Mit dem Befehl Dir::resort kann man aber jederzeit eine andere
 * Sortierung einstellen.
 * \par
 * Möchte man nur Dateien, die einem bestimmten Muster (Pattern) entsprechen, kann man
 * statt Dir::getFirst und Dir::getNext auch Dir::getFirstPattern und Dir::getNextPattern
 * verwenden, oder falls mal Regular Expressions verwenden möchte, Dir::getFirstRegExp und
 * Dir::getNextRegExp.
 * \example
 * Das folgende Beispiel zeigt, wie das Homeverzeichnis des aktuellen
 * Benutzers ermittelt wird, und dessen Dateien alphabetisch sortiert
 * ausgegeben werden:
 * \code
#include <ppl7.h>
int main(int argc, char **argv)
{
	// Homeverzeichnis des Users ermitteln
	ppl7::String Home=ppl7::Dir::homePath();
	// Verzeichnis öffnen, Dateien nach Dateiname sortieren
	ppl7::Dir d(Home, ppl7::Dir::SORT_FILENAME);
	// Iterator zum Durchwandern des Verzeichnisses anlegen
	ppl7::Dir::Iterator it;
	d.reset(it);
	// Variable zum Aufnehmen der Dateinformationen
	ppl7::DirEntry e;

	// Wir benötigen einen Try-Block, da ppl7::Dir::getNext eine Exception wirft,
	// wenn das Ende der Dateiliste erreicht ist
	try {
		while (1) {
			// Nächsten Eintrag holen
			e=d.getNext(it);
			// Dateinamen ausgeben
			std::cout << "Datei: " << e.Filename << "\n";

		}
	} catch (ppl7::EndOfListException) {
		std::cout << "Ende\n";
	}
	return 0;
\endcode
 * \par
 * Falls wir die Exceptions umgehen wollen, läßt sich die Schleife auch so
 * realisieren:
\code
	ppl7::DirEntry e;
	while (d.getNext(e,it)) {
		std::cout << "Datei: " << e.Filename << "\n";
	}
\endcode
 *
 */

/*!\enum Dir::Sort
 * \brief Sortiermöglichkeiten
 *
 * In dieser Enumeration sind die verschiedenen Sortiermöglichkeiten definiert,
 * die als Parameter der Funktionen Dir::open, Dir::resort und des Konstruktors
 * der Klasse Dir verwendet werden können.
 */


/*!\var Dir::Sort Dir::SORT_NONE
 * Keine Sortierung. Die Reihenfolge der Dateien hängt vom Betriebs- und Filesystem ab.
 */

/*!\var Dir::Sort Dir::SORT_FILENAME
 * Es wird eine Sortierung anhand der Dateinamen vorgenommen. Dabei wird Groß- und Kleinschreibung
 * beachtet. Dateien, die mit einem Großbuchstaben beginnen, werden zuerst aufgelistet, danach
 * Dateien mit Kleinbuchstaben.
 */

/*!\var Dir::Sort Dir::SORT_FILENAME_IGNORCASE
 * Es wird eine Sortierung anhand der Dateinamen vorgenommen. Dabei wird Groß- und Kleinschreibung
 * ignoriert. Dateien mit Großbuchstaben und Kleinbuchstaben werden vermischt ausgegeben, wobei
 * jedoch die Alphabetische Reihenfolge erhalten bleibt.
 */

/*!\var Dir::Sort Dir::SORT_ATIME
 * Es wird eine Sortierung nach dem Datum des letzten Zugriffs vorgenommen.
 */

/*!\var Dir::Sort Dir::SORT_MTIME
 * Es wird eine Sortierung nach dem Datum der letzten Modifikation vorgenommen.
 * Dieser Zeitstempel ändert sich nur bei Neuanlage der Datei oder des Verzeichnisses,
 * oder wenn ein Schreibzugriff stattgefunden hat.
 */

/*!\var Dir::Sort Dir::SORT_CTIME
 * Es wird eine Sortierung nach dem Datum der letzten Statusänderung vorgenommen.
 * Eine Statusänderung besteht nicht nur bei Neuanlage und Schreibzugriff, sondern
 * auch bei Änderung der Zugriffsrechte oder Verlinkung.
 */

/*!\var Dir::Sort Dir::SORT_SIZE
 * Es wird eine Sortierung nach der Größe der Datei vorgenommen.
 */


/*!\typedef ppl7::List<const DirEntry*>::Iterator Dir::Iterator;
 * \brief Iterator zum Durchwandern der Verzeichnisliste
 *
 * \desc
 * Dieser Iterator wird benötigt, wenn man die Verzeichnisliste mit Dir::getNext oder
 * den verwandten Befehlen durchwandern will.
 * \example
\code
	ppl7::String Home=ppl7::Dir::homePath();
	ppl7::Dir d(Home, ppl7::Dir::SORT_FILENAME);
	// Iterator zum Durchwandern des Verzeichnisses anlegen
	ppl7::Dir::Iterator it;
	d.reset(it);
	ppl7::DirEntry e;
	while (d.getNext(e,it)) {
		std::cout << "Datei: " << e.Filename << "\n";
	}
\endcode
 */



/*!\var ppl7::List<DirEntry> ppl7::Dir::Files
 * \brief Interne Liste mit den eingelesenen, unsortierten Verzeichniseinträgen
 *
 * \desc
 * Interne Liste mit den eingelesenen, unsortierten Verzeichniseinträgen
 *
 */

/*!\var ppl7::Dir::SortedFiles
 * \brief Interne sortierte Liste mit Pointern auf die Verzeichniseinträgen
 *
 * \desc
 * Interne sortierte Liste mit Pointern auf die Verzeichniseinträgen
 */

/*!\var ppl7::Dir::sort
 * \brief Aktuelle Sortiermethode
 *
 * \desc
 * Aktuelle Sortiermethode
 */

/*!\var ppl7::Dir::Path
 * \brief Pfad des aktuell geöffneten Verzeichnisses
 *
 * \desc
 * Pfad des aktuell geöffneten Verzeichnisses
 */


/*!\ingroup PPLGroupFileIO
 * \brief Aktuelles Verzeichnis
 *
 * \desc
 * Diese statische Funktion liefert das aktuelle Verzeichnis zurück.
 *
 * \return String mit dem aktuellen Verzeichnis
 */
String Dir::currentPath()
{
	String ret;
	char *buf=(char*)malloc(2048);
	if (!buf) throw OutOfMemoryException();
#ifdef _WIN32
	if (_getcwd(buf,2048)) {
#else
	if (getcwd(buf,2048)) {
#endif
		ret=buf;
		free(buf);
		return ret;
	}
	int e=errno;
	free(buf);
	switch (e) {
		case EINVAL: throw IllegalArgumentException();
		case ENOENT: throw NonexistingPathException();
		case ENOMEM: throw OutOfMemoryException();
		case ERANGE: throw PathnameTooLongException();
		case EACCES: throw PermissionDeniedException();
		default: throw UnknownException();
	}
}


/*!\ingroup PPLGroupFileIO
 * \brief Homeverzeichnis des aktuellen Users
 *
 * \desc
 * Diese statische Funktion liefert das Homeverzeichnis des aktuellen Benutzers
 * zurück.
 *
 * \return String mit dem Verzeichnis
 * \exception UnsupportedFeatureException Wird geworfen, wenn das Homeverzeichnis
 * nicht ermittelt werden kann.
 */
String Dir::homePath()
{
	String ret;
#ifdef _WIN32
	char *homeDir = getenv("HOMEPATH");
	char *homeDrive = getenv("HOMEDRIVE");
	ret.setf("%s\\%s",homeDrive, homeDir);
	return ret;
#else
	char *homeDir = getenv("HOME");
	if (homeDir!=NULL && strlen(homeDir)>0) {
		ret.set(homeDir);
		return ret;
	}
	throw UnsupportedFeatureException("Dir::homePath");
#endif
}

/*!\ingroup PPLGroupFileIO
 * \brief Verzeichnis für temporäre Dateien
 *
 * \desc
 * Diese statische Funktion liefert das Verzeichnis zurück, in dem
 * temporäre Dateien abgelegt werden können.
 *
 * \return String mit dem Verzeichnis
 */
String Dir::tempPath()
{
#ifdef _WIN32
	TCHAR TempPath[MAX_PATH];
	GetTempPath(MAX_PATH, TempPath);
	String s;
	s.set(TempPath);
	return s;
#endif
	const char *dir = getenv("TMPDIR");
	if (dir!=NULL && strlen(dir)>0) return String(dir);
#ifdef P_tmpdir
	dir=P_tmpdir;
#endif
	if (dir!=NULL && strlen(dir)>0) return String(dir);
	return String("/tmp");
}


/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Konstruktor der Klasse, ohne ein Verzeichnis zu öffnen.
 */
Dir::Dir()
{
	sort=SORT_NONE;
}

/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Der Konstruktor sorgt dafür, dass die internen Variablen und Datenstrukturen
 * initialisert werden. Mit dem Parameter \p path wird das zu öffnende
 * Verzeichni angegeben. Optional kann mit \p s eine Sortierreihenfolge vorgegeben
 * werden. Ohne Angabe des Parameters findet keine Sortierung statt.
 *
 * @param[in] path Zu öffnender Pfad (siehe auch Dir::open)
 * @param[in] s gewünschte Sortierreihenfolge. Defaultmäßig wird keine Sortierung
 * verwendet.
 */
Dir::Dir(const char *path, Sort s)
{
	sort=s;
	if (path) open(path,s);
}

/*!\brief Konstruktor der Klasse
 *
 * \desc
 * Der Konstruktor sorgt dafür, dass die internen Variablen und Datenstrukturen
 * initialisert werden. Mit dem Parameter \p path wird das zu öffnende
 * Verzeichni angegeben. Optional kann mit \p s eine Sortierreihenfolge vorgegeben
 * werden. Ohne Angabe des Parameters findet keine Sortierung statt.
 *
 * @param[in] path Zu öffnender Pfad (siehe auch Dir::open)
 * @param[in] s gewünschte Sortierreihenfolge. Defaultmäßig wird keine Sortierung
 * verwendet.
 */
Dir::Dir(const String &path, Sort s)
{
	sort=s;
	if (path.notEmpty()) open(path,s);
}


/*!\brief Destruktor der Klasse
 *
 * \desc
 * Der Destruktor sorgt dafür, dass der intern reservierte Speicher wieder
 * freigegeben wird.
 */
Dir::~Dir()
{
	clear();
}

/*!\brief Verzeichnisliste löschen
 *
 * \desc
 * Wird diese Funktion nach Dir::open aufgerufen, wird die interne Dateiliste
 * wieder gelöscht und der durch die Klasse belegte Speicher freigegeben.
 * Die Funktion wird automatisch vom Destruktor und zu Beginn
 * von Dir::open aufgerufen, so dass sich ein manueller Aufruf der Funktion in der Regel
 * erübrigt.
 */
void Dir::clear()
{
	Files.clear();
	SortedFiles.clear();
	Path.clear();
	sort=SORT_NONE;
}

/*!\brief Verzeichnis-Eintrag auf STDOUT ausgeben
 *
 * \desc
 * Mit dieser Funktion kann ein Verzeichniseintrag auf STDOUT ausgegeben werden.
 * Die Ausgabe ist ähnlich der des "ls"-Befehls unter Unix, enthält jedoch
 * nicht die Benutzerrechte. Die Funktion wurde hauptsächlich zu Debuggingzwecken
 * eingebaut.
 *
 * \param[in] de Referenz auf einen Verzeichniseintrag
 */
void Dir::print(const DirEntry &de) const
{
	printf ("%s %3u ",(const char*)de.AttrStr,de.NumLinks);
	printf ("%5u %5u ",de.Uid, de.Gid);
	printf ("%10llu ",de.Size);
	printf ("%s %s\n",(const char*)de.MTime.get(),(const char*)de.Filename);
}


/*!\brief Verzeichnis auf STDOUT ausgeben
 *
 * \desc
 * Mit dieser Funktion wird das mit Dir::open oder im Konstruktor ausgewählte Verzeichnis
 * auf STDOUT ausgegeben. Die Ausgabe ist ähnlich der des "ls"-Befehls unter Unix, enthält jedoch
 * nicht die Benutzerrechte. Die Funktion wurde hauptsächlich zu Debuggingzwecken
 * eingebaut.
 */
void Dir::print() const
{
	ppl7::List<const DirEntry*>::Iterator it;
	printf ("Directory Listing: %s\n",(const char*)Path);
	printf ("Total Files: %zu\n",num());
	SortedFiles.reset(it);
	while (SortedFiles.getNext(it)) {
		print(*it.value());
	}

}


/*!\brief Anzahl Dateien
 *
 * \desc
 * Diese Funktion liefert die Anzahl Einträge im geöffneten Verzeichnis zurück. Sie
 * gibt daher erst nach Aufruf von Dir::open einen korrekten Wert zurück. Einträge können
 * nicht nur Dateien sein, sondern auch Verzeichnisse und Symlinks.
 *
 * @return Anzahl Einträge im geöffneten Verzeichnis
 */
size_t Dir::num() const
{
	return Files.count();
}

/*!\brief Anzahl Dateien
 *
 * \desc
 * Diese Funktion liefert die Anzahl Einträge im geöffneten Verzeichnis zurück. Sie
 * gibt daher erst nach Aufruf von Dir::open einen korrekten Wert zurück. Einträge können
 * nicht nur Dateien sein, sondern auch Verzeichnisse und Symlinks.
 *
 * @return Anzahl Einträge im geöffneten Verzeichnis
 */
size_t Dir::count() const
{
	return Files.count();
}


/*!\brief Sortierung ändern
 *
 * \desc
 * Durch Aufruf dieser Funktion kann die Sortierreihenfolge für die get...-Befehle
 * geändert werden. Standardmäßig werden die Dateien unsortiert zurückgegeben.
 * Die Reihenfolge hängt somit im Wesentlichen davon ab, in welcher Reihenfolge
 * die Dateien erstellt wurden, aber auch von Betriebs- und Filesystemabhängigen
 * Vorgängen.
 * \par
 * Die Sortierreihenfolge läßt sich jederzeit durch Aufruf dieser Funktion ändern.
 *
 * \param[in] s Die gewünschte Sortierreihenfolge. Siehe dazu auch die Enumeration Dir::Sort
 * \exception IllegalArgumentException Wird geworfen, wenn eine ungültige Sortiermethode angegeben wird
 */

void Dir::resort(Sort s)
{
	SortedFiles.clear();
	switch (s) {
		case SORT_NONE: resortNone(); break;
		case SORT_FILENAME: resortFilename(); break;
		case SORT_FILENAME_IGNORCASE: resortFilenameIgnoreCase(); break;
		case SORT_ATIME: resortATime(); break;
		case SORT_CTIME: resortCTime(); break;
		case SORT_MTIME: resortMTime(); break;
		case SORT_SIZE: resortSize(); break;
		default: throw IllegalArgumentException();
	}
	sort=s;
}

/*!\brief Dateien unsortiert belassen
 *
 * \desc
 * Diese interne Funktion kopiert lediglich das von Dir::open eingescannte Verzeichnis
 * unsortiert in die von den Iterationsfunktionen verwendete Liste. Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortNone()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);
	while (Files.getNext(it)) {
		SortedFiles.add(&it.value());
	}
	return;
}

/*!\brief Dateien nach Dateiname sortieren
 *
 * \desc
 * Diese interne Funktion sortiert das durch Dir::open eingescannte Verzeichnis
 * nach Dateiname, unter Beachtung von Gross-/Kleinschreibung.
 * Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortFilename()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);

	std::multimap<String, const DirEntry*> sorter;
	while (Files.getNext(it)) {
		const DirEntry &de=it.value();
		sorter.insert(std::pair<String,const DirEntry*>(de.Filename,&de));
	}
	std::multimap<String, const DirEntry*>::const_iterator sortit;
	for (sortit=sorter.begin();sortit!=sorter.end();++sortit) {
		SortedFiles.add((*sortit).second);
	}
}

/*!\brief Dateien nach Dateiname sortieren, Gross-/Kleinschreibung wird ignoriert
 *
 * \desc
 * Diese interne Funktion sortiert das durch Dir::open eingescannte Verzeichnis
 * nach Dateiname, wobei Gross-/Kleinschreibung ignoriert wird.
 * Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortFilenameIgnoreCase()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);

	std::multimap<String, const DirEntry*> sorter;
	String filename;
	while (Files.getNext(it)) {
		const DirEntry &de=it.value();
		filename.set(de.Filename);
		filename.lowerCase();
		sorter.insert(std::pair<String,const DirEntry*>(filename,&de));
	}
	std::multimap<String, const DirEntry*>::const_iterator sortit;
	for (sortit=sorter.begin();sortit!=sorter.end();++sortit) {
		SortedFiles.add((*sortit).second);
	}
}

/*!\brief Dateien nach Modifizierungsdatum sortieren
 *
 * \desc
 * Diese interne Funktion sortiert das durch Dir::open eingescannte Verzeichnis
 * nach dem Modifikations-Zeitstempel der Dateien. Dieser Zeitstempel ändert sich
 * nur bei Neuanlage der Datei oder des Verzeichnisses, oder wenn ein Schreibzugriff
 * stattgefunden hat.
 * Falls mehrere Dateien den gleichen Zeitstempel haben, ist deren Reihenfolge unbestimmt.
 * \par
 * Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortMTime()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);

	std::multimap<DateTime, const DirEntry*> sorter;
	while (Files.getNext(it)) {
		const DirEntry &de=it.value();
		sorter.insert(std::pair<DateTime,const DirEntry*>(de.MTime,&de));
	}
	std::multimap<DateTime, const DirEntry*>::const_iterator sortit;
	for (sortit=sorter.begin();sortit!=sorter.end();++sortit) {
		SortedFiles.add((*sortit).second);
	}
}

/*!\brief Dateien nach Datum der letzten Statusänderung sortieren
 *
 * \desc
 * Diese interne Funktion sortiert das durch Dir::open eingescannte Verzeichnis
 * nach dem Zeitstempel der letzten Statusänderung der Dateien. Eine Statusänderung
 * besteht nicht nur bei Neuanlage und Schreibzugriff, sondern auch bei Änderung
 * der Zugriffsrechte oder Verlinkung.
 * Falls mehrere Dateien den gleichen Zeitstempel haben, ist deren Reihenfolge unbestimmt.
 * \par
 * Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortCTime()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);

	std::multimap<DateTime, const DirEntry*> sorter;
	while (Files.getNext(it)) {
		const DirEntry &de=it.value();
		sorter.insert(std::pair<DateTime,const DirEntry*>(de.CTime,&de));
	}
	std::multimap<DateTime, const DirEntry*>::const_iterator sortit;
	for (sortit=sorter.begin();sortit!=sorter.end();++sortit) {
		SortedFiles.add((*sortit).second);
	}
}

/*!\brief Dateien nach Datum des letzten Zugriffs sortieren
 *
 * \desc
 * Diese interne Funktion sortiert das durch Dir::open eingescannte Verzeichnis
 * nach dem Zeitstempel des letzten Zugriffs auf die Datei.
 * Falls mehrere Dateien den gleichen Zeitstempel haben, ist deren Reihenfolge unbestimmt.
 * \par
 * Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortATime()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);

	std::multimap<DateTime, const DirEntry*> sorter;
	while (Files.getNext(it)) {
		const DirEntry &de=it.value();
		sorter.insert(std::pair<DateTime,const DirEntry*>(de.ATime,&de));
	}
	std::multimap<DateTime, const DirEntry*>::const_iterator sortit;
	for (sortit=sorter.begin();sortit!=sorter.end();++sortit) {
		SortedFiles.add((*sortit).second);
	}
}

/*!\brief Dateien nach Dateigröße sortieren
 *
 * \desc
 * Diese interne Funktion sortiert das durch Dir::open eingescannte Verzeichnis
 * nach der Größe der Dateien. Falls mehrere Dateien mit gleicher Größe vorhanden
 * sind, ist deren Reihenfolge unbestimmt.
 * \par
 * Die Funktion wird
 * von Dir::resort in Abhängigkeit des eingestellten Sortieralgorithmus aufgerufen.
 */
void Dir::resortSize()
{
	ppl7::List<DirEntry>::Iterator it;
	Files.reset(it);

	std::multimap<ppluint64, const DirEntry*> sorter;
	while (Files.getNext(it)) {
		const DirEntry &de=it.value();
		sorter.insert(std::pair<ppluint64,const DirEntry*>(de.Size,&de));
	}
	std::multimap<ppluint64, const DirEntry*>::const_iterator sortit;
	for (sortit=sorter.begin();sortit!=sorter.end();++sortit) {
		SortedFiles.add((*sortit).second);
	}

}


/*!\brief Zeiger auf den ersten Eintrag des Verzeichnisses
 *
 * \desc
 * Mit dieser Funktion wird der Zeiger des Iterators \p it auf den ersten Eintrag
 * im Verzeichnis gesetzt. Der nächste Aufruf von einer der "getNext..."-Funktionen
 * würde somit den ersten Eintrag zurückliefern.
 * \param it Iterator vom Typ ppl7::Dir::Iterator
 */
void Dir::reset(Iterator &it) const
{
	SortedFiles.reset(it);
}

/*!\brief Erster Verzeichniseintrag
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf den ersten Verzeichniseintrag gesetzt und
 * dessen Daten zurückgegeben. Alle weiteren Einträge können mit Dir::getNext ausgelesen werden.
 * \par
 * Die Reihenfolge der durch Dir::getFirst und Dir::getNext zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 * \param it Iterator vom Typ ppl7::Dir::Iterator
 * @return Referenz auf die erste Datei des Verzeichnisses.
 * \exception EndOfListException Wird geworfen, wenn keine Einträge im geöffneten Verzeichnis vorhanden sind.
 */
const DirEntry &Dir::getFirst(Iterator &it) const
{
	if (SortedFiles.getFirst(it)) return *it.value();
	throw EndOfListException();
}

/*!\brief Nächster Verzeichniseintrag
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf den nächsten Verzeichniseintrag gesetzt und
 * dessen Daten zurückgegeben.
 * \par
 * Die Reihenfolge der durch Dir::getFirst und Dir::getNext zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 *
 * \param it Iterator vom Typ ppl7::Dir::Iterator
 * @return Referenz auf die nächste Datei des Verzeichnisses.
 * \exception EndOfListException Wird geworfen, wenn das Ende der Liste erreicht wurde.
 */
const DirEntry &Dir::getNext(Iterator &it) const
{
	if (SortedFiles.getNext(it)) return *it.value();
	throw EndOfListException();
}

/*!\brief Erster Verzeichniseintrag, der zu einem bestimmten Muster passt
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf die erste Datei gesetzt, die
 * zu dem angegebenen Muster \p pattern passt. Die Daten der Datei werden als Referenz zurückgegeben.
 * Alle weiteren Dateien können mit Dir::getNextPattern ausgelesen werden.
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort, Dir::open
 *
 * \param[in] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] pattern Ein beliebiges Suchpattern, wie es auch beim Unix-Befehl "ls" oder mit
 * "dir" unter Windows angegeben werden kann. Dabei sind die Wildcards "*" und "?" erlaubt.
 * Das Sternchen "*" steht dabei für beliebig viele Zeichen, das Fragezeichen "?" für ein einzelnes.
 * \param[in] ignorecase Wird diese Variable auf "true" gesetzt, wird Groß- und Kleinschreibung
 * ignoriert. Wird als Pattern beispielsweise "*.TXT" angegeben, würde auch "*.txt" passen.
 * Der Default ist "false".
 *
 * @return Referenz auf die erste Datei des Verzeichnisses.
 * \exception EndOfListException Wird geworfen, wenn keine Einträge im geöffneten Verzeichnis vorhanden sind.
 */
const DirEntry &Dir::getFirstPattern(Iterator &it, const String &pattern, bool ignorecase) const
{
	reset(it);
	return getNextPattern(it, pattern, ignorecase);
}

/*!\brief Nächster Verzeichniseintrag, der zu einem bestimmten Muster passt
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf den nächsten Verzeichniseintrag gesetzt,
 * der  zu dem angegebenen Muster \p pattern passt, und dessen Daten zurückgegeben.
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort, Dir::open
 *
 * \param[in] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] pattern Ein beliebiges Suchpattern, wie es auch beim Unix-Befehl "ls" oder mit
 * "dir" unter Windows angegeben werden kann. Dabei sind die Wildcards "*" und "?" erlaubt.
 * Das Sternchen "*" steht dabei für beliebig viele Zeichen, das Fragezeichen "?" für ein einzelnes.
 * \param[in] ignorecase Wird diese Variable auf "true" gesetzt, wird Groß- und Kleinschreibung
 * ignoriert. Wird als Pattern beispielsweise "*.TXT" angegeben, würde auch "*.txt" passen.
 * Der Default ist "false"
 *
 * @return Referenz auf die nächste Datei des Verzeichnisses.
 * \exception EndOfListException Wird geworfen, wenn das Ende der Liste erreicht wurde.
 */
const DirEntry &Dir::getNextPattern(Iterator &it, const String &pattern, bool ignorecase) const
{
	String Pattern;
	Pattern=pattern;
	//printf ("Pattern: %ls\n",(const wchar_t*)Pattern);
	Pattern.pregEscape();
	//printf ("Pattern: %ls\n",(const wchar_t*)Pattern);
	Pattern.replace("\\*",".*");
	Pattern.replace("\\?",".");
	Pattern="/^"+Pattern;
	Pattern+="$/s";
	if (ignorecase) Pattern+="i";
	//printf ("Pattern: %s\n",(const char*)Pattern);
	while (SortedFiles.getNext(it)) {
		const DirEntry *de=it.value();
		// Patternmatch
		//printf ("Match gegen: %s\n",(const char*)de->Filename);
		if (de->Filename.pregMatch(Pattern)) return *de;
	}
	throw EndOfListException();
}

/*!\brief Erster Verzeichniseintrag, der zu der angegebenen Regular Expression passt
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf die erste Datei gesetzt, die
 * zu der angegebenen Regular Expression \p regexp passt. Die Daten der Datei werden als Referenz zurückgegeben.
 * Alle weiteren Dateien können mit Dir::getNextRegExp ausgelesen werden.
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort, Dir::open
 *
 * \param[in] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] regexp Eine beliebige Perl kompatible Regular Expression. Beispiel:"/^*.txt$/i"
 *
 * @return Referenz auf die erste Datei des Verzeichnisses.
 * \exception EndOfListException Wird geworfen, wenn keine Einträge im geöffneten Verzeichnis vorhanden sind.
 */
const DirEntry &Dir::getFirstRegExp(Iterator &it, const String &regexp) const
{
	reset(it);
	return getNextRegExp(it,regexp);
}

/*!\brief Nächster Verzeichniseintrag, der zu der angegebenen Regular Expression passt
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf den nächsten Verzeichniseintrag gesetzt,
 * der zu der angegebenen Regular Expression \p regexp passt, und dessen Daten zurückgegeben.
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort, Dir::open
 *
 * \param[in] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] regexp Eine beliebige Perl kompatible Regular Expression. Beispiel:"/^*.txt$/i"
 *
 * @return Referenz auf die nächste Datei des Verzeichnisses.
 * \exception EndOfListException Wird geworfen, wenn das Ende der Liste erreicht wurde.
 */
const DirEntry &Dir::getNextRegExp(Iterator &it, const String &regexp) const
{
	while (SortedFiles.getNext(it)) {
		const DirEntry *de=it.value();
		// Patternmatch
		if (de->Filename.pregMatch(regexp)) return *de;
	}
	throw EndOfListException();
}



/*!\brief Erster Verzeichniseintrag
 *
 * \desc
 * Durch Aufruf dieser Funktion wird der Iterator \p it auf die erste gefundene Datei gesetzt.
 * Deren Daten werden in das DirEntry Objekt \p e kopiert.
 * Alle weiteren Dateien können mit Dir::getNext ausgelesen werden.
 * \par
 * Die Reihenfolge der durch Dir::getFirst und Dir::getNext zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 * \param[out] e Objekt, in dem die Daten der Datei gespeichert werden sollen
 * \param[in,out] it Iterator vom Typ ppl7::Dir::Iterator
 * @return Gibt \c true zurück, wenn eine Datei im Verzeichnis vorhanden war, sonst \c false.
 * Falls ein Fehler auftritt, kann auch eine Exception geworfen werden.
 */
bool Dir::getFirst(DirEntry &e, Iterator &it) const
{
	if (!SortedFiles.getFirst(it)) return false;
	e=*it.value();
	return true;
}

/*!\brief Nächster Verzeichniseintrag
 *
 * \desc
 * Diese Funktion kopiert die nächste Datei aus dem geöffneten Verzeichnis in das DirEntry Objekt
 * \p e
 * \par
 * Die Reihenfolge der durch Dir::getFirst und Dir::getNext zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 * \param[out] e Objekt, in dem die Daten der Datei gespeichert werden sollen
 * \param[in,out] it Iterator vom Typ ppl7::Dir::Iterator
 * @return Gibt \c true zurück, wenn eine Datei im Verzeichnis vorhanden war, sonst \c false.
 * Falls ein Fehler auftritt, kann auch eine Exception geworfen werden.
 */
bool Dir::getNext(DirEntry &e, Iterator &it) const
{
	if (!SortedFiles.getNext(it)) return false;
	e=*it.value();
	return true;

}

/*!\brief Erster Verzeichniseintrag, der zu einem bestimmten Muster passt
 *
 * \desc
 * Durch Aufruf dieser Funktion wird die erste Datei aus dem Verzeichnis,
 * die zu dem angegebenen Muster \p pattern passt, in das DirEntry Objekt
 * \p e kopiert. Alle weiteren Dateien können mit Dir::getNextPattern
 * ausgelesen werden.
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 *
 * \param[out] e Objekt, in dem die Daten der Datei gespeichert werden sollen
 * \param[in,out] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] pattern Ein beliebiges Suchpattern, wie es auch beim Unix-Befehl "ls" oder mit
 * "dir" unter Windows angegeben werden kann. Dabei sind die Wildcards "*" und "?" erlaubt.
 * Das Sternchen "*" steht dabei für beliebig viele Zeichen, das Fragezeichen "?" für ein einzelnes.
 * \param[in] ignorecase Wird diese Variable auf "true" gesetzt, wird Groß- und Kleinschreibung
 * ignoriert. Wird als Pattern beispielsweise "*.TXT" angegeben, würde auch "*.txt" passen.
 * Der Default ist "false"
 *
 * @return Gibt \c true zurück, wenn eine Datei im Verzeichnis vorhanden war, sonst \c false.
 * Falls ein Fehler auftritt, kann auch eine Exception geworfen werden.
 */
bool Dir::getFirstPattern(DirEntry &e, Iterator &it, const String &pattern, bool ignorecase) const
{
	reset(it);
	return getNextPattern(e, it, pattern, ignorecase);
}

/*!\brief Nächster Verzeichniseintrag, der zu einem bestimmten Muster passt
 *
 * \desc
 * Diese Funktion kopiert die nächste Datei aus dem geöffneten Verzeichnis,
 * die zu dem angegebenen Muster \p pattern passt, in das DirEntry Objekt \p e
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 *
 * \param[out] e Objekt, in dem die Daten der Datei gespeichert werden sollen
 * \param[in,out] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] pattern Ein beliebiges Suchpattern, wie es auch beim Unix-Befehl "ls" oder mit
 * "dir" unter Windows angegeben werden kann. Dabei sind die Wildcards "*" und "?" erlaubt.
 * Das Sternchen "*" steht dabei für beliebig viele Zeichen, das Fragezeichen "?" für ein einzelnes.
 * \param[in] ignorecase Wird diese Variable auf "true" gesetzt, wird Groß- und Kleinschreibung
 * ignoriert. Wird als Pattern beispielsweise "*.TXT" angegeben, würde auch "*.txt" passen.
 * Der Default ist "false"
 *
 * @return Gibt \c true zurück, wenn eine Datei im Verzeichnis vorhanden war, sonst \c false.
 * Falls ein Fehler auftritt, kann auch eine Exception geworfen werden.
 */
bool Dir::getNextPattern(DirEntry &e, Iterator &it, const String &pattern, bool ignorecase) const
{
	String Pattern;
	Pattern=pattern;
	//printf ("Pattern: %ls\n",(const wchar_t*)Pattern);
	Pattern.pregEscape();
	//printf ("Pattern: %ls\n",(const wchar_t*)Pattern);
	Pattern.replace("\\*",".*");
	Pattern.replace("\\?",".");
	Pattern="/^"+Pattern;
	Pattern+="$/";
	if (ignorecase) Pattern+="i";
	//printf ("Pattern: %ls\n",(const wchar_t*)Pattern);
	while (SortedFiles.getNext(it)) {
		const DirEntry *de=it.value();
		// Patternmatch
		//printf ("Match gegen: %ls\n",(const wchar_t*)Name);
		if (de->Filename.pregMatch(Pattern)) {
			e=*de;
			return true;
		}
	}
	return false;

}

/*!\brief Erster Verzeichniseintrag, der zu der angegebenen Regular Expression passt
 *
 * \desc
 * Durch Aufruf dieser Funktion wird die erste Datei aus dem Verzeichnis,
 * die zu der angegebenen Regular Expression \p regexp passt, in das DirEntry Objekt
 * \p e kopiert. Alle weiteren Dateien können mit Dir::getNextPattern
 * ausgelesen werden.
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 *
 * \param[out] e Objekt, in dem die Daten der Datei gespeichert werden sollen
 * \param[in,out] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] regexp Eine beliebige Perl kompatible Regular Expression. Beispiel:"/^*.txt$/i"
 *
 * @return Gibt \c true zurück, wenn eine Datei im Verzeichnis vorhanden war, sonst \c false.
 * Falls ein Fehler auftritt, kann auch eine Exception geworfen werden.
 */
bool Dir::getFirstRegExp(DirEntry &e, Iterator &it, const String &regexp) const
{
	reset(it);
	return getNextRegExp(e,it,regexp);
}

/*!\brief Nächster Verzeichniseintrag, der zu der angegebenen Regular Expression passt
 *
 * \desc
 * Diese Funktion kopiert die nächste Datei aus dem geöffneten Verzeichnis, die
 * zu der angegebenen Regular Expression \p regexp passt, in das DirEntry Objekt
 * \p e.
 *
 * \par
 * Die Reihenfolge der durch Dir::getFirstPattern und Dir::getNextPattern zurückgelieferten Dateien hängt von
 * der eingestellten Sortierung ab. Siehe dazu Dir::resort und Dir::open
 *
 * \param[out] e Objekt, in dem die Daten der Datei gespeichert werden sollen
 * \param[in,out] it Iterator vom Typ ppl7::Dir::Iterator
 * \param[in] regexp Eine beliebige Perl kompatible Regular Expression. Beispiel:"/^*.txt$/i"
 *
 * @return Gibt \c true zurück, wenn eine Datei im Verzeichnis vorhanden war, sonst \c false.
 * Falls ein Fehler auftritt, kann auch eine Exception geworfen werden.
 */
bool Dir::getNextRegExp(DirEntry &e, Iterator &it, const String &regexp) const
{
	while (SortedFiles.getNext(it)) {
		const DirEntry *de=it.value();
		// Patternmatch
		if (de->Filename.pregMatch(regexp)) {
			e=*de;
			return true;
		}
	}
	return false;
}


/*!\brief Verzeichnis einlesen
 *
 * \desc
 * Mit dieser Funktion wird das mit \p path angegebene Verzeichnis geöffnet,
 * eingelesen und mit der Sortiermethode \p s sortiert.
 *
 * @param[in] path Zu öffnender Pfad (siehe auch CDir::Open)
 * @param[in] s gewünschte Sortierreihenfolge. Defaultmäßig wird keine Sortierung
 * verwendet.
 * @return Die Funktion hat keinen Rückgabewert. Bei Auftreten eines Fehlers wird
 * eine Exception geworfen.
 */
void Dir::open(const String &path, Sort s)
{
	open((const char*)path,s);
}

/*!\brief Verzeichnis einlesen
 *
 * \desc
 * Mit dieser Funktion wird das mit \p path angegebene Verzeichnis geöffnet,
 * eingelesen und mit der Sortiermethode \p s sortiert.
 *
 * @param[in] path Zu öffnender Pfad (siehe auch CDir::Open)
 * @param[in] s gewünschte Sortierreihenfolge. Defaultmäßig wird keine Sortierung
 * verwendet.
 * @return Die Funktion hat keinen Rückgabewert. Bei Auftreten eines Fehlers wird
 * eine Exception geworfen.
 */
void Dir::open(const char *path, Sort s)
{
#ifdef HAVE_OPENDIR
	clear();
	sort=s;
	Path=path;
	Path.trim();
	Path.trimRight("/");
	Path.trimRight("\\");
	DIR *dir=opendir((const char*)Path);
	if (!dir) {
		File::throwErrno(errno,path);
	}
	DirEntry de;
	String CurrentFile;
	while (1) {
		struct dirent *result=readdir(dir);
		if (result==NULL) break;
		CurrentFile=Path+"/"+String(result->d_name);
		//ppl7::PrintDebugTime ("DEBUG: CurrentFile=%s\n",(const char*)CurrentFile);
		try {
			File::statFile(CurrentFile,de);
			Files.add(de);
		} catch (...) {

		}
	}
	closedir(dir);
	resort(sort);
#else
	throw UnsupportedFeatureException("Dir::open");
#endif
}


bool Dir::exists(const String &dirname)
{
	try {
		DirEntry f;
		File::statFile(dirname,f);
		if (f.isDir()) return true;
		if (f.isLink()) return true;
		return false;
	} catch (...) {
		return false;
	}
	return false;
}

void Dir::mkDir(const String &path)
{
	Dir::mkDir(path,false);
}

void Dir::mkDir(const String &path, bool recursive)
{
#ifdef _WIN32
	Dir::mkDir(path,0,recursive);
#else
	Dir::mkDir(path,
		S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH,
		recursive);
#endif
}

void Dir::mkDir(const String &path, mode_t mode, bool recursive)
{
	String s;
	if (path.isEmpty()) throw IllegalArgumentException("IllegalArgumentException");
	// Wenn es das Verzeichnis schon gibt, koennen wir sofort aussteigen
	if (Dir::exists(path)) return;

	//printf ("path=%s\n",(const char*)path);
	// 1=erfolgreich, 0=Fehler
	if (!recursive) {
#ifdef _WIN32
		s=path;
		s.replace("/","\\");
		if (mkdir(s)==0) return;
#else
		if (mkdir((const char*)path,mode)==0) return;
#endif
		throw CreateDirectoryFailedException();
	}
	// Wir hangeln uns von unten nach oben
	s.clear();
	Array tok;
	StrTok(tok,path,"/");
	//tok.explode(path,"/");
	//tok.list("tok");
	//throw UnknownException();

	if(path[0]=='/') s.append("/");
	for (size_t i=0;i<tok.count();i++) {
		s.append(tok[i]);
		// Prüfen, ob das Verzeichnis da ist.
		if (!Dir::exists(s)) {
#ifdef _WIN32
			if(s.right(1)!=":") {
				s.replace("/","\\");
				if (mkdir((const char*)s)!=0) throw CreateDirectoryFailedException();
			}
#else
			if (mkdir((const char*)s,mode)!=0) throw CreateDirectoryFailedException();
#endif
		}
		s.append("/");
	}
}

} // EOF namespace ppl7
