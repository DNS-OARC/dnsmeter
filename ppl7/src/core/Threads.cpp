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

#ifdef HAVE_PTHREADS
	#include <pthread.h>
#endif

#ifdef HAVE_LIMITS_H
	#include <limits.h>
#endif

//#define HAVE_HELGRIND
#ifdef HAVE_VALGRIND_HELGRIND_H
#include <valgrind/helgrind.h>
#endif

#include "ppl7.h"
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif
#include "threads.h"

namespace ppl7 {

static ppluint64 global_thread_id=0;
static Mutex GlobalThreadMutex;



#ifdef _WIN32
Mutex Win32ThreadMutex;
DWORD Win32ThreadTLS=TLS_OUT_OF_INDEXES;
#endif


typedef struct {
	Thread		*threadClass;
	void 		(*threadFunction)(void *);
	void 		*data;
	THREADDATA	*td;
} THREADSTARTUP;

#ifdef HAVE_TLS
	__thread THREADDATA *myThreadData=NULL;
#else
#ifdef HAVE_PTHREADS
static pthread_key_t thread_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static void make_key()
{
	//DLOG ("static void make_key()\n");
	(void) pthread_key_create(&thread_key, NULL);
}
#endif
#endif


static void CreateTLS(THREADDATA *ptr)
{
#ifdef HAVE_TLS
	myThreadData=ptr;
#elif defined  HAVE_PTHREADS
	(void) pthread_once(&key_once, make_key);
	(void) pthread_setspecific(thread_key, myThreadData);
#elif defined  _WIN32
	Win32ThreadMutex.lock();
	if (Win32ThreadTLS==TLS_OUT_OF_INDEXES) {
		Win32ThreadTLS=TlsAlloc();
		if (Win32ThreadTLS==TLS_OUT_OF_INDEXES) {
			::MessageBoxW(NULL,L"TLS_OUT_OF_INDEXES",L"Error: ppl7::CreateTLS",MB_ICONERROR);
			exit(0);
		}
	}
	ptr->thread=GetCurrentProcess();
	if (!TlsSetValue(Win32ThreadTLS,ptr)) {
		::MessageBoxW(NULL,L"TlsSetValue failed",L"Error: ppl7::CreateTLS",MB_ICONERROR);
		exit(0);
	}
	Win32ThreadMutex.unlock();
#else
	throw NoThreadSupportException();
#endif
}


THREADDATA * GetThreadData()
/*!\ingroup PPLGroupThreads
 */
{
#if defined(HAVE_TLS) && defined(HAVE_PTHREADS)
	if (myThreadData==NULL) {
		myThreadData = new THREADDATA;
		memset(myThreadData,0,sizeof(THREADDATA));
		GlobalThreadMutex.lock();
		myThreadData->threadId=global_thread_id;
		global_thread_id++;
		GlobalThreadMutex.unlock();
		myThreadData->thread=pthread_self();
	}
	return myThreadData;
#elif defined  HAVE_PTHREADS
	(void) pthread_once(&key_once, make_key);
	THREADDATA *ptr;
	if ((ptr = (THREADDATA*)pthread_getspecific(thread_key)) == NULL) {
		// Nur der erste Thread kann hier landen, oder Threads die manuell ohne
		// die Thread-Klasse oder StartThread erstellt wurden
		ptr = new THREADDATA;
		memset(ptr,0,sizeof(THREADDATA));
		ptr->thread=pthread_self();
		GlobalThreadMutex.lock();
		ptr->threadid=global_thread_id;
		global_thread_id++;
		GlobalThreadMutex.unlock();
		pthread_attr_init(&ptr->attr);
		(void) pthread_setspecific(thread_key, ptr);
	}
	return ptr;
#elif defined  _WIN32
	THREADDATA *ptr;
	Win32ThreadMutex.lock();
	if (Win32ThreadTLS==TLS_OUT_OF_INDEXES) {
		Win32ThreadTLS=TlsAlloc();
		if (Win32ThreadTLS==TLS_OUT_OF_INDEXES) {
			::MessageBoxW(NULL,L"TLS_OUT_OF_INDEXES",L"Error: ppl7::GetThreadData",MB_ICONERROR);
			exit(0);
		}
	}
	ptr=(THREADDATA*)TlsGetValue(Win32ThreadTLS);
	if (!ptr) {
		ptr = new THREADDATA;
		memset(ptr,0,sizeof(THREADDATA));
		ptr->thread=GetCurrentProcess();
		GlobalThreadMutex.lock();
		ptr->threadId=global_thread_id;
		global_thread_id++;
		GlobalThreadMutex.unlock();
		if (!TlsSetValue(Win32ThreadTLS,ptr)) {
			::MessageBoxW(NULL,L"TlsSetValue failed",L"Error: ppl7::GetThreadData",MB_ICONERROR);
			exit(0);
		}
	}
	Win32ThreadMutex.unlock();
	return ptr;
#else
	throw NoThreadSupportException();
#endif
}

ppluint64 ThreadID()
{
	THREADDATA *ptr=GetThreadData();
	return ptr->threadId;

}

void *GetTLSData()
{
	THREADDATA *ptr=GetThreadData();
	return ptr->clientData;
}

void SetTLSData(void *data)
{
	THREADDATA *ptr=GetThreadData();
	ptr->clientData=data;
}


#ifdef _WIN32
	static DWORD WINAPI ThreadProc(void *param)
	{
		THREADSTARTUP *ts=(THREADSTARTUP *)param;
		CreateTLS(ts->td);
		if (ts->threadClass) {
			ts->threadClass->threadStartUp();
			if (ts->threadClass->threadShouldDeleteOnExit()) delete ts->threadClass;
		} else {
			ts->threadFunction(ts->data);
		}
#ifdef HAVE_TLS
		if (ts->threadClass==NULL) {
			delete(myThreadData);
		}
		myThreadData=NULL;
#endif
		free(ts);
		return 0;
	}
#elif defined HAVE_PTHREADS
	static void *ThreadProc(void *param)
	{
		THREADSTARTUP *ts=(THREADSTARTUP *)param;
		CreateTLS(ts->td);
		if (ts->threadClass) {
			ts->threadClass->threadStartUp();
			if (ts->td->mysql_thread_end) ts->td->mysql_thread_end();
			if (ts->threadClass->threadShouldDeleteOnExit()) {
				delete ts->threadClass;
			}
		} else {
			ts->threadFunction(ts->data);
			if (ts->td->mysql_thread_end) ts->td->mysql_thread_end();
			pthread_attr_destroy(&ts->td->attr);
		}

#ifdef HAVE_TLS
		if (ts->threadClass==NULL) {
			delete(myThreadData);
		}
		myThreadData=NULL;
#endif
		free(ts);
		pthread_exit(NULL);
		return NULL;
	}
#endif


ppluint64 StartThread(void (*start_routine)(void *),void *data)
{
	THREADSTARTUP *ts=(THREADSTARTUP*)malloc(sizeof(THREADSTARTUP));
	if (!ts) throw OutOfMemoryException();
	ts->threadClass=NULL;
	ts->threadFunction=start_routine;
	ts->data=data;
	ts->td=new THREADDATA;
	if (ts->td==NULL) {
		free(ts);
		throw OutOfMemoryException();
	}
	memset(ts->td,0,sizeof(THREADDATA));
	THREADDATA *t=ts->td;
	// ThreadId festlegen
	GlobalThreadMutex.lock();
	t->threadId=global_thread_id;
	global_thread_id++;
	GlobalThreadMutex.unlock();
#ifdef _WIN32
	t->thread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadProc,ts,0,&t->dwThreadID);
	if (t->thread!=NULL) {
		return t->threadId;
	}
	throw ThreadStartException();
#elif defined HAVE_PTHREADS
	pthread_attr_init(&t->attr);
	int ret=pthread_create(&t->thread,&t->attr,ThreadProc,ts);
	if(ret==0) {
		pthread_detach(t->thread);
		return t->threadId;
	}
	free(ts);
	throw ThreadStartException();
#else
	throw NoThreadSupportException();
#endif
	return true;
}

