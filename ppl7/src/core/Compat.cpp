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
namespace compat {

/*! \brief Unnötige Zeichen am Anfang und Ende eines Textes löschen
 *
 * Die \c trim-Funktion löscht alle Leerzeilen, Tabs, Linefeeds und Returns am
 * Zeilenanfang und -ende.
 *
 * \param text Ein Pointer auf einen Null-terminierten String
 * \return Die Funktion gibt NULL zurück, wenn der Parameter \c text NULL war.
 * Ansonsten wird der durch \c text übergebene Pointer zurückgegeben.
 * \warning Die Funktion allokiert keinen neuen Speicher, sondern modifiziert den Text innerhalb des
 * übergebenen Pointers. Soll der Originaltext erhalten bleiben, muß dieser vorher gesichert werden.
 * \see Siehe auch die Funktion trimchar, die ein einzelnes definierbares Zeichen am Anfang und Ende
 * löscht.
 */
char * trim (char * text)
{
	if (!text) return NULL;
	size_t l=strlen(text);
	if (l>0) {
		size_t start=0, s=0;
		int ende=l;
		for (size_t i=0;i<l;i++) {
			if (text[i]==13||text[i]==10||text[i]==32||text[i]=='\t') {
				if (s==0) start=i+1;
			} else {
				s=1; ende=i;
			}
		}
		text [ende+1]=0;
		if (start==0) return (text);
		memmove(text,text+start,ende-start+2);
		return (text);
	}
    return (text);
}

char *strtolower (char * text)
{
	if (text != NULL) {
		size_t l=strlen(text);
		for (size_t i=0;i<l;i++)
			text[i]=tolower(text[i]);
	}
	return text;
}

char *strtoupper (char * text)
{
	if (text != NULL) {
		size_t l=strlen(text);
		for (size_t i=0;i<l;i++)
			text[i]=toupper(text[i]);
	}
	return text;
}

#ifndef HAVE_STRCASESTR
/*!\brief Findet das erste Auftauchen einer Zeichenkette in einem String
 */
const char *strcasestr(const char *haystack, const char *needle)
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

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2)
{
	const unsigned char
	*us1 = (const unsigned char *)s1,
	*us2 = (const unsigned char *)s2;

	while (tolower(*us1) == tolower(*us2++))
		if (*us1++ == '\0')
			return (0);
	return (tolower(*us1) - tolower(*--us2));
}
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n != 0) {
        const unsigned char
                *us1 = (const unsigned char *)s1,
                *us2 = (const unsigned char *)s2;

        do {
            if (tolower(*us1) != tolower(*us2++))
                return (tolower(*us1) - tolower(*--us2));
            if (*us1++ == '\0')
                break;
        } while (--n != 0);
    }
    return (0);
}
#endif


#ifndef HAVE_VASPRINTF
int vasprintf(char **buff, const char *fmt, va_list args)
{
#if defined HAVE_WORKING_VSNPRINTF
	char tb[4];
	int size=vsnprintf(tb,1,fmt,args);
	char *b=(char*)malloc(size+2);
	if (b) {
		vsnprintf(b,size+1,fmt,args);
		*buff=b;
		return size;
	} else {
		*buff=NULL;
		return 0;
	}
#elif defined _WIN32
	// Feststellen, wie groß der String werden würde
	int size=_vscprintf(fmt,args);
	// Buffer allocieren
	char *b=(char*)malloc(size+2);
	if (b) {
		_vsnprintf(b,size+1,fmt,args);
		*buff=b;
		return size;
	} else {
		*buff=NULL;
		return 0;
	}

#else
#pragma error No working vasprintf!!!
	*buff=NULL;
	return 0;
#endif
}
#endif

#ifndef HAVE_ASPRINTF
int asprintf(char **buff, const char *format, ...)
{
	va_list args;
	va_start(args,format);
	char *b=NULL;
	vasprintf(&b,format,args);
	va_end(args);
	if (b) {
		*buff=b;
		return (int)strlen(b);
	}
	*buff=NULL;
	return -1;
}
#endif


#ifndef HAVE_HTOL
long htol (const char * wert)
{
	long mp[]={0x1,0x10,0x100,0x1000,0x10000,0x100000,0x1000000,0x10000000};
	size_t i,l;
	long w;
	char * p;
	strtolower ((char*)wert);
	p=trim((char*)wert);
	w=0;
	l=strlen(p);
	for (i=0;i<l;i++) {
		char t=p[l-i-1]-48;
		if (t>9) t=t-39;
		w+=t*mp[i];
	}
	return (w);
}
#endif

#ifndef HAVE_ATOLL
long long atoll (const char *wert)
{
	if(!wert) return 0;
#if defined _WIN32
	return (long long) _atoi64(wert);
#elif defined HAVE_STRTOLL
	return strtoll(str, (char **)NULL, 10);
#else
#pragma error No working atoll!!!
	return 0;
#endif
}
#endif


