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
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#ifdef HAVE_SIGNAL_H
	#include <signal.h>
#endif

#ifdef HAVE_ERRNO_H
	#include <errno.h>
#endif


#include "ppl7.h"


namespace ppl7 {

static Signal *signalhandler=NULL;

static void pplsignal(int sig)
{
	if (signalhandler) {
		signalhandler->signalHandler((Signal::SignalType)sig);
	}
}


/*!\class Signal
 * \ingroup PPLGroupThreads
 */
Signal::Signal()
{
	if (!signalhandler) signalhandler=this;
}

Signal::~Signal()
{
	if (signalhandler==this) signalhandler=NULL;
}

void Signal::catchSignal(SignalType sig)
{
	if (SIG_ERR==signal((int)sig,pplsignal)) throwExceptionFromErrno(errno,"Signal::catchSignal");
}

void Signal::ignoreSignal(SignalType sig)
{
	if (SIG_ERR==signal((int)sig, SIG_IGN)) throwExceptionFromErrno(errno,"Signal::catchSignal");
}

void Signal::clearSignal(SignalType sig)
{
	if (SIG_ERR==signal((int)sig, SIG_DFL)) throwExceptionFromErrno(errno,"Signal::catchSignal");
}

void Signal::signalHandler(SignalType sig)
{
}


}	// EOF namespace ppl7