/*! \brief Priorität des aktuellen Threads ändern
 * \ingroup PPLGroupThreads
 *
 * \ingroup PPLGroupThreadsPriority
 *
 */
void ThreadSetPriority(Thread::Priority priority)
{
#ifdef WIN32
	HANDLE h=GetCurrentProcess();
	int p=GetThreadPriority(h);
	switch(priority) {
		case Thread::LOWEST:
			p=THREAD_PRIORITY_LOWEST;
			break;
		case Thread::BELOW_NORMAL:
			p=THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case Thread::NORMAL:
			p=THREAD_PRIORITY_NORMAL;
			break;
		case Thread::ABOVE_NORMAL:
			p=THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case Thread::HIGHEST:
			p=THREAD_PRIORITY_HIGHEST;
			break;
		default:
			throw IllegalArgumentException();
	}
	if (!SetThreadPriority(h,p)) throw ThreadOperationFailedException();
#elif defined HAVE_PTHREADS
	struct sched_param s;
	pthread_t p=pthread_self();
	int policy,c;
	c=pthread_getschedparam(p,&policy,&s);
	if (c!=0) throw ThreadOperationFailedException();
	int min=sched_get_priority_min(policy);
	int max=sched_get_priority_max(policy);
	int normal=(min+max)/2;
	switch(priority) {
		case Thread::LOWEST:
			s.sched_priority=min;
			break;
		case Thread::BELOW_NORMAL:
			s.sched_priority=normal/2;
			break;
		case Thread::NORMAL:
			s.sched_priority=normal;
			break;
		case Thread::ABOVE_NORMAL:
			s.sched_priority=normal+normal/2;
			break;
		case Thread::HIGHEST:
			s.sched_priority=max;
			break;
		default:
			throw IllegalArgumentException();
	}
	c=pthread_setschedparam(p,policy,&s);
	if(c!=0) throw ThreadOperationFailedException();
#else
	throw NoThreadSupportException();
#endif
}