#ifndef HAVE_STRNDUP
char *strndup(const char *str, size_t len)
{
	if (!str) return NULL;
	size_t ll=strlen(str);
	if (ll<len) len=ll;
	char *buff=(char*)malloc(len+1);
	if (!buff) return NULL;
	strncpy(buff,str,len);
	buff[len]=0;
	return buff;
}
#endif

#ifndef HAVE_STRNCMP
int strncmp(const char *s1, const char *s2, size_t len)
{
	if (len==0) return 0;
	do {
		if (*s1 != *s2++)
			return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
			if (*s1++ == 0) break;
	} while (--len != 0);
	return (0);
}
#endif

#ifndef HAVE_BCOPY
void bcopy(const void *src, void *dst, size_t len)
{
#if defined _WIN32
	CopyMemory(dst,src,len);
#elif defined HAVE_MEMMOVE
	::memmove(dst,src,len);
#else
#pragma error No working bcopy!!!
#endif
}
#endif

#ifndef HAVE_BZERO
void bzero(void *b, size_t len)
{
#if defined _WIN32
	ZeroMemory(b,len);
#elif defined HAVE_MEMMOVE
	::memset(b,0,len);
#else
#pragma error No working bzero!!!
#endif
}
#endif


#ifndef HAVE_STRNLEN
size_t strnlen(const char *str, size_t len)
{
	if (str==NULL || len==0) return 0;
	size_t p=0;
	while (p<len) {
		if (str[p]==0) return p;
		p++;
	}
	return p;
}
#endif

#ifndef HAVE_INET_NTOP
/*
 * Copyright (C) 2004, 2005, 2007, 2009  Internet Systems Consortium, Inc. ("ISC
")
 * Copyright (C) 1996-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#define NS_INT16SZ       2
#define NS_IN6ADDRSZ    16
static const char *inet_ntop4(const unsigned char *src, char *dst,
		socklen_t size);

static const char *inet_ntop6(const unsigned char *src, char *dst,
		socklen_t size);


const char * inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    switch (af) {
    case AF_INET:
            return (inet_ntop4((const unsigned char*)src, dst, size));
    case AF_INET6:
            return (inet_ntop6((const unsigned char*)src, dst, size));
    default:
            errno = EAFNOSUPPORT;
            return (NULL);
    }
    /* NOTREACHED */
}

static const char *inet_ntop4(const unsigned char *src, char *dst, socklen_t size)
{
        static const char *fmt = "%u.%u.%u.%u";
        char tmp[sizeof("255.255.255.255")];

        if ((socklen_t)sprintf(tmp, fmt, src[0], src[1], src[2], src[3]) >= size)
        {
                errno = ENOSPC;
                return (NULL);
        }
        strcpy(dst, tmp);

        return (dst);
}
static const char *inet_ntop6(const unsigned char *src, char *dst, socklen_t size)
{
        /*
         * Note that int32_t and int16_t need only be "at least" large enough
         * to contain a value of the specified size.  On some systems, like
         * Crays, there is no such thing as an integer variable with 16 bits.
         * Keep this in mind if you think this function should have been coded
         * to use pointer overlays.  All the world's not a VAX.
         */
        char tmp[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")], *tp;
        struct { int base, len; } best, cur;
        unsigned int words[NS_IN6ADDRSZ / NS_INT16SZ];
        int i;

        /*
         * Preprocess:
         *      Copy the input (bytewise) array into a wordwise array.
         *      Find the longest run of 0x00's in src[] for :: shorthanding.
         */
        memset(words, '\0', sizeof(words));
        for (i = 0; i < NS_IN6ADDRSZ; i++)
                words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
        best.base = -1;
        best.len = 0;
        cur.base = -1;
        cur.len = 0;
        for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
                if (words[i] == 0) {
                        if (cur.base == -1)
                                cur.base = i, cur.len = 1;
                        else
                                cur.len++;
                } else {
                        if (cur.base != -1) {
                                if (best.base == -1 || cur.len > best.len)
                                        best = cur;
                                cur.base = -1;
                        }
                }
        }
        if (cur.base != -1) {
            if (best.base == -1 || cur.len > best.len)
                    best = cur;
    }
    if (best.base != -1 && best.len < 2)
            best.base = -1;

    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
            /* Are we inside the best run of 0x00's? */
            if (best.base != -1 && i >= best.base &&
                i < (best.base + best.len)) {
                    if (i == best.base)
                            *tp++ = ':';
                    continue;
            }
            /* Are we following an initial run of 0x00s or any real hex? */
            if (i != 0)
                    *tp++ = ':';
            /* Is this address an encapsulated IPv4? */
            if (i == 6 && best.base == 0 && (best.len == 6 ||
                (best.len == 7 && words[7] != 0x0001) ||
                (best.len == 5 && words[5] == 0xffff))) {
                    if (!inet_ntop4(src+12, tp,
                                    sizeof(tmp) - (tp - tmp)))
                            return (NULL);
                    tp += strlen(tp);
                    break;
            }
            tp += sprintf(tp, "%x", words[i]);
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (NS_IN6ADDRSZ / NS_INT16SZ))
            *tp++ = ':';
    *tp++ = '\0';

    /*
     * Check for overflow, copy, and we're done.
     */
    if ((socklen_t)(tp - tmp) > size) {
            errno = ENOSPC;
            return (NULL);
    }
    strcpy(dst, tmp);
    return (dst);
}
#endif

