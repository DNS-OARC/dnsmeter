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

#ifndef PPL7TYPES_H_
#define PPL7TYPES_H_

#ifndef _PPL7_INCLUDE
    #ifdef PPL7LIB
        #include "ppl7.h"
    #else
        #include <ppl7.h>
    #endif
#endif

#include <string>
#include <iostream>
#include <map>

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

namespace ppl7 {

class String;
class WideString;
class Array;
class AssocArray;
class ByteArray;
class ByteArrayPtr;
class DateTime;
class Pointer;

class Variant
{
	public:
		enum DataType {
			TYPE_UNKNOWN		=0,
			TYPE_STRING			=4,
			TYPE_ASSOCARRAY		=5,
			TYPE_BYTEARRAY		=6,
			TYPE_POINTER		=7,
			TYPE_WIDESTRING		=8,
			TYPE_ARRAY			=9,
			TYPE_DATETIME		=10,
			TYPE_BYTEARRAYPTR	=12
		};
	private:
		void		*value;
		DataType	t;
	public:
		Variant();
		~Variant();
		Variant(const Variant &value);
		Variant(const String &value);
		Variant(const WideString &value);
		Variant(const Array &value);
		Variant(const AssocArray &value);
		Variant(const ByteArray &value);
		Variant(const ByteArrayPtr &value);
		Variant(const DateTime &value);
		Variant(const Pointer &value);

		void clear();
		void set(const Variant &value);
		void set(const String &value);
		void set(const WideString &value);
		void set(const Array &value);
		void set(const AssocArray &value);
		void set(const ByteArray &value);
		void set(const ByteArrayPtr &value);
		void set(const DateTime &value);
		void set(const Pointer &value);

		DataType type() const;
		bool isType(DataType type) const;
		bool isString() const;
		bool isWideString() const;
		bool isArray() const;
		bool isAssocArray() const;
		bool isByteArray() const;
		bool isByteArrayPtr() const;
		bool isDateTime() const;
		bool isPointer() const;

		const String& toString() const;
		String& toString();
		const WideString& toWideString() const;
		WideString& toWideString();
		const Array& toArray() const;
		Array& toArray();
		const AssocArray& toAssocArray() const;
		AssocArray& toAssocArray();
		const ByteArray& toByteArray() const;
		ByteArray& toByteArray();
		const ByteArrayPtr& toByteArrayPtr() const;
		ByteArrayPtr& toByteArrayPtr();
		const DateTime& toDateTime() const;
		DateTime& toDateTime();
		const Pointer& toPointer() const;
		Pointer& toPointer();

		operator String() const;
		operator WideString() const;
		operator Array() const;
		operator AssocArray() const;
		operator ByteArray() const;
		operator ByteArrayPtr() const;
		operator DateTime() const;
		operator Pointer() const;

		Variant &operator=(const Variant &other);
		Variant &operator=(const String &other);
		Variant &operator=(const WideString &other);
		Variant &operator=(const Array &other);
		Variant &operator=(const AssocArray &other);
		Variant &operator=(const ByteArray &other);
		Variant &operator=(const ByteArrayPtr &other);
		Variant &operator=(const DateTime &other);
		Variant &operator=(const Pointer &other);

		bool operator==(const Variant &other) const;
		bool operator!=(const Variant &other) const;
};


class Pointer
{
	private:
		void *myptr;
	public:
		Pointer();
		Pointer(const Pointer &other);
		Pointer(void *ptr);
		bool isNull() const;
		const void *ptr() const;
		operator const void*() const;
		operator const char*() const;
		operator const unsigned char*() const;
		void set(const void *ptr);
		Pointer &operator=(const Pointer &other);
		Pointer &operator=(const void *ptr);
		bool operator<(const Pointer &other) const;
		bool operator<=(const Pointer &other) const;
		bool operator==(const Pointer &other) const;
		bool operator!=(const Pointer &other) const;
		bool operator>=(const Pointer &other) const;
		bool operator>(const Pointer &other) const;
};

class ByteArrayPtr
{
	friend class ByteArray;
	private:
		void *ptradr;
		size_t ptrsize;
	public:
		ByteArrayPtr();
		ByteArrayPtr(const ByteArrayPtr &other);
		ByteArrayPtr(const String &data);
		ByteArrayPtr(const WideString &data);
		ByteArrayPtr(void *adr, size_t size);
		ByteArrayPtr(const void *adr, size_t size);
		bool isNull() const;
		bool isEmpty() const;
		size_t size() const;
		const void *adr() const;
		const void *ptr() const;
		void use(void *adr, size_t size);
		void use(const ByteArrayPtr &other);
		ByteArrayPtr &operator=(const ByteArrayPtr &other);
		operator const void*() const;
		operator const char*() const;
		operator const unsigned char*() const;
		String toHex() const;
		String toBase64() const;
		const char* toCharPtr() const;
		String md5() const;
		ppluint32 crc32() const;
		unsigned char operator[](size_t pos) const;
		void set(size_t pos, unsigned char value);
		unsigned char get(size_t pos) const;
		void hexDump() const;
		void hexDump(size_t bytes) const;
		void hexDump(size_t offset, size_t bytes) const;
		void memset(int value);
		int memcmp(const ByteArrayPtr &other) const;