/*! \brief Priorität des aktuellen Threads abfragen
 * \ingroup PPLGroupThreads
 *
 * \ingroup PPLGroupThreadsPriority
 */
Thread::Priority ThreadGetPriority()
{
#ifdef WIN32
	HANDLE h=GetCurrentProcess();
	int p=GetThreadPriority(h);
	switch(p) {
		case THREAD_PRIORITY_LOWEST:
			return Thread::LOWEST;
		case THREAD_PRIORITY_BELOW_NORMAL:
			return Thread::BELOW_NORMAL;
		case THREAD_PRIORITY_NORMAL:
			return Thread::NORMAL;
		case THREAD_PRIORITY_ABOVE_NORMAL:
			return Thread::ABOVE_NORMAL;
		case THREAD_PRIORITY_HIGHEST:
			return Thread::HIGHEST;
	}
	return Thread::UNKNOWN;
#elif defined HAVE_PTHREADS
	struct sched_param s;
	pthread_t p=pthread_self();
	int policy,c;
	c=pthread_getschedparam(p,&policy,&s);
	if(c!=0) throw ThreadOperationFailedException();
	int min=sched_get_priority_min(policy);
	int max=sched_get_priority_max(policy);
	int normal=(min+max)/2;

	if (s.sched_priority==normal) return Thread::NORMAL;
	if (s.sched_priority==min) return Thread::LOWEST;
	if (s.sched_priority==max) return Thread::HIGHEST;
	if (s.sched_priority<normal) return Thread::BELOW_NORMAL;
	if (s.sched_priority>normal) return Thread::ABOVE_NORMAL;
	return Thread::UNKNOWN;
#else
	throw NoThreadSupportException();
#endif
}


