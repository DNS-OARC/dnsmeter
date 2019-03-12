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

/*
       The glibc version of struct tm has additional fields
              long tm_gmtoff;           // Seconds east of UTC
              const char *tm_zone;      // Timezone abbreviation

       defined  when _BSD_SOURCE was set before including <time.h>.  This is a
       BSD extension, present in 4.3BSD-Reno.
*/
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <time.h>
#ifdef HAVE_SYS_TIME_H
	#include <sys/time.h>
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
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Keine MFCs
#include <windows.h>
#endif
#include "ppl7.h"



namespace ppl7 {

/*
ppl_time_t time(ppl_time_t *timer)
{
#ifdef _WIN32
	return (ppl_time_t) _time64((__time64_t*)timer);
#else
	if (!timer) return (ppl_time_t) ::time(NULL);
	time_t a,b;
	a=(time_t)*timer;
	b=::time(&a);
	*timer=a;
	return b;
#endif
}
*/


#if ! defined (HAVE_LOCALTIME_R) || ! defined (HAVE_GMTIME_R)
static Mutex LocalTimeMutex;
#endif




int datum (char *str1)
{
	time_t now;
	struct tm tmstruct;
	time(&now);
#ifdef WIN32
	if (0 == localtime_s(&tmstruct, &now)) {
		sprintf(str1, "%02d.%02d.%04d", tmstruct.tm_mday, tmstruct.tm_mon + 1, tmstruct.tm_year + 1900);
		return 1;
	}
#else
	if (localtime_r(&now,&tmstruct)) {
		sprintf (str1,"%02d.%02d.%04d",tmstruct.tm_mday,tmstruct.tm_mon+1,tmstruct.tm_year+1900);
		return 1;
	}
#endif
	return 0;
}

/*!\ingroup PPLGroupDateTime
 * \brief Liefert die aktuelle Unixtime in einer Struktur zurück
 *
 * Liefert die aktuelle Unix-Zeit als Return-Wert zurück, sowie aufgeschlüsselt in der
 * Struktur PPLTIME.
 *
 * \param t ist ein Pointer auf eine Struktur vom Typ PPLTIME oder NULL.
 * \returns Bei Erfolg wird die Zeit seit 1.1.1970, 00:00 Uhr in Sekunden zurückgegeben,
 * sowie die Struktur PPLTIME gefüllt, sofert der Parameter \a t nicht NULL ist.
 * Tritt ein Fehler auf, wird ((ppluint64)-1) zurückgegeben und errno entsprechend gesetzt.
 *
 * \see ppl6::GetTime()
 * \see ppl6::GetTime(PPLTIME *t, ppluint64 now)
 *
 */
ppluint64 GetTime(PPLTIME *t)
{
	time_t now;
	time(&now);
	if (t) GetTime(t,now);
	return (ppluint64) now;
}

/*!\ingroup PPLGroupDateTime
 * \brief Liefert die aktuelle Unixtime in einer Struktur zurück
 *
 * Liefert die aktuelle Unix-Zeit als Return-Wert zurück, sowie aufgeschlüsselt in der
 * Struktur PPLTIME.
 *
 * \param t Referenz aif eine Struktur vom Typ PPLTIME.
 * \returns Bei Erfolg wird die Zeit seit 1.1.1970, 00:00 Uhr in Sekunden zurückgegeben,
 * sowie die Struktur PPLTIME gefüllt.
 * Tritt ein Fehler auf, wird ((ppluint64)-1) zurückgegeben und errno entsprechend gesetzt.
 *
 */
ppluint64 GetTime(PPLTIME &t)
{
	time_t now;
	time(&now);
	return GetTime(t,now);
}

/*! \fn ppl6::GetTime (PPLTIME *t, ppluint64 now)
 * \ingroup PPLGroupDateTime
 * \brief Wandelt Unix-Zeit in die Struktur PPLTIME um
 *
 * Wandelt die angegebene Unix-Zeit in eine Struktur vom Typ PPLTIME um.
 *
 * \param t ist ein Pointer auf eine Struktur vom Typ PPLTIME oder NULL.
 * \param now enthält die Sekunden seit 1970, die in die PPLTIME-Struktur umgewandelt werden
 * sollen.
 * \returns Bei Erfolg werden die über den Parameter \a now angegebenen Sekunden
 * zurückgeliefert und die Struktur PPLTIME wird gefüllt,
 * \exception Bei Auftreten eines Fehlers wird eine InvalidDateException geworfen.
 *
 * \see ppl6::GetTime()
 * \see ppl6::GetTime(PPLTIME *t)
 *
 */
ppl_time_t GetTime(PPLTIME *t, ppl_time_t now)
{
	struct tm tmstruct;
	time_t n=(time_t)now;
	if (!t) return now;
#ifdef WIN32
	if (0 != localtime_s(&tmstruct, &n)) throw InvalidDateException();
#else
	if (!localtime_r(&n,&tmstruct)) throw InvalidDateException();
#endif
	t->year=tmstruct.tm_year+1900;
	t->month=tmstruct.tm_mon+1;
	t->day=tmstruct.tm_mday;
	t->hour=tmstruct.tm_hour;
	t->min=tmstruct.tm_min;
	t->sec=tmstruct.tm_sec;
	t->epoch=now;
	t->day_of_week=tmstruct.tm_wday;
	t->day_of_year=tmstruct.tm_yday;
	t->summertime=tmstruct.tm_isdst;
	#ifdef STRUCT_TM_HAS_GMTOFF
		t->gmt_offset=tmstruct.tm_gmtoff;
		t->have_gmt_offset=1;
	#else
		t->gmt_offset=0;
		t->have_gmt_offset=0;
	#endif
	return now;
}

/*!\ingroup PPLGroupDateTime
 * \brief Wandelt Unix-Zeit in die Struktur PPLTIME um
 *
 * Wandelt die angegebene Unix-Zeit in eine Struktur vom Typ PPLTIME um.
 *
 * \param t Referenz auf Eine PPLTIME-Struktur
 * \param now enthält die Sekunden seit 1970, die in die PPLTIME-Struktur umgewandelt werden
 * sollen.
 * \returns Bei Erfolg werden die über den Parameter \a now angegebenen Sekunden
 * zurückgeliefert und die Struktur PPLTIME wird gefüllt.
 * \exception Bei Auftreten eines Fehlers wird eine InvalidDateException geworfen.
 *
 * \see ppl6::GetTime()
 * \see ppl6::GetTime(PPLTIME *t)
 *
 */
ppl_time_t GetTime(PPLTIME &t, ppl_time_t now)
{
	struct tm tmstruct;
	memset(&tmstruct,0,sizeof(tm));
	memset(&t,0,sizeof(t));

	time_t n=(time_t)now;
#ifdef WIN32
	if (0 != localtime_s(&tmstruct, &n)) throw InvalidDateException();
#else
	if (!localtime_r(&n, &tmstruct)) throw InvalidDateException();
#endif
	t.year=tmstruct.tm_year+1900;
	t.month=tmstruct.tm_mon+1;
	t.day=tmstruct.tm_mday;
	t.hour=tmstruct.tm_hour;
	t.min=tmstruct.tm_min;
	t.sec=tmstruct.tm_sec;
	t.epoch=now;
	t.day_of_week=tmstruct.tm_wday;
	t.day_of_year=tmstruct.tm_yday;
	t.summertime=tmstruct.tm_isdst;
	#ifdef STRUCT_TM_HAS_GMTOFF
		t.gmt_offset=tmstruct.tm_gmtoff;
		t.have_gmt_offset=1;
	#else
		t.gmt_offset=0;
		t.have_gmt_offset=0;
	#endif
	return now;
}

/*! \brief Liefert die aktuelle Unixtime zurück
 * \ingroup PPLGroupDateTime
 *
 * Diese Funktion gibt die Zeit seit Beginn der "Computer-Epoche" (00:00:00 UTC, January 1, 1970)
 * in Sekunden zurück.
 * \returns Bei Erfolg wird die Zeit seit 1.1.1970, 00:00 Uhr in Sekunden zurückgegeben.
 * Tritt ein Fehler auf, wird ((ppl_time_t)-1) zurückgegeben und errno entsprechend gesetzt.
 *
 * \see ppl6::GetTime(PPLTIME *t)
 * \see ppl6::GetTime(PPLTIME *t, ppl_time_t now)
 *
 */
ppl_time_t GetTime()
{
	time_t now;
	now=time(NULL);
	return (ppl_time_t) now;
}



int USleep(ppluint64 microseconds)
/*!\ingroup PPLGroupDateTime
 */

{		// 1 sec = 1000000 microseconds
	#ifdef _WIN32
		Sleep(DWORD((microseconds+999)/1000));
		return 1;
	#elif defined HAVE_USLEEP
		if (usleep(microseconds)==0) return 1;
		return 0;
	#endif
	return 0;

}

int MSleep(ppluint64 milliseconds)
/*!\ingroup PPLGroupDateTime
 */
{		// 1 sec = 1000 milliseconds
	#ifdef _WIN32
		Sleep((DWORD)milliseconds);
		return 1;
	#elif defined HAVE_USLEEP
		if (usleep(1000*milliseconds)==0) return 1;
		return 0;
	#endif
	return 0;
}

int SSleep(ppluint64 seconds)
/*!\ingroup PPLGroupDateTime
 */
{
	#ifdef _WIN32
		Sleep((DWORD)seconds*1000);
		return 1;
	#elif defined HAVE_USLEEP
		if (usleep(1000000*seconds)==0) return 1;
		return 0;
	#else
		sleep(seconds);
	#endif
	return 0;
}

double GetMicrotime()
/*!\ingroup PPLGroupDateTime
 */
{
	#ifdef _WIN32
		static double time_frequency=0.0;
		LARGE_INTEGER gettime;
		if (time_frequency==0) {
			QueryPerformanceFrequency(&gettime);
			time_frequency=(double)gettime.QuadPart;
		}
		QueryPerformanceCounter(&gettime);
		return (double)gettime.QuadPart/time_frequency;
	#else
		struct timeval tp;
		if (gettimeofday(&tp,NULL)==0) {
			return (double)tp.tv_sec+(double)tp.tv_usec/1000000.0;
		}
		return 0.0;
	#endif
}

ppluint64 GetMilliSeconds()
/*!\ingroup PPLGroupDateTime
 * \brief Aktuelle Zeit in Millisekunden
 *
 * \desc
 * Diese Funktion liefert die Anzahl Millisekunden, die seit dem 1.1.1970 0 Uhr vergangen
 * sind. (1000 Millisekunden = 1 Sekunde).
 *
 * \return Anzahl Millisekunden seit 1970.
 *
 */
{
	ppluint64 t=0;
	#ifdef _WIN32
	static ppluint64 time_frequency=0;
	LARGE_INTEGER gettime;
	if (time_frequency==0) {
		QueryPerformanceFrequency(&gettime);
		time_frequency=(ppluint64)gettime.QuadPart;
	}
	QueryPerformanceCounter(&gettime);
	t=(ppluint64)gettime.QuadPart*1000/time_frequency;
	return t;
	#else
		struct timeval tp;
		if (gettimeofday(&tp,NULL)==0) {
			t=(ppluint64)tp.tv_sec*1000+(ppluint64)(tp.tv_usec/1000);
		}
		return t;
	#endif
}

ppl_time_t MkTime(const String &year, const String &month, const String &day, const String &hour, const String &min, const String &sec)
/*!\ingroup PPLGroupDateTime
 */
{
	struct tm Time;
	memset(&Time,0,sizeof(Time));
	Time.tm_mday = day.toInt();
	Time.tm_mon  = month.toInt()-1;
	Time.tm_year = year.toInt()-1900;
	Time.tm_hour = hour.toInt();
	Time.tm_min  = min.toInt();
	Time.tm_sec  = sec.toInt();
	time_t LTime=mktime(&Time);
	return (ppl_time_t) LTime;
}

ppl_time_t MkTime(int year, int month, int day, int hour, int min, int sec)
/*!\ingroup PPLGroupDateTime
 */
{
	struct tm Time;
	if (year<1900 || month<1) return 0;
	memset(&Time,0,sizeof(Time));
	Time.tm_mday = day;
	Time.tm_mon  = month-1;
	Time.tm_year = year-1900;
	Time.tm_hour = hour;
	Time.tm_min  = min;
	Time.tm_sec  = sec;
	time_t LTime=mktime(&Time);
	return (ppl_time_t) LTime;
}

ppl_time_t MkTime(const PPLTIME &t)
/*!\ingroup PPLGroupDateTime
 */
{
	struct tm Time;
	if (t.year<1900 || t.month<1 || t.month>12 ) return 0;
	memset(&Time,0,sizeof(Time));
	Time.tm_mday = t.day;
	Time.tm_mon  = t.month-1;
	Time.tm_year = t.year-1900;
	Time.tm_hour = t.hour;
	Time.tm_min  = t.min;
	Time.tm_sec  = t.sec;
	time_t LTime=mktime(&Time);
	return (ppl_time_t) LTime;
}


ppl_time_t MkTime(const String &iso8601date, PPLTIME *t)
/*!\ingroup PPLGroupDateTime
 */
{
	Array match;
	struct tm Time;
	memset(&Time,0,sizeof(Time));
	if (iso8601date.pregMatch("/^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})\\+([0-9]{2}):([0-9]{2})$/i",match)) {
		Time.tm_hour = 0-match[7].toInt();
		Time.tm_min = 0-match[8].toInt();
	} else if (iso8601date.pregMatch("/^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})\\-([0-9]{2}):([0-9]{2})$/i",match)) {
		Time.tm_hour = match[7].toInt();
		Time.tm_min = match[8].toInt();
	} else if (!iso8601date.pregMatch("/^([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})$/i",match)) {
		throw InvalidFormatException();
	}
	Time.tm_mday = match[3].toInt();
	Time.tm_mon  = match[2].toInt()-1;
	Time.tm_year = match[1].toInt()-1900;
	Time.tm_hour += match[4].toInt();
	Time.tm_min  += match[5].toInt();
	Time.tm_sec  = match[6].toInt();

	time_t LTime=::mktime(&Time);
	if (LTime==(time_t)-1) throw InvalidDateException(iso8601date);
	if (t) GetTime(t,(ppluint64)LTime);
	return (ppl_time_t) LTime;
}

String MkRFC822Date (const PPLTIME &t)
/*!\ingroup PPLGroupDateTime
 * \brief Datumstring nach RFC-822 (Mailformat) erzeugen
 *
 * \desc
 * Mit dieser Funktion wird ein Datummstring nach RFC-822 erzeugt, wie er im Header einer Email verwendet wird.
 * Das Format lautet:
 * \code
 * weekday, day month year time zone
 * \endcode
 * und hat folgende Bedeutung:
 * - weekday: Name des Wochentags ("Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat")
 * - day: Tag des Monats mit ein oder zwei Ziffern
 * - month: Name des Monats ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")
 * - year: Das Jahr mit 4 Ziffern
 * - time: Stunde:Minute:Sekunde (hh:mm:ss), jeweils mit zwei Ziffern und Doppelpunkt getrennt
 * - zone: Offset zu UTC in Stunden und Minuten (+|-HHMM)
 *
 * \param[in] t Eine PPLTIME-Struktur, der die Datumsinformationen entnommen werden
 *
 * \exception Exception::FunctionFailed Die Funktion wirft eine Exception, wenn die Datumsinformation in der PPLTIME-Struktur ungültig ist.
 */
{
	String s;
	const char *day[]={ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	const char *month[]={ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	// PPLTIME prüfen
	if (t.day_of_week<0 || t.day_of_week>6) throw IllegalArgumentException("MkRFC822Date: week<0 order week>6");
	if (t.month<1 || t.month>12) throw IllegalArgumentException("MkRFC822Date: month<0 order month>12");

	s=day[t.day_of_week];
	s+=", ";
	s.appendf("%i ",t.day);
	s+=month[t.month-1];
	s.appendf(" %04i %02i:%02i:%02i ",t.year,t.hour,t.min,t.sec);
	if (t.have_gmt_offset) {
		if (t.gmt_offset>=0) s.appendf("+%02i%02i",abs(t.gmt_offset/3600),abs(t.gmt_offset%3600));
		else s.appendf("-%02i%02i",abs(t.gmt_offset/3600),abs(t.gmt_offset%3600));
	}
	return s;
}

String MkRFC822Date (ppl_time_t sec)
/*!\ingroup PPLGroupDateTime
 * \brief Datumstring nach RFC-822 (Mailformat) erzeugen
 *
 * \desc
 * Mit dieser Funktion wird ein Datummstring nach RFC-822 erzeugt, wie er im Header einer Email verwendet wird.
 * Das Format lautet:
 * \code
 * weekday, day month year time zone
 * \endcode
 * und hat folgende Bedeutung:
 * - weekday: Name des Wochentags ("Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat")
 * - day: Tag des Monats mit ein oder zwei Ziffern
 * - month: Name des Monats ("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")
 * - year: Das Jahr mit 4 Ziffern
 * - time: Stunde:Minute:Sekunde (hh:mm:ss), jeweils mit zwei Ziffern und Doppelpunkt getrennt
 * - zone: Offset zu UTC in Stunden und Minuten (+|-HHMM)
 *
 * \param[in] sec Ein Optionaler Parameter mit Sekunden seit 1970. Ist er 0 oder wird nicht angegeben, wird die
 * aktuelle Zeit verwendet.
 *
 * \exception Exception::FunctionFailed Die Funktion wirft eine Exception, wenn die Datumsinformation in der PPLTIME-Struktur ungültig ist.
 */
{
	PPLTIME t;
	if (!sec) sec=GetTime();
	if (GetTime(t,sec)!=sec) throw OperationFailedException();
	return MkRFC822Date(t);
}

String MkISO8601Date (ppl_time_t sec)
/*!\ingroup PPLGroupDateTime
 */
{
	PPLTIME t;
	if (!sec) sec=GetTime();
	if (GetTime(t,sec)!=sec) throw OperationFailedException();
	return MkISO8601Date(t);
}

String MkISO8601Date (const PPLTIME &t)
/*!\ingroup PPLGroupDateTime
 */
{
	String buffer;
	buffer.setf("%04i-%02i-%02iT%02i:%02i:%02i",
		t.year, t.month, t.day, t.hour, t.min, t.sec);
	if (t.have_gmt_offset) {
		int off=abs(t.gmt_offset)/60;
		int h=(off/60);
		int m=(off%60);
		if (t.gmt_offset<0) buffer.appendf("-%02i:%02i",h,m);
		else buffer.appendf("+%02i:%02i",h,m);
	} else {
		if (t.summertime) buffer.append("+01:00");
		else buffer.append("+02:00");
	}
	return buffer;
}

/*!\brief Datum/Zeit formatieren
 * \ingroup PPLGroupDateTime
 *
 * \header \#include <ppl7.h>
 * \desc
 * Die Funktion MkDate wandelt einen Unix-Timestamp in einen String um.
 *
 * \param format ist ein beliebiger String, der verschiedene  Platzhalter
 * entahlten darf (siehe unten)
 * \param sec
 * \return Bei Erfolg gibt die Funktion einen neuen String mit dem formatierten
 * Zeitpunkt zurück.
 * \exception Im Fehlerfall wird eine Exception geworfen
 *
 * \par Syntax-Formatstring
 * \copydoc strftime.dox
 */
String MkDate(const String &format, ppl_time_t sec)
{
	String buffer;
	size_t size=strlen(format)*2+32;
	char *b=(char*)malloc(size);
	if (!b) throw OutOfMemoryException();
	struct tm t;
	const time_t tt=(const time_t)sec;

#ifdef WIN32
	if (0 != localtime_s(&t, &tt)) throw InvalidDateException();
#else
	if (!localtime_r(&tt, &t)) throw InvalidDateException();
#endif
	if (strftime(b, size,format, &t)==0) {
		free(b);
		throw OperationFailedException();
	}
	buffer.set(b);
	free(b);
	return buffer;
}


String MkDate(const String &format, const PPLTIME &t)
{
	return MkDate(format,MkTime(t));
}


#ifdef TODO

void datumsauswertung (pplchar * d, pplchar * dat)
/*!\ingroup PPLGroupDateTime
 */
{
	CTok Tok;
	char t [15], ad [11];
	char *strptr = t;

	strcpy (t,dat);
	datum(ad);						/* Aktuelles Datum holen                  */
	if (strlen(t)==0)				/* Ist ein Datum vorhanden?               */
		strcpy(d,ad);				/* Wenn nein, dann aktuelles Datum nehmen */
	else {
		strxchg (t,",",".");		/* Zuerst die Trennzeichen in Punkte      */
		strxchg (t,"-",".");		/* umwandeln                              */
		strxchg (t,"/",".");
		strcat  (t,"..");
		size_t z=0;
		size_t p=0;
		strcpy (d,"");
		Tok.Split(strptr,".");
		while (z<3) {
			const char *tokptr = Tok.GetNext();
			if (tokptr!=NULL) {
				size_t l=strlen(tokptr);
				if (l<2 && z<2)
					strcat (d,"0");
				else if (l<4 && z==2)
					strncat (d,&ad[p],4-l);
				strcat (d,tokptr);
				if (z<2) strcat (d,".");
			} else {
				if (z<2)
					strncat (d,&ad[p],3);
				else
					strncat (d,&ad[p],4);
			}
			p=p+3;
			z++;
		}
	}
	/* Datum auf Plausibilitaet pruefen */
	strncpy (t,d,2);
	z=atoi(t);
	strncpy (t,d+3,2);
	int m=atoi(t);
	if (z<1) {
		d[0]='0';
		d[1]='1';
	}
	if (m<0) {
		d[3]='0';
		d[4]='1';
		m=1;
	}
	if (m>12) {
		d[3]='1';
		d[4]='2';
		m=12;
	}
	if ((m==1||m==3||m==5||m==7||m==8||m==10||m==12) && z>31) {
		d[0]='3';
		d[1]='1';
	}
	if ((m==4||m==6||m==9||m==11) && z>30) {
		d[0]='3';
		d[1]='0';
	}
	if (m==2 && z>28) {
		strncpy (t,d+6,4);
		m=atoi(t);
		if ((m&3)==0) {					// Ein Schaltjahr
			if (z>29) {
				d[0]='2';
				d[1]='9';
			}
		} else {						// Kein Schaltjahr
			if (z>28) {
				d[0]='2';
				d[1]='8';
			}
		}
	}
}


/*!\brief Datum/Zeit formatieren
 * \ingroup PPLGroupDateTime
 *
 * \header \#include <ppl6.h>
 * \desc
 * Die Funktion MkDate wandelt einen Unix-Timestamp in einen String um.
 *
 * \param buffer
 * \param format ist ein beliebiger String, der verschiedene  Platzhalter
 * entahlten darf (siehe unten)
 * \param sec
 *
 * \par Syntax-Formatstring
 * \copydoc strftime.dox
 */
const char *MkDate (CString &buffer, const char *format, ppluint64 sec)
{
	if (!format) {
		SetError(194,"const char *format");
		return NULL;
	}
	size_t size=strlen(format)*2+32;
	char *b=(char*)malloc(size);
	if (!b) {
		SetError(2);
		return NULL;
	}
	char *ret=MkDate(b,size,format,sec);
	if (ret) {
		buffer.Set(b);
	} else {
		free(b);
		return NULL;
	}
	free(b);
	return buffer;
}


CString Long2Date(const char *format, int value)
{
	int day=value%100;
	value=value/100;
	int month=value%100;
	int year=value/100;
	ppluint64 t=MkTime(year,month,day);
	return MkDate(format,t);
}

char *MkDate (char *buffer, int size, const char *format, ppluint64 sec)
/*!\brief Datum/Zeit formatieren
 * \ingroup PPLGroupDateTime
 *
 * \header \#include <ppl6.h>
 * \desc
 * Die Funktion MkDate wandelt einen Unix-Timestamp in einen String um.
 *
 * \param buffer
 * \param size
 * \param format ist ein beliebiger String, der verschiedene  Platzhalter
 * entahlten darf (siehe unten)
 * \param sec
 *
 * \par Syntax-Formatstring
 * \copydoc strftime.dox
 */
{
	if (!buffer) {
		SetError(194,"char *buffer");
		return NULL;
	}
	if (!format) {
		SetError(194,"const char *format");
		return NULL;
	}
	struct tm t;
	const time_t tt=(const time_t)sec;

	localtime_r(&tt, &t);
	if (strftime(buffer, size,format, &t)==0) {
		SetError(348,"%s",format);
		return NULL;
	}
	return buffer;
}

#endif

/*
 * Timer-Klasse
 */

/*!\class Timer
 * \ingroup PPLGroupDateTime
 */

Timer::Timer()
{
	startzeit=GetMicrotime();
	endzeit=0.0;
	myduration=0.0;
}

Timer::~Timer()
{
}

double Timer::start()
{
	startzeit=GetMicrotime();
	return startzeit;
}

double Timer::stop()
{
	endzeit=GetMicrotime();
	myduration=endzeit-startzeit;
	return myduration;
}

double Timer::currentDuration()
{
	return GetMicrotime()-startzeit;
}


double Timer::duration()
{
	return myduration;
}





}		// EOF namespace ppl6