		bool operator<(const ByteArrayPtr &other) const;
		bool operator<=(const ByteArrayPtr &other) const;
		bool operator==(const ByteArrayPtr &other) const;
		bool operator!=(const ByteArrayPtr &other) const;
		bool operator>=(const ByteArrayPtr &other) const;
		bool operator>(const ByteArrayPtr &other) const;
};
std::ostream& operator<<(std::ostream& s, const ByteArrayPtr &bap);

class ByteArray : public ByteArrayPtr
{
private:

public:
	ByteArray();
	ByteArray(const ByteArrayPtr &other);
	ByteArray(const ByteArray &other);
	ByteArray(const String &str);
	ByteArray(const void *adr, size_t size);
	ByteArray(size_t size);
	~ByteArray();
	void useadr(void *adr, size_t size);
	void *copy(const void *adr, size_t size);
	void *copy(const ByteArrayPtr &other);
	void *append(void *adr, size_t size);
	void *append(const ByteArrayPtr &other);
	void *prepend(void *adr, size_t size);
	void *prepend(const ByteArrayPtr &other);
	void *fromHex(const String &hex);
	void *fromBase64(const String &base64);
	void *malloc(size_t size);
	void *calloc(size_t size);
	void free();
	void clear();
	ByteArray &operator=(const ByteArrayPtr &other);
	ByteArray &operator=(const ByteArray &other);
	ByteArray &operator=(const String &str);
	operator const void*() const;
	operator const unsigned char*() const;
	operator const char*() const;
	unsigned char operator[](size_t pos) const;

	bool operator<(const ByteArrayPtr &other) const;
	bool operator<=(const ByteArrayPtr &other) const;
	bool operator==(const ByteArrayPtr &other) const;
	bool operator!=(const ByteArrayPtr &other) const;
	bool operator>=(const ByteArrayPtr &other) const;
	bool operator>(const ByteArrayPtr &other) const;

};
std::ostream& operator<<(std::ostream& s, const ByteArray &ba);

ByteArray fromBase64(const String &base64);

class String
{
	private:
		char *ptr;
		size_t s, stringlen;
	public:
		//! @name Konstruktoren und Destruktor
		//@{

		String() throw();
		String(const char *str);
		String(const char *str, size_t size);
		String(const String *str);
		String(const String &str);
		String(const WideString *str);
		String(const WideString &str);
		String(const std::string &str);
		String(const std::wstring &str);
		~String() throw();
#ifdef WITH_QT
		String(const QString &q) {
			ptr=NULL; stringlen=0; s=0;
			QByteArray a=q.toLocal8Bit();
			set((const char*)a);
		}
		String(QString *q) {
			ptr=NULL; stringlen=0; s=0;
			QByteArray a=q->toLocal8Bit();
			set((const char*)a);
		}
#endif
#ifdef _PPL6_INCLUDE
		String(const ppl6::CString &q) {
			ptr=NULL; stringlen=0; s=0;
			set(q.GetPtr(),q.Size());
		}
		String(const ppl6::CString *q) {
			ptr=NULL; stringlen=0; s=0;
			set(q->GetPtr(),q->Size());
		}
#endif
		//@}

		//! @name Statische Funktionen
		//@{
		static void setGlobalEncoding(const char *encoding);
		static const char *getGlobalEncoding();
		//@}

		void clear() throw();
		size_t capacity ( ) const;
		void reserve(size_t size);
		size_t len() const;
		size_t length() const;
		size_t size() const;
		bool	isEmpty() const;
		bool	notEmpty() const;
		bool	isNumeric() const;
		bool	isInteger() const;
		bool	isTrue() const;
		bool	isFalse() const;

		int strcmp(const String &str, size_t size=(size_t)-1) const;
		int strCaseCmp(const String &str, size_t size=(size_t)-1) const;
		int strcmp(const char *str, size_t size=(size_t)-1) const;
		int strCaseCmp(const char *str, size_t size=(size_t)-1) const;
		String left(size_t len) const;
		String right(size_t len) const;
		String mid(size_t start, size_t len=(size_t)-1) const;
		String substr(size_t start, size_t len=(size_t)-1) const;