/*!\class Thread
 * \ingroup PPLGroupThreads
 * \brief Klasse zum Verwalten von Threads
 *
 * Klasse zum Starten und Verwalten von Threads.
 * \see \ref PPLGroupThreads
 * \par Beispiel:
 * \include Thread_ThreadMain.cpp
 *
 */


/*! \brief Konstruktor der Thread-Klasse
 *
 * Konstruktor der Thread-Klasse. Es werden interne Variablen allokiert und mit
 * Default-Werten gefüllt.
 *
 * \see \ref PPLGroupThreads
 */
Thread::Thread()
{
	flags=0;
	myPriority=Thread::NORMAL;
	THREADDATA *t=new THREADDATA;
	if (!t) throw OutOfMemoryException();
	memset(t,0,sizeof(THREADDATA));
	threaddata=t;
	t->thread=0;
	IsRunning=0;
	IsSuspended=0;
	deleteMe=0;
	#ifdef HAVE_PTHREADS
		pthread_attr_init(&t->attr);
	#endif
}

/*! \brief Destruktor der Thread-Klasse
 *
 * Falls der Thread noch läuft, wird er zunächst gestoppt. Anschließend werden die
 * internen Variablen wieder freigegeben.
 *
 * \see \ref PPLGroupThreads
 */
Thread::~Thread()
{
	threadStop();
	threadmutex.lock();
	THREADDATA *t=(THREADDATA *)threaddata;
	#ifdef HAVE_PTHREADS
		pthread_attr_destroy(&t->attr);
	#endif
	delete t;
	threadmutex.unlock();
#ifdef HAVE_VALGRIND_HELGRIND_H
	//VALGRIND_HG_DISABLE_CHECKING(this,sizeof(Thread));
	VALGRIND_HG_CLEAN_MEMORY(this,sizeof(Thread));
#endif
}

/*! \brief Der Thread wird gestoppt
 *
 * Dem Thread wird zunächst signalisiert, dass er stoppen soll. Anschließend wartet die
 * Funktion, bis der Thread sich beendet hat.
 *
 * \note Die Thread-Funktion muß in regelmäßigen Abständen mittels der Funktion
 * ThreadShouldStop überprüfen, ob er stoppen soll. Ist dies der Fall, muß sich die
 * Funktion beenden.
 *
 * \see Thread::ThreadSignalStop
 * \see Thread::ThreadShouldStop
 * \see \ref PPLGroupThreads
 */
void Thread::threadStop()
{
	threadmutex.lock();
	flags|=1;
	//THREADDATA *t=(THREADDATA *)threaddata;
	if (IsSuspended) {
		threadmutex.signal();
	}
	while (IsRunning) {
		threadmutex.unlock();
		MSleep(1);
		threadmutex.lock();
	}
	flags=flags&0xfffffffe;
	threadmutex.unlock();
}

/*! \brief Dem Thread signalisieren, dass er stoppen soll
 *
 * Dem Thread wird nur signalisiert, dass er stoppen soll.
 *
 * \see Thread::ThreadStop
 * \see Thread::ThreadShouldStop
 * \see \ref PPLGroupThreads
 */
void Thread::threadSignalStop()
{
	threadmutex.lock();
	flags|=1;
	//THREADDATA *t=(THREADDATA *)threaddata;
	if (IsSuspended) {
		threadmutex.signal();
	} else {
		threadmutex.unlock();
	}
}

/*! \brief Der Thread wird gestartet
 *
 * ThreadStart startet den Thread und kehrt sofort zur aufrufenden Funktion zurück.
 *
 * \see Thread::ThreadMain
 * \see \ref PPLGroupThreads
 */