#ifndef HAVE_INET_PTON
/*
 * Copyright (C) 2004, 2005, 2007, 2013, 2014  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1996-2003  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#define NS_INT16SZ       2
#define NS_INADDRSZ      4
#define NS_IN6ADDRSZ    16
static int inet_pton4(const char *src, unsigned char *dst);
static int inet_pton6(const char *src, unsigned char *dst);

int inet_pton(int af, const char *src, void *dst)
{
    switch (af) {
    case AF_INET:
            return (inet_pton4(src, (unsigned char*)dst));
    case AF_INET6:
            return (inet_pton6(src, (unsigned char*)dst));
    default:
            errno = EAFNOSUPPORT;
            return (-1);
    }
    /* NOTREACHED */

}

static int
inet_pton4(const char *src, unsigned char *dst) {
        static const char digits[] = "0123456789";
        int saw_digit, octets, ch;
        unsigned char tmp[NS_INADDRSZ], *tp;

        saw_digit = 0;
        octets = 0;
        *(tp = tmp) = 0;
        while ((ch = *src++) != '\0') {
                const char *pch;

                if ((pch = strchr(digits, ch)) != NULL) {
                        unsigned int new_v = *tp * 10;

                        new_v += (int)(pch - digits);
                        if (saw_digit && *tp == 0)
                                return (0);
                        if (new_v > 255)
                                return (0);
                        *tp = new_v;
                        if (!saw_digit) {
                                if (++octets > 4)
                                        return (0);
                                saw_digit = 1;
                        }
                } else if (ch == '.' && saw_digit) {
                        if (octets == 4)
                                return (0);
                        *++tp = 0;
                        saw_digit = 0;
                } else
                        return (0);
        }
        if (octets < 4)
                return (0);
        memmove(dst, tmp, NS_INADDRSZ);
        return (1);
}
static int inet_pton6(const char *src, unsigned char *dst) {
        static const char xdigits_l[] = "0123456789abcdef",
                          xdigits_u[] = "0123456789ABCDEF";
        unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
        const char *xdigits, *curtok;
        int ch, seen_xdigits;
        unsigned int val;

        memset((tp = tmp), '\0', NS_IN6ADDRSZ);
        endp = tp + NS_IN6ADDRSZ;
        colonp = NULL;
        /* Leading :: requires some special handling. */
        if (*src == ':')
                if (*++src != ':')
                        return (0);
        curtok = src;
        seen_xdigits = 0;
        val = 0;
        while ((ch = *src++) != '\0') {
                const char *pch;

                if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
                        pch = strchr((xdigits = xdigits_u), ch);
                if (pch != NULL) {
                        val <<= 4;
                        val |= (pch - xdigits);
                        if (++seen_xdigits > 4)
                                return (0);
                        continue;
                }
                if (ch == ':') {
                        curtok = src;
                        if (!seen_xdigits) {
                                if (colonp)
                                        return (0);
                                colonp = tp;
                                continue;
                        }
                        if (tp + NS_INT16SZ > endp)
                                return (0);
                        *tp++ = (unsigned char) (val >> 8) & 0xff;
                        *tp++ = (unsigned char) val & 0xff;
                        seen_xdigits = 0;
                        val = 0;
                        continue;
                }
                if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
                    inet_pton4(curtok, tp) > 0) {
                        tp += NS_INADDRSZ;
                        seen_xdigits = 0;
                        break;  /* '\0' was seen by inet_pton4(). */
                }
                return (0);
        }
        if (seen_xdigits) {
                if (tp + NS_INT16SZ > endp)
                        return (0);
                *tp++ = (unsigned char) (val >> 8) & 0xff;
                *tp++ = (unsigned char) val & 0xff;
        }
        if (colonp != NULL) {
                /*
                 * Since some memmove()'s erroneously fail to handle
                 * overlapping regions, we'll do the shift by hand.
                 */
                const int n = (int)(tp - colonp);
                int i;

                if (tp == endp)
                        return (0);
                for (i = 1; i <= n; i++) {
                        endp[- i] = colonp[n - i];
                        colonp[n - i] = 0;
                }
                tp = endp;
        }
        if (tp != endp)
                return (0);
        memmove(dst, tmp, NS_IN6ADDRSZ);
        return (1);
}

#endif




}	// EOF namespace compat
}	// EOF namespace ppl7