		//! @name String setzen und verändern
		//@{
		String & set(const char *str, size_t size=(size_t)-1);
		String & set(const String *str, size_t size=(size_t)-1);
		String & set(const String &str, size_t size=(size_t)-1);
		String & set(const WideString *str, size_t size=(size_t)-1);
		String & set(const WideString &str, size_t size=(size_t)-1);
		String & set(const std::string &str, size_t size=(size_t)-1);
		String & set(const std::wstring &str, size_t size=(size_t)-1);
		String & set(const wchar_t *str, size_t size = (size_t)-1);
		String & set(char c);
		String & set(size_t position, char c);
		String & setf(const char *fmt, ...);
		String & useadr(void *adr, size_t size, size_t stringlen=(size_t)-1);

		String & append(const char *str, size_t size=(size_t)-1);
		String & append(const String *str, size_t size=(size_t)-1);
		String & append(const String &str, size_t size=(size_t)-1);
		String & append(const std::string &str, size_t size=(size_t)-1);
		String & append(const std::wstring &str, size_t size=(size_t)-1);
		String & append(const wchar_t *str, size_t size=(size_t)-1);
		String & appendf(const char *fmt, ...);
		String & append(char c);

		String & prepend(const char *str, size_t size = (size_t)-1);
		String & prepend(const String *str, size_t size = (size_t)-1);
		String & prepend(const String &str, size_t size = (size_t)-1);
		String & prepend(const std::string &str, size_t size = (size_t)-1);
		String & prepend(const std::wstring &str, size_t size = (size_t)-1);
		String & prepend(const wchar_t *str, size_t size = (size_t)-1);
		String & prependf(const char *fmt, ...);
		String & prepend(char c);

		String & vasprintf(const char *fmt, va_list args);

		String& repeat(size_t num);
		String& repeat(char code, size_t num);
		String& repeat(const String& str, size_t num);
		String repeated(size_t num) const;


		void lowerCase();
		void upperCase();
		void upperCaseWords();
		void trim();
		String trimmed() const;
		String toLowerCase() const;
		String toUpperCase() const;
		String toUpperCaseWords() const;
		void trimLeft();
		void trimRight();
		void trim(const String &chars);
		void trimLeft(const String &chars);
		void trimRight(const String &chars);
		void chopRight(size_t num=1);
		void chop(size_t num=1);
		void chopLeft(size_t num=1);
		void chomp();
		void cut(size_t pos);
		void cut(const String &letter);

	    String strchr(char c) const;
	    String strrchr(char c) const;
	    String strstr(const String &needle) const;
	    ssize_t find(const String &needle, ssize_t start=0) const;
	    ssize_t findCase(const String &needle, ssize_t start=0) const;
	    ssize_t instr(const String &needle, size_t start=0) const;
	    ssize_t instrCase(const String &needle, size_t start=0) const;
	    bool has(const String &needle, bool ignoreCase=false) const;

	    String& stripSlashes();

	    String& replace(const String &search, const String &replacement);
	    String& pregReplace(const String &expression, const String &replacement, int max=0);
	    String& pregEscape();

	    bool pregMatch(const String &expression) const;
	    bool pregMatch(const String &expression, Array &matches, size_t maxmatches=16) const;
		//@}

		//! @name String ausgeben und auslesen
		//@{
		void print(bool withNewline=false) const throw();
		void printnl() const throw();
		void hexDump() const;
		char get(ssize_t pos) const;
		const char* getPtr() const;
		const char* c_str() const;

		ByteArray toEncoding(const char *encoding) const;
		ByteArray toUCS4() const;
		ByteArray toUtf8() const;
		String &fromUCS4(const ppluint32 *str, size_t size=(size_t)-1);
		String &fromUCS4(const ByteArrayPtr &bin);
		String md5() const;

		int toInt() const;
		unsigned int toUnsignedInt() const;
		pplint64 toInt64() const;
		ppluint64 toUnsignedInt64() const;
		WideString toWideString() const;
		bool toBool() const;
		long toLong() const;
		unsigned long toUnsignedLong() const;
		long long toLongLong() const;
		unsigned long long toUnsignedLongLong() const;
		float toFloat() const;
		double toDouble() const;
		const char * toChar() const;

		//@}

		//! @name Operatoren
		//@{
		operator const char *() const;
		operator const unsigned char *() const;
		operator int() const;
		operator unsigned int() const;
		operator bool() const;
		operator long() const;
		operator unsigned long() const;
		operator long long () const;
		operator unsigned long long () const;
		operator float () const;
		operator double () const;
		operator std::string() const;
		operator std::wstring() const;

		char operator[](ssize_t pos) const;