void Thread::threadStart()
{
	if (threadIsRunning()) {
		throw ThreadAlreadyRunningException();
	}
	IsSuspended=0;
	IsRunning=0;
	THREADSTARTUP *ts=(THREADSTARTUP*)malloc(sizeof(THREADSTARTUP));
	if (!ts) throw OutOfMemoryException();
	ts->threadClass=this;
	ts->threadFunction=NULL;
	ts->data=NULL;
	ts->td=(THREADDATA*)threaddata;
	if (ts->td==NULL) {
		free(ts);
		throw OutOfMemoryException();
	}
	THREADDATA *t=ts->td;
	if (t->threadId==0) {
		GlobalThreadMutex.lock();
		t->threadId=global_thread_id;
		global_thread_id++;
		GlobalThreadMutex.unlock();
	}
#ifdef _WIN32
	t->thread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadProc,ts,0,&t->dwThreadID);
	if (t->thread!=NULL) {
		return;
	}
	threadmutex.lock();
	IsRunning=0;
	threadmutex.unlock();
	free(ts);
	throw ThreadStartException();
#elif defined HAVE_PTHREADS
	int ret=pthread_create(&t->thread,&t->attr,ThreadProc,ts);
	if(ret==0) {
		pthread_detach(t->thread);
		//printf ("Thread erfolgreich gestartet\n");
		return;
	}
	threadmutex.lock();
	IsRunning=0;
	threadmutex.unlock();
	free(ts);
	throw ThreadStartException();
#else
	free(ts);
	throw NoThreadSupportException();
#endif
}

/*!\brief Der Thread gibt seine CPU-Zeit an andere Threads ab
 */
void Thread::threadIdle()
{
#ifdef _WIN32
#elif defined HAVE_PTHREADS
	#ifdef SOLARIS
	#else
		pthread_yield();
	#endif
#endif
}


/*! \brief Der Thread soll pausieren
 *
 * ThreadSuspend setzt das Suspended Flag. Hat nur Auswirkungen, wenn dieses Flag in ThreadMain
 * beachtet wird.
 *
 * \todo Es wäre besser, wenn diese Funktion den Thread Betriebssystemseitig schlafen legen
 * würde, bis ein Resume gegeben wird.
 *
 * \see Thread::ThreadResume
 * \see Thread::ThreadWaitSuspended
 * \see \ref PPLGroupThreads
 */
void Thread::threadSuspend()
{
	threadmutex.lock();
	flags|=2;
	threadmutex.unlock();
}

/*! \brief Der Thread soll weitermachen
 *
 * Dem Thread wird signalisiert, daß er weitermachen soll.
 *
 * \todo Es wäre besser, wenn diese Funktion vom Betriebssystemseitig erledigt würde.
 *
 * \see Thread::ThreadSuspend
 * \see Thread::ThreadWaitSuspended
 * \see \ref PPLGroupThreads
 */
void Thread::threadResume()
{
	threadmutex.lock();
	flags=flags&~2;
	if (IsSuspended) {
		threadmutex.unlock();
		threadmutex.signal();
	} else {
		threadmutex.unlock();
	}
}



/*! \brief Interne Funktion
 *
 * ThreadStartUp wird unmittelbar nach Starten des Threads aufgerufen. Hier werden einige
 * Variablen initialisiert und dann ThreadMain aufgerufen.
 *
 * \note Diese Funktion wird intern verwendet und sollte nicht vom Anwender aufgerufen
 * werden
 *
 * \see Thread::ThreadMain
 * \see \ref PPLGroupThreads
 */
void Thread::threadStartUp()
{
	threadmutex.lock();
	IsRunning=1;
	IsSuspended=0;
	threadmutex.unlock();
	threadSetPriority(myPriority);
	run();
	threadmutex.lock();
	flags=0;
	IsRunning=0;
	IsSuspended=0;
	threadmutex.unlock();
#ifdef HAVE_VALGRIND_HELGRIND_H
	VALGRIND_HG_CLEAN_MEMORY(this,sizeof(Thread));
#endif
	//VALGRIND_HG_DISABLE_CHECKING(this,sizeof(Thread));
}

