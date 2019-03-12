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

#ifndef COMPAT_H_
#define COMPAT_H_

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef  HAVE_STRINGS_H
#include <strings.h>
#endif

#include <string>

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef _WIN32
    #include <winsock2.h>
	#include <Ws2tcpip.h>
	#include <windows.h>
#else

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#endif // _WIN32

namespace ppl7 {
namespace compat {

char * trim (char * text);

#ifndef HAVE_STRTOLOWER
char *strtolower (char * text);
#endif

#ifndef HAVE_STRTOUPPER
char *strtoupper (char * text);
#endif

#ifndef HAVE_STRCASESTR
const char *strcasestr(const char *haystack, const char *needle);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char *s1, const char *s2, size_t n);
#endif

#ifndef HAVE_VASPRINTF
int vasprintf(char **buff, const char *fmt, va_list args);
#endif

#ifndef HAVE_ASPRINTF
int asprintf(char **buff, const char *format, ...);
#endif

#ifndef HAVE_HTOL
long htol (const char * wert);
#endif

#ifndef HAVE_ATOLL
long long atoll (const char *wert);
#endif

#ifndef HAVE_STRNDUP
char *strndup(const char *str, size_t len);
#endif

#ifndef HAVE_STRNCMP
int strncmp(const char *s1, const char *s2, size_t len);
#endif

#ifndef HAVE_BCOPY
void bcopy(const void *src, void *dst, size_t len);
#endif

#ifndef HAVE_BZERO
void bzero(void *b, size_t len);
#endif

#ifndef HAVE_STRNLEN
size_t strnlen(const char *str, size_t len);
#endif

#ifndef HAVE_INET_NTOP
const char * inet_ntop(int af, const void *src, char *dst, socklen_t size);
#endif

#ifndef HAVE_INET_PTON
int inet_pton(int af, const char *src, void *dst);
#endif


/*
 * 	const char *strcasestr(const char *haystack, const char *needle);
	int strcasecmp(const char *s1, const char *s2);
	int strncasecmp(const char *s1, const char *s2, size_t n);
	int vasprintf(char **buff, const char *fmt, va_list args);
	int asprintf(char **buff, const char *format, ...);
	long htol (const char * wert);
	long long atoll (const char * wert);
	int atoi(const char *wert);
	long atol(const char *wert);
	double atof(const char *wert);
	char *strndup(const char *str, size_t len);
	int strncmp(const char *s1, const char *s2, size_t len);
	size_t strnlen(const char *str, size_t len);
	void bcopy(const void *src, void *dst, size_t len);
 *
 */
} // EOF namespace compat

using namespace compat;

} // EOF namespace ppl7


#endif /* COMPAT_H_ */