		String& operator=(const char* str);
		String& operator=(const wchar_t* str);
		String& operator=(const String *str);
		String& operator=(const String &str);
		String& operator=(const std::string &str);
		String& operator=(const std::wstring &str);
		String& operator=(char c);
		String& operator+=(const char* str);
		String& operator+=(const wchar_t* str);
		String& operator+=(const String& str);
		String& operator+=(const std::string &str);
		String& operator+=(const std::wstring &str);
		String& operator+=(char c);
		bool operator<(const String &str) const;
		bool operator<=(const String &str) const;
		bool operator==(const String &str) const;
		bool operator!=(const String &str) const;
		bool operator>=(const String &str) const;
		bool operator>(const String &str) const;

		bool operator<(const char *str) const;
		bool operator<=(const char *str) const;
		bool operator==(const char *str) const;
		bool operator!=(const char *str) const;
		bool operator>=(const char *str) const;
		bool operator>(const char *str) const;

		//@}

#ifdef WITH_QT
		//! @name Operatoren zur Verwendung der Klasse mit Qt
		//@{
		operator const QString() const {
			return QString::fromLocal8Bit (ptr,stringlen);
		}

		operator const QVariant() const {
			QVariant v=QString::fromLocal8Bit (ptr,stringlen);
			return v;
		}

		String& operator=(const QString& q) {
			QByteArray a=q.toLocal8Bit();
			set((const char*)a);
			return *this;
		}
		String& operator=(const QString *q) {
			QByteArray a=q->toLocal8Bit();
			set((const char*)a);
			return *this;
		}
		//@}
#endif
#ifdef _PPL6_INCLUDE
		//! @name Operatoren zur Verwendung der Klasse mit ppl6
		//@{
		operator const ppl6::CString() const {
			return ppl6::CString(ptr,stringlen);
		}
		ppl6::CString toPpl6CString() const {
			return ppl6::CString(ptr,stringlen);
		}

		String& operator=(const ppl6::CString &q) {
			set(q.GetPtr(),q.Size());
			return *this;
		}
		String& operator=(const ppl6::CString *q) {
			set(q->GetPtr(),q->Size());
			return *this;
		}
		String& operator+=(const ppl6::CString& str) {
			ppl7::String p7(str);
			return (*this)+=p7;
		}

		//@}

#endif
};

String operator+(const String &str1, const String& str2);
String operator+(const char *str1, const String& str2);
String operator+(const String &str1, const char *str2);
String operator+(const wchar_t *str1, const String& str2);
String operator+(const String &str1, const wchar_t *str2);
String operator+(const std::string &str1, const String& str2);
String operator+(const String &str1, const std::string &str2);
String operator+(const std::wstring &str1, const String& str2);
String operator+(const String &str1, const std::wstring &str2);

std::ostream& operator<<(std::ostream& s, const String &str);

class WideString
{
	private:
		wchar_t *ptr;
		size_t s, stringlen;
	public:
		//! @name Konstruktoren und Destruktor
		//@{

		WideString() throw();
		WideString(const wchar_t *str);
		WideString(const wchar_t *str, size_t size);
		WideString(const WideString *str);
		WideString(const WideString &str);
		WideString(const String *str);
		WideString(const String &str);
		WideString(const std::string &str);
		WideString(const std::wstring &str);
		~WideString() throw();
#ifdef WITH_QT
		WideString(const QString &q) {
			ptr=NULL; stringlen=0; s=0;
			QByteArray a=q.toUtf8();
			set((const char*)a);
		}
		WideString(QString *q) {
			ptr=NULL; stringlen=0; s=0;
			QByteArray a=q->toUtf8();
			set((const char*)a);
		}
#endif
		//@}

		//! @name Statische Funktionen
		//@{
		static void setGlobalEncoding(const char *encoding);
		//@}

		void clear() throw();
		size_t capacity ( ) const;
		void reserve(size_t size);
		size_t len() const;
		size_t length() const;
		size_t size() const;
		size_t byteLength() const;
		bool	isEmpty() const;
		bool	notEmpty() const;
		bool	isNumeric() const;
		bool	isInteger() const;
		bool	isTrue() const;
		bool	isFalse() const;

		int strcmp(const WideString &str, size_t size=(size_t)-1) const;
		int strCaseCmp(const WideString &str, size_t size=(size_t)-1) const;
		WideString left(size_t len) const;
		WideString right(size_t len) const;
		WideString mid(size_t start, size_t len=(size_t)-1) const;
		WideString substr(size_t start, size_t len=(size_t)-1) const;


		//! @name String setzen und verändern
		//@{