/*! \brief Flag setzen: Klasse beim Beenden löschen
 *
 * Dem Thread wird mitgeteilt, ob er beim beenden seine eigene Klasse löschen soll. Der
 * Default ist, dass der Anwender selbst die Klasse löschen muß.
 *
 * \param flag kann entweder 1 (Klasse soll automatisch gelöscht werden) oder 0 sein
 * (Klasse nicht löschen).
 * \see Thread::ThreadDeleteOnExit
 * \see \ref PPLGroupThreads
 */
void Thread::threadDeleteOnExit(int flag)
{
	threadmutex.lock();
	if (flag) deleteMe=1;
	else deleteMe=0;
	threadmutex.unlock();
}

/*! \brief Interne Funktion
 *
 * Diese Funktion wird intern beim beenden des Threads aufgerufen. Liefert sie "true" zurück,
 * wird die Thread-Klasse automatisch mit delete gelöscht.
 *
 * \return Liefert 1 zurück, wenn die Klasse gelöscht werden soll, sonst 0.
 * \see Thread::ThreadDeleteOnExit
 * \see \ref PPLGroupThreads
 */
int  Thread::threadShouldDeleteOnExit()
{
	int ret=0;
	threadmutex.lock();
	ret=deleteMe;
	threadmutex.unlock();
	if (ret) return 1;
	return 0;
}

/*! \brief Status abfragen: Läuft der Thread?
 *
 * Mit dieser Funktion kann überprüft werden, ob der Thread aktuell ausgeführt wird.
 *
 * \return Liefert 1 zurück, wenn der Thread läuft, sonst 0.
 * \see \ref PPLGroupThreads
 */
int Thread::threadIsRunning()
{
	int ret;
	threadmutex.lock();
	ret=IsRunning;
	threadmutex.unlock();
	return ret;
}

/*! \brief Status abfragen: Schläft der Thread?
 *
 * Mit dieser Funktion kann überprüft werden, ob der Thread aktuell schläft.
 *
 * \return Liefert 1 zurück, wenn der Thread schläft, sonst 0.
 * \see \ref PPLGroupThreads
 */
int Thread::threadIsSuspended()
{
	int ret;
	threadmutex.lock();
	ret=IsSuspended;
	threadmutex.unlock();
	return ret;
}

/*! \brief Flags des Thread auslesen
 *
 * Mit dieser Funktion können die internen Flags ausgelesen werden.
 *
 * \return Liefert den Wert der internen Flag-Variable zurück
 * \deprecated
 * Diese Funktion ist veraltet und sollte nicht mehr verwendet werden.
 * \see \ref PPLGroupThreads
 */
int Thread::threadGetFlags()
{
	int ret;
	threadmutex.lock();
	ret=flags;
	threadmutex.unlock();
	return ret;
}

/*! \brief Prüfen, ob der Thread beendet werden soll
 *
 * Diese Funktion liefert \c true zurück, wenn der Thread gestoppt werden soll.
 * Dies ist der Fall, wenn vorher die Funktion ThreadStop oder ThreadShouldStop
 * aufgerufen wurde.
 *
 * \return Liefert 1 zurück, wenn der Thread gestoppt werden soll, sonst 0.
 * \see Thread::ThreadStop
 * \see Thread::ThreadShouldStop
 * \see \ref PPLGroupThreads
 */
int Thread::threadShouldStop()
{
	int ret;
	threadmutex.lock();
	ret=flags&1;
	threadmutex.unlock();
	return ret;
}

/*! \brief Prüfen, ob der Thread schlafen soll
 *
 * ThreadWaitSuspended prüft, ob der Thread schlafen (suspend) soll, und wenn
 * ja, wartet sie solange, bis ein unsuspend oder stop signalisiert wird.
 * Der optionale Parameter gibt an, nach wievielen Millisekunden jeweils der Status
 * geprüft werden soll.
 *
 * \param msec Millisekunden, nach denen jeweils der Status geprüft werden soll.
 * Wird der Parameter nicht angegeben, wartet die Funktion so lange, bis entweder
 * die Funktion ThreadResume, TheadSignalStop, ThreadStop oder der Destruktor
 * der Klasse aufgerufen wird.
 *
 * \see Thread::ThreadSuspend
 * \see Thread::ThreadResume
 * \see \ref PPLGroupThreads
 */
