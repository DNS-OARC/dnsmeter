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
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef HAVE_PTHREADS
	#include <pthread.h>
#endif


#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
	#include <sys/time.h>
#endif


#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN		// Keine MFCs

	// ms-help://MS.VSCC.2003/MS.MSDNQTR.2003FEB.1031/sdkintro/sdkintro/using_the_sdk_headers.htm
	#ifndef _WIN32_WINNT
	#   define _WIN32_WINNT 0x500
	#endif
	#include <windows.h>
#endif


#include "ppl7.h"

namespace ppl7 {

typedef struct tagMutex
{
	#ifdef _WIN32
		HANDLE	handle;
		HANDLE	condition;
	#elif defined HAVE_PTHREADS
		pthread_mutex_t handle;
		pthread_cond_t	condition;
	#else
		//void *handle;
	#endif
} PPLMUTEX;

/*!\class Mutex
 * \ingroup PPLGroupThreads
 * \ingroup PPLGroupThreadsMutex
 *
 * Mutexes stellen ein Low-Level-Interface für die Synchronisation von Threads dar. Mit ihnen
 * kann verhindert werden, dass mehrere Threads versuchen gleichzeitig auf die gleichen Daten
 * zuzugreifen.
 *
 */

Mutex::Mutex()
{
	handle=::malloc(sizeof(PPLMUTEX));
	if (!handle) throw OutOfMemoryException();
	#ifdef _WIN32
		PPLMUTEX *h=(PPLMUTEX*)handle;
		h->handle=CreateMutex(NULL,false,NULL);
		if (!h->handle)
			throw MutexException("could not create mutex");
		h->condition = CreateEvent(
		        NULL,         // default security attributes
		        TRUE,         // manual-reset event
		        TRUE,         // initial state is signaled
		        NULL 		 // object name
		        );
		if (!h->condition)
			throw MutexException("could not create mutex condition");

	#elif defined HAVE_PTHREADS
		PPLMUTEX *h=(PPLMUTEX*)handle;
		pthread_mutexattr_t Attr;
		pthread_mutexattr_init(&Attr);
		pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutex_init(&h->handle,&Attr);
		pthread_mutexattr_destroy(&Attr);
		/*
		pthread_mutex_init(&h->handle,NULL);
		*/
		pthread_cond_init(&h->condition,NULL);
	#else
		throw UnsupportedFeatureException();
	#endif
}

Mutex::~Mutex() throw()
		{
	if (handle==0) return;
#ifdef _WIN32
	PPLMUTEX *h=(PPLMUTEX*)handle;
	CloseHandle(h->handle);
#elif defined HAVE_PTHREADS
	PPLMUTEX *h=(PPLMUTEX*)handle;
	pthread_mutex_destroy(&h->handle);
	int rc=pthread_cond_destroy(&h->condition);
	if (rc == EBUSY) {	// Thread warten noch auf Condition-Variable
		pthread_cond_signal(&h->condition);
	}
#endif
	::free(handle);
	handle=NULL;
		}

/*!\brief Mutex sperren
 *
 * Diese Funktion versucht einen Mutex zu sperren. Ist dieser bereits durch einen
 * anderen Thread blockiert, wird gewartet, bis der Mutex wieder frei wird.
 *
 * \exception DeadlockException Wird geworfen, wenn durch das Sperren des Mutex ein
 * Deadlock entstehen würde
 * \exception MutexLockingException Mutex konnte nicht gesperrt werden
 */
void Mutex::lock()
{
#ifdef _WIN32
	PPLMUTEX *h=(PPLMUTEX*)handle;
	DWORD ret=WaitForSingleObject(h->handle,INFINITE);
	if (ret==WAIT_OBJECT_0) return;
    throw MutexLockingException("Mutex::lock");
#elif defined HAVE_PTHREADS
	PPLMUTEX *h = (PPLMUTEX*) handle;
	int ret = pthread_mutex_lock(&h->handle);
	if (ret == 0) return;
	else if (ret==EDEADLK) throw DeadlockException("Mutex::lock");
	throw MutexLockingException("Mutex::lock");
#endif
}

/*!\brief Mutex entsperren
 *
 * Mit dieser Funktion wird ein zuvor mit Mutex::lock gesperrter
 * Mutex wieder frei gegeben.
 *
 * \exception MutexNotLockedException Mutex war nicht gesperrt
 * \exception MutexLockingException Mutex konnte nicht entsperrt werden
 */
void Mutex::unlock()
{
#ifdef _WIN32
	PPLMUTEX *h=(PPLMUTEX*)handle;
	if (ReleaseMutex(h->handle)) return;
	throw MutexLockingException("Mutex::unlock");
#elif defined HAVE_PTHREADS
	PPLMUTEX *h=(PPLMUTEX*)handle;
	int ret = pthread_mutex_unlock(&h->handle);
	if (ret == 0) return;
	if (ret == EPERM) throw MutexNotLockedException("Mutex::unlock");
	throw MutexLockingException("Mutex::unlock");
#endif
}

bool Mutex::tryLock() throw ()
/*!\brief Mutex versuchen zu sperren
 *
 * Diese Funktion versucht wie CMutex::Lock einen Mutex zu sperren.
 * Ist dieser allerdings bereits durch einen anderen Thread blockiert,
 * wird nicht gewartet, bis der Mutex wieder frei wird, sondern es wird
 * sofort ein Fehler zurückgeliefert.
 *
 * \return Konnte der Mutex erfolgreich gesperrt werden, liefert die Funktion
 * true (1) zurück, im Fehlerfall false (0)
 */
{
#ifdef _WIN32
	PPLMUTEX *h=(PPLMUTEX*)handle;
	DWORD ret=WaitForSingleObject(h->handle,0);
	if (ret==WAIT_TIMEOUT) return false;
	if (ret!=WAIT_FAILED) return true;
	return false;
#elif defined HAVE_PTHREADS
	PPLMUTEX *h = (PPLMUTEX*) handle;
	int ret = pthread_mutex_trylock(&h->handle);
	if (ret == 0)
		return true;
	return false;
#endif
}


int Mutex::wait(int milliseconds) throw ()
/*! \brief Auf Signal warten
 *
 * Der aufrufende Thread wird angehalten, bis ein anderer Thread diesem Mutex
 * ein Signal gibt (siehe CMutex::Signal) oder die optional angegebene Zeit
 * abgelaufen ist.
 *
 * \param milliseconds Optionale Angabe einer Zeit, nach deren Ablauf der Thread
 * automatisch wieder fortgeführt wird. Default = 0 = unendlich.
 *
 * \return Liefert true (1), wenn dem Mutex ein Signal gegeben wurde. Wenn \p microseconds
 * angegeben wurde, liefert die Funktion nach Ablauf des Timeouts 0 zurück, andernfalls wartet
 * die Funktion so lange, bis ein Signal gegeben wurde.
 *
 * \note
 * War der Mutex vor Aufruf dieser Funktion bereits gelocked, ist er es nach dem Aufruf
 * immer noch.
 *
 */
{
#ifdef _WIN32
	PPLMUTEX *h=(PPLMUTEX*)handle;
	ResetEvent(h->condition);
	int ret;
	if (milliseconds) ret=WaitForSingleObject(h->condition,milliseconds);
	else ret=WaitForSingleObject(h->condition,INFINITE);
	if (ret==WAIT_OBJECT_0) return 1;
	return 0;
#elif defined HAVE_PTHREADS
	PPLMUTEX *h = (PPLMUTEX*) handle;
	int ret = pthread_mutex_trylock(&h->handle);
	int releaseatend = 0;
	if (ret == 0) {
		releaseatend = 1;
	} else if (ret == EBUSY) { // Mutex ist schon gelocked
		releaseatend = 0;
	} else {
		return 0;
	}
	if (milliseconds > 0) {
		struct timeval now;
		struct timespec timeout;
		// Get current Time
		gettimeofday(&now, NULL);
		timeout.tv_nsec = now.tv_usec * 1000 + (milliseconds * 1000000);
		timeout.tv_sec = now.tv_sec + (timeout.tv_nsec / 1000000000);
		timeout.tv_nsec = timeout.tv_nsec % 1000000000;
		ret = pthread_cond_timedwait(&h->condition, &h->handle, &timeout);
	} else {
		ret = pthread_cond_wait(&h->condition, &h->handle);
	}
	if (releaseatend)
		pthread_mutex_unlock(&h->handle);
	if (ret == 0) {
		return 1;
	}
	return 0;
#endif
}


int Mutex::signal() throw ()
/*! \brief Signal senden
 *
 * Mit dieser Funktion wird dem Mutex ein Signal gegeben. Hat ein Thread sich selbst
 * durch Aufruf der Wait-Funktion (siehe CMutex::Wait) angehalten, wird er
 * nach Aufruf des Signals fortgesetzt.
 *
 * \return Bei Erfolg liefert die Funktion true (1) zurück, sonst false (0).
 */
{
#ifdef _WIN32
	PPLMUTEX *h=(PPLMUTEX*)handle;
	int ret=SetEvent(h->condition);
	if (ret==0) return 0;
	return 1;
#elif defined HAVE_PTHREADS
	PPLMUTEX *h = (PPLMUTEX*) handle;
	int ret = pthread_cond_signal(&h->condition);
	if (ret == 0)
		return 1;
	return 0;
#endif
}


} // EOF namespace ppl