		WideString & set(const char *str, size_t size = (size_t)-1);
		WideString & set(const wchar_t *str, size_t size = (size_t)-1);
		WideString & set(const WideString *str, size_t size = (size_t)-1);
		WideString & set(const WideString &str, size_t size = (size_t)-1);
		WideString & set(const String *str, size_t size = (size_t)-1);
		WideString & set(const String &str, size_t size = (size_t)-1);
		WideString & set(const std::string &str, size_t size = (size_t)-1);
		WideString & set(const std::wstring &str, size_t size = (size_t)-1);
		WideString & set(wchar_t c);
		WideString & set(size_t position, wchar_t c);
		WideString & setf(const char *fmt, ...);

		WideString & append(const char *str, size_t size = (size_t)-1);
		WideString & append(const wchar_t *str, size_t size = (size_t)-1);
		WideString & append(const WideString *str, size_t size=(size_t)-1);
		WideString & append(const WideString &str, size_t size=(size_t)-1);
		WideString & append(const std::string &str, size_t size=(size_t)-1);
		WideString & append(const std::wstring &str, size_t size=(size_t)-1);
		WideString & appendf(const char *fmt, ...);
		WideString & append(wchar_t c);

		WideString & prepend(const char *str, size_t size = (size_t)-1);
		WideString & prepend(const wchar_t *str, size_t size = (size_t)-1);
		WideString & prepend(const WideString *str, size_t size=(size_t)-1);
		WideString & prepend(const WideString &str, size_t size=(size_t)-1);
		WideString & prepend(const std::string &str, size_t size=(size_t)-1);
		WideString & prepend(const std::wstring &str, size_t size=(size_t)-1);
		WideString & prependf(const char *fmt, ...);
		WideString & prepend(wchar_t c);

		WideString & vasprintf(const char *fmt, va_list args);
		WideString& repeat(size_t num);
		WideString& repeat(wchar_t unicode, size_t num);
		WideString& repeat(const WideString& str, size_t num);
		WideString repeated(size_t num) const;


		void lowerCase();
		void upperCase();
		void upperCaseWords();
		void trim();
		void trimLeft();
		void trimRight();
		void trim(const WideString &chars);
		void trimLeft(const WideString &chars);
		void trimRight(const WideString &chars);
		void chopRight(size_t num=1);
		void chop(size_t num=1);
		void chopLeft(size_t num=1);
		void chomp();
		void cut(size_t pos);
		void cut(const WideString &letter);

	    WideString strchr(wchar_t c) const;
	    WideString strrchr(wchar_t c) const;
	    WideString strstr(const WideString &needle) const;
	    ssize_t find(const WideString &needle, ssize_t start=0) const;
	    ssize_t findCase(const WideString &needle, ssize_t start=0) const;
	    ssize_t instr(const WideString &needle, size_t start=0) const;
	    ssize_t instrCase(const WideString &needle, size_t start=0) const;

	    WideString& stripSlashes();

	    WideString& replace(const WideString &search, const WideString &replacement);
	    WideString& pregReplace(const WideString &expression, const WideString &replacement, int max=0);
	    WideString& pregEscape();

	    bool pregMatch(const WideString &expression) const;
	    bool pregMatch(const WideString &expression, Array &matches, size_t maxmatches=16) const;
		//@}

		//! @name String ausgeben und auslesen
		//@{
		void print(bool withNewline=false) const throw();
		void printnl() const throw();
		void hexDump() const;
		wchar_t get(ssize_t pos) const;
		const wchar_t* getPtr() const;

		ByteArray toUtf8() const;
		String toString() const;
		ByteArray toLocalEncoding() const;
		String toLocalString() const;
		ByteArray toEncoding(const char *encoding) const;
		ByteArray toUCS4() const;
		WideString &fromUCS4(const ppluint32 *str, size_t size=(size_t)-1);
		WideString &fromUCS4(const ByteArrayPtr &bin);

		int toInt() const;
		unsigned int toUnsignedInt() const;
		pplint64 toInt64() const;
		ppluint64 toUnsignedInt64() const;
		bool toBool() const;
		long toLong() const;
		unsigned long toUnsignedLong() const;
		long long toLongLong() const;
		unsigned long long toUnsignedLongLong() const;
		float toFloat() const;
		double toDouble() const;
		const wchar_t * toWchart() const;

		//@}

		//! @name Operatoren
		//@{
		operator const wchar_t *() const;
		operator int() const;
		operator unsigned int() const;
		operator bool() const;
		operator long() const;
		operator unsigned long() const;
		operator long long () const;
		operator unsigned long long () const;
		operator float () const;
		operator double () const;
		operator std::string() const;
		operator std::wstring() const;

		wchar_t operator[](ssize_t pos) const;