void Thread::threadWaitSuspended(int msec)
{
	threadmutex.lock();
	//THREADDATA *t=(THREADDATA *)threaddata;
	while ((flags&3)==2) {
		IsSuspended=1;
		threadmutex.wait(msec);
	}
	IsSuspended=0;
	threadmutex.unlock();
}

void Thread::threadSleep(int msec)
{
	threadmutex.lock();
	IsSuspended=1;
	threadmutex.wait(msec);
	IsSuspended=0;
	threadmutex.unlock();
}


/*! \brief ThreadID zurückgeben
 *
 * Diese Funktion liefert die interne ID des Threads zurück.
 *
 * \return Liefert einen 64-Bit-Wert mit der Thread-ID zurück.
 * \see \ref PPLGroupThreads
 */
ppluint64 Thread::threadGetID()
{
	THREADDATA *t=(THREADDATA *)threaddata;
	if (!t) return 0;
	return t->threadId;
}


/*!\brief Einsprungfunktion bei Start des Threads
 *
 * ThreadMain ist die Funktion, die nach Starten des Threads aufgerufen wird.
 * Sie muß von der abgeleiteten Klasse überschrieben werden und enthält den vom
 * Thread auszuführenden Code.
 *
 * \return Die Funktion liefert keinen Return-Wert, jedoch wird bei Verlassen
 * der Funktion der Thread automatisch gestoppt. Wurde zuvor die Funktion
 * Thread::ThreadShouldDeleteOnExit() aufgerufen, wird außerdem die Klasse
 * mit delete gelöscht.
 * \see \ref PPLGroupThreads
 * \par Example
 * \include Thread_ThreadMain.cpp
 */
void Thread::run()
{

}

/*! \brief Priorität des Threads auslesen
 * \ingroup PPLGroupThreadsPriority
 *
 * Liefert die Priorität des Threads zurück.
 *
 * \return liefert einen Wert zurück, der die Priorität des Threads angibt.
 * \see \ref PPLGroupThreads
 */
int Thread::threadGetPriority()
{
#ifdef WIN32
	THREADDATA *t=(THREADDATA *)threaddata;
	int p=GetThreadPriority(t->thread);
	switch(p) {
		case THREAD_PRIORITY_LOWEST:
			return LOWEST;
		case THREAD_PRIORITY_BELOW_NORMAL:
			return BELOW_NORMAL;
		case THREAD_PRIORITY_NORMAL:
			return NORMAL;
		case THREAD_PRIORITY_ABOVE_NORMAL:
			return ABOVE_NORMAL;
		case THREAD_PRIORITY_HIGHEST:
			return HIGHEST;
	}
#elif defined HAVE_PTHREADS
	THREADDATA *t=(THREADDATA *)threaddata;
	struct sched_param s;
	int policy,c;
	c=pthread_getschedparam(t->thread,&policy,&s);
	if(c!=0) return 0;
	int min=sched_get_priority_min(policy);
	int max=sched_get_priority_max(policy);
	int normal=(min+max)/2;

	if (s.sched_priority==normal) return NORMAL;
	if (s.sched_priority==min) return LOWEST;
	if (s.sched_priority==max) return HIGHEST;
	if (s.sched_priority<normal) return BELOW_NORMAL;
	if (s.sched_priority>normal) return ABOVE_NORMAL;
	return UNKNOWN;

#else
	return UNKNOWN;
#endif

	return UNKNOWN;
}

/*! \brief Priorität des Threads ändern
 * \ingroup PPLGroupThreadsPriority
 *
 * Setz die Priorität des Threads
 * \param priority Gibt die Priorität des Threads an. Die möglichen Werte sind im
 * Kapitel \link PPLGroupThreadsPriority Thread Prioritäten \endlink beschrieben.
 * \return Liefert 1 zurück, wenn die Priorität erfolgreich geändert wurde, sonst 0.
 * \see \ref PPLGroupThreads
 */