		WideString& operator=(const char* str);
		WideString& operator=(const wchar_t* str);
		WideString& operator=(const WideString *str);
		WideString& operator=(const WideString &str);
		WideString& operator=(const String *str);
		WideString& operator=(const String &str);
		WideString& operator=(const std::string &str);
		WideString& operator=(const std::wstring &str);
		WideString& operator=(wchar_t c);
		WideString& operator+=(const char* str);
		WideString& operator+=(const wchar_t* str);
		WideString& operator+=(const WideString& str);
		WideString& operator+=(const std::string &str);
		WideString& operator+=(const std::wstring &str);
		WideString& operator+=(wchar_t c);


		bool operator<(const WideString &str) const;
		bool operator<=(const WideString &str) const;
		bool operator==(const WideString &str) const;
		bool operator!=(const WideString &str) const;
		bool operator>=(const WideString &str) const;
		bool operator>(const WideString &str) const;

		bool operator<(const wchar_t *str) const;
		bool operator<=(const wchar_t *str) const;
		bool operator==(const wchar_t *str) const;
		bool operator!=(const wchar_t *str) const;
		bool operator>=(const wchar_t *str) const;
		bool operator>(const wchar_t *str) const;

		//@}

#ifdef WITH_QT
		//! @name Operatoren zur Verwendung der Klasse mit Qt
		//@{
		operator const QString() const {
			return QString::fromWCharArray (ptr,stringlen);
		}

		operator const QVariant() const {
			QVariant v=QString::fromWCharArray (ptr,stringlen);
			return v;
		}

		WideString& operator=(const QString& q) {
			QByteArray a=q.toUtf8();
			set((const char*)a);
			return *this;
		}
		WideString& operator=(const QString *q) {
			QByteArray a=q->toUtf8();
			set((const char*)a);
			return *this;
		}
		//@}
#endif
};

WideString operator+(const WideString &str1, const WideString& str2);
WideString operator+(const char *str1, const WideString& str2);
WideString operator+(const WideString &str1, const char *str2);
WideString operator+(const wchar_t *str1, const WideString& str2);
WideString operator+(const WideString &str1, const wchar_t *str2);
WideString operator+(const std::string &str1, const WideString& str2);
WideString operator+(const WideString &str1, const std::string &str2);
WideString operator+(const std::wstring &str1, const WideString& str2);
WideString operator+(const WideString &str1, const std::wstring &str2);

std::ostream& operator<<(std::ostream& s, const WideString &str);



class Array
{
	private:
		size_t numElements;
		size_t numCapacity;
		void *rows;

	public:
		class Iterator
		{
			friend class Array;
			private:
			size_t pos;
			public:
				Iterator();
		};

		//! @name Konstruktoren und Destruktor
		//@{
		Array();
		Array(const Array &other);
		Array(const String &str, const String &delimiter=String("\n"), size_t limit=0, bool skipemptylines=false);
		~Array();
		//@}

		//! @name Elemente hinzufügen/verändern
		//@{
		void add(const String &value);
		void add(const String &value, size_t size);
		void add(const char *value, size_t size);
		void add(const Array &other);
		void addf(const char *fmt, ...);
		void copy(const Array &other);
		void set(size_t index, const String &value);
		void setf(size_t index, const char *fmt, ...);
		void insert(size_t index, const String &value);
		void insert(size_t index, const Array &other);
		void insertf(size_t index, const char *fmt, ...);
		Array &fromArgs(int argc, const char **argv);
		Array &fromArgs(const String &args);
		Array &explode(const String &text, const String &delimiter="\n", size_t limit=0, bool skipemptylines=false);
		//@}

		//! @name Elemente löschen
		//@{
		void clear();
		String erase(size_t index);
		String shift();
		String pop();
		//@}

		//! @name Elemente auslesen
		//@{
		const String &get(ssize_t index) const;
		const String &getRandom() const;
		String &get(ssize_t index);
		String &getRandom();
		String getRest(size_t index, const String &delimiter=" ");
		const char *getPtr(ssize_t index) const;
		const char *getRandomPtr() const;
		void reset(Iterator &it) const;
		const String &getFirst(Iterator &it) const;
		const String &getNext(Iterator &it) const;
		String implode(const String &delimiter="\n") const;
		//@}

		//! @name Sonstiges
		//@{
		size_t count() const;
		size_t size() const;
		size_t capacity() const;
		bool empty() const;
		void reserve (size_t size);
		void list(const String &prefix=String()) const;
		void sort();
		void sortReverse();
		void sortUnique();
		void makeUnique();

		size_t indexOf(const String &search);
		bool has(const String &search);
		//@}