int Thread::threadSetPriority(int priority)
{
	THREADDATA *t=(THREADDATA *)threaddata;
	myPriority=priority;
	if(!t->thread) return 1;
#ifdef WIN32
	int p=GetThreadPriority(t->thread);
	switch(priority) {
		case LOWEST:
			p=THREAD_PRIORITY_LOWEST;
			break;
		case BELOW_NORMAL:
			p=THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case NORMAL:
			p=THREAD_PRIORITY_NORMAL;
			break;
		case ABOVE_NORMAL:
			p=THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case HIGHEST:
			p=THREAD_PRIORITY_HIGHEST;
			break;
	}
	if (SetThreadPriority(t->thread,p)) return 1;
	return 0;
#elif defined HAVE_PTHREADS
	struct sched_param s;
	int policy,c;
	c=pthread_getschedparam(t->thread,&policy,&s);
	if (c!=0) return 0;
	int min=sched_get_priority_min(policy);
	int max=sched_get_priority_max(policy);
	int normal=(min+max)/2;
	switch(priority) {
		case LOWEST:
			s.sched_priority=min;
			break;
		case BELOW_NORMAL:
			s.sched_priority=normal/2;
			break;
		case NORMAL:
			s.sched_priority=normal;
			break;
		case ABOVE_NORMAL:
			s.sched_priority=normal+normal/2;
			break;
		case HIGHEST:
			s.sched_priority=max;
			break;
		default:
			return 0;
	}
	c=pthread_setschedparam(t->thread,policy,&s);
	if(c==0) return 1;
	return 0;
#else
	return 0;
#endif

}

/*! \brief Stack-Größe des Threads setzen
 * \ingroup PPLGroupThreadsStacksize
 *
 * \see \ref PPLGroupThreadsStacksize
 * \see \ref PPLGroupThreads
 */
int Thread::threadSetStackSize(size_t size)
{
	#ifdef HAVE_PTHREADS
		#ifndef _POSIX_THREAD_ATTR_STACKSIZE
			throw UnsupportedFeatureException("Thread::threadSetStackSize");
		#endif
		THREADDATA *t=(THREADDATA *)threaddata;
		if (size==0) size=PTHREAD_STACK_MIN;
		if (size<PTHREAD_STACK_MIN) {
			throw IllegalArgumentException("Stacksize must not be smaller than %u Bytes",PTHREAD_STACK_MIN);
			return 0;
		}
		if (pthread_attr_setstacksize(&t->attr,size)==0) return 1;
	#endif
	return 0;
}

size_t Thread::threadGetMinimumStackSize()
/*! \brief Minimale Stack-Größe auslesen
 * \ingroup PPLGroupThreadsStacksize
 *
 * \see \ref PPLGroupThreadsStacksize
 * \see \ref PPLGroupThreads
 */
{
	#ifdef HAVE_PTHREADS
		#ifndef _POSIX_THREAD_ATTR_STACKSIZE
			throw UnsupportedFeatureException("Thread::threadGetMinimumStackSize");
		#endif
		return PTHREAD_STACK_MIN;
		#endif
	return 0;
}

/*! \brief Stack-Größe des Threads auslesen
 * \ingroup PPLGroupThreadsStacksize
 *
 * \see \ref PPLGroupThreadsStacksize
 * \see \ref PPLGroupThreads
 */
size_t Thread::threadGetStackSize()
{
	#ifdef HAVE_PTHREADS
		#ifndef _POSIX_THREAD_ATTR_STACKSIZE
			throw UnsupportedFeatureException("Thread::threadGetStackSize");
		#endif
		THREADDATA *t=(THREADDATA *)threaddata;
		size_t s;
		if (pthread_attr_getstacksize(&t->attr,&s)==0) return s;
	#endif
	return 0;
}


} // EOF namespace ppl7