		//! @name Operatoren
		//@{
		String &operator[](ssize_t index);
		const String &operator[](ssize_t index) const;
		Array& operator=(const Array &other);
		Array& operator+=(const Array &other);
		bool operator==(const Array &other) const;
		bool operator!=(const Array &other) const;
		//@}
};

Array operator+(const Array &a1, const Array& a2);

class AssocArray
{
	private:
		class ArrayKey : public String
		{
			private:
				int compare(const ArrayKey &str) const;
			public:
				ArrayKey();
				ArrayKey(const String &other);
				ArrayKey& operator=(const String &str);
				bool operator<(const ArrayKey &str) const;
				bool operator<=(const ArrayKey &str) const;
				bool operator==(const ArrayKey &str) const;
				bool operator!=(const ArrayKey &str) const;
				bool operator>=(const ArrayKey &str) const;
				bool operator>(const ArrayKey &str) const;
		};

		std::map<ArrayKey, Variant*> Tree;
		ppluint64		maxint;

		Variant *findInternal(const ArrayKey &key) const;
		void createTree(const ArrayKey &key, Variant *var);

	public:
		PPL7EXCEPTION(InvalidKeyException, Exception);
		PPL7EXCEPTION(ExportBufferToSmallException, Exception);
		PPL7EXCEPTION(ImportFailedException, Exception);

		typedef std::map<ArrayKey, Variant*>::iterator iterator;
		typedef std::map<ArrayKey, Variant*>::const_iterator const_iterator;
		typedef std::map<ArrayKey, Variant*>::reverse_iterator reverse_iterator;
		typedef std::map<ArrayKey, Variant*>::const_reverse_iterator const_reverse_iterator;

		class Iterator
		{
			private:
				friend class AssocArray;
				const_iterator it;
				Variant empty;
				bool reset;
			public:
				Iterator() { reset=true; }
				const String &key() { return (*it).first; }
				const Variant &value() {
					if ((*it).second==NULL) return empty;
					return *(*it).second;
				};
		};
		class ReverseIterator
		{
			private:
				friend class AssocArray;
				const_reverse_iterator it;
				Variant empty;
				bool reset;
			public:
				ReverseIterator() { reset=true; }
				const String &key() { return (*it).first; }
				const Variant &value() {
					if ((*it).second==NULL) return empty;
					return *(*it).second;
				};
		};


		//!\name Konstruktoren und Destruktoren
		//@{
		AssocArray();
		AssocArray(const AssocArray &other);
		~AssocArray();
		//@}

		//!\name Informationen ausgeben/auslesen
		//@{
		size_t count(bool recursive=false) const;
		size_t count(const String &key, bool recursive=false) const;
		size_t size() const;
		void list(const String &prefix="") const;

		//@}

		//!\name Werte setzen
		//@{
		void add(const AssocArray &other);
		void set(const String &key, const String &value);
		void set(const String &key, const String &value, size_t size);
		void set(const String &key, const WideString &value);
		void set(const String &key, const Array &value);
		void set(const String &key, const DateTime &value);
		void set(const String &key, const ByteArray &value);
		void set(const String &key, const ByteArrayPtr &value);
		void set(const String &key, const AssocArray &value);
		void set(const String &key, const Pointer &value);
		void set(const String &key, const Variant &value);
		void setf(const String &key, const char *fmt, ...);
		//@}

		//!\name Werte erweitern (nur Strings)
		//@{
		void append(const String &key, const String &value, const String &concat="");
		void appendf(const String &key, const String &concat, const char *fmt, ...);
		//@}

		//!\name Werte löschen
		//@{
		void clear();
		void erase(const String &key);
		void remove(const String &key);
		//@}

		//!\name Import und Export von Daten
		//@{
		size_t	fromTemplate(const String &templ, const String &linedelimiter="\n", const String &splitchar="=", const String &concat="\n", bool dotrim=false);
		size_t	fromConfig(const String &content, const String &linedelimiter="\n", const String &splitchar="=", const String &concat="\n", bool dotrim=false);
		void toTemplate(String &s, const String &prefix="", const String &linedelimiter="\n", const String &splitchar="=") const;
		size_t binarySize() const;
		void exportBinary(void *buffer, size_t buffersize, size_t *realsize) const;
		void exportBinary(ByteArray &buffer) const;
		size_t importBinary(const void *buffer, size_t buffersize);
		void importBinary(const ByteArrayPtr &buffer);
		//@}

		//!\name Werte direkt auslesen
		//@{
		Variant	&get(const String &key) const;
		String	&getString(const String &key) const;
		String	&getString(const String &key, String &default_value) const;
		int		getInt(const String &key) const;
		int		getInt(const String &key, int default_value) const;
		long long		getLongLong(const String &key) const;
		long long		getLongLong(const String &key, long long default_value) const;
		AssocArray	&getAssocArray(const String &key) const;
		AssocArray	&getAssocArray(const String &key, AssocArray &default_value) const;
		Array	&getArray(const String &key) const;
		Array	&getArray(const String &key, Array &default_value) const;
		bool	exists(const String &key) const;
		bool	isTrue(const String &key) const;

		//@}

		//!\name Array durchwandern
		//@{
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
		reverse_iterator rbegin();
		const_reverse_iterator rbegin() const;
		reverse_iterator rend();
		const_reverse_iterator rend() const;

		void reset(Iterator &it) const;
		void reset(ReverseIterator &it) const;
		bool getFirst(Iterator &it, Variant::DataType type=Variant::TYPE_UNKNOWN) const;
		bool getNext(Iterator &it, Variant::DataType type=Variant::TYPE_UNKNOWN) const;
		bool getLast(ReverseIterator &it, Variant::DataType type=Variant::TYPE_UNKNOWN) const;
		bool getPrevious(ReverseIterator &it, Variant::DataType type=Variant::TYPE_UNKNOWN) const;

		bool getFirst(Iterator &it, String &key, String &value) const;
		bool getNext(Iterator &it, String &key, String &value) const;
		bool getLast(ReverseIterator &it, String &key, String &value) const;
		bool getPrevious(ReverseIterator &it, String &key, String &value) const;
		//@}

		//!\name Operatoren
		//@{
		Variant &operator[](const String &key);
		const Variant &operator[](const String &key) const;
		AssocArray& operator=(const AssocArray& other);
		AssocArray& operator+=(const AssocArray& other);

		bool operator==(const AssocArray &other);
		bool operator!=(const AssocArray &other);
		//@}
};
AssocArray operator+(const AssocArray &a1, const AssocArray& a2);

//! \brief Eine Struktur zum Erfassen von Uhrzeit und Datum
typedef struct tagTime {
	ppluint64	epoch;
	int			year;
	int			month;
	int			day;
	int			hour;
	int			min;
	int			sec;
	int			day_of_week;
	int			day_of_year;
	int			summertime;
	int			gmt_offset;
	int			have_gmt_offset;
} PPLTIME;

//! \brief Datentyp für Unix-Time
typedef ppluint64 ppl_time_t;

class DateTime
{
	private:
		ppluint32 us;
		ppluint16 yy;
		ppluint8 mm;
		ppluint8 dd;
		ppluint8 hh;
		ppluint8 ii;
		ppluint8 ss;

	public:
		DateTime();
		DateTime(const String &datetime);
		DateTime(const DateTime &other);
		DateTime(ppluint64 t);

		void setTime_t(ppluint64 t);
		void setEpoch(ppluint64 t);
		void setLongInt(ppluint64 i);
		void set(const String &datetime);
		void set(const DateTime &other);
		void set(const String &date, const String &time);
		void set(const PPLTIME &t);
		void setDate(const String &date);
		void setTime(const String &time);
		void set(int year, int month, int day, int hour=0, int minute=0, int sec=0, int msec=0, int usec=0);
		void setCurrentTime();
		void print() const;
		void clear();
		bool notEmpty() const;
		bool isEmpty() const;
		bool isLeapYear() const;

		String get(const String &format="%Y-%m-%d %H:%M:%S") const;
		String getDate(const String &format="%Y-%m-%d") const;
		String getTime(const String &format="%H:%M:%S") const;
		String getISO8601() const;
		String getISO8601withMsec() const;
		String getISO8601withUsec() const;
		String getRFC822Date () const;
		String strftime(const String &format) const;
		ppluint64 time_t() const;
		ppluint64 epoch() const;
		ppluint64 longInt() const;

		int year() const;
		int month() const;
		int day() const;
		int hour() const;
		int minute() const;
		int second() const;
		int millisecond() const;
		int microsecond() const;
		int week() const;
		int weekISO8601() const;


		pplint64 diffSeconds(const DateTime &other) const;
		int compareSeconds(const DateTime &other, int tolerance=0) const;

		DateTime& operator=(const String &datetime);
		DateTime& operator=(const DateTime &other);


		static bool isLeapYear(int year);
		static DateTime currentTime();
		String toString() const;
		String toString(const String &format) const;
		operator String() const;

		bool operator<(const DateTime &other) const;
		bool operator<=(const DateTime &other) const;
		bool operator==(const DateTime &other) const;
		bool operator!=(const DateTime &other) const;
		bool operator>=(const DateTime &other) const;
		bool operator>(const DateTime &other) const;

};


} // EOF namespace ppl7

#endif /* PPL7TYPES_H_ */
