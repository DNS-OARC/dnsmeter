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
#include <time.h>
#ifdef _WIN32
    #include <winsock2.h>
	#include <process.h>
#else
	#ifdef HAVE_UNISTD_H
    #include <unistd.h>
	#endif
	#ifdef HAVE_SYS_SOCKET_H
    #include <sys/socket.h>
	#endif
	#ifdef HAVE_SYS_POLL_H
    #include <sys/poll.h>
	#endif
	#ifdef HAVE_NETINET_IN_H
    #include <netinet/in.h>
	#endif
	#ifdef HAVE_NETDB_H
    #include <netdb.h>
	#endif
	#ifdef HAVE_ARPA_INET_H
    #include <arpa/inet.h>
	#endif
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef NO_SSL
#undef HAVE_OPENSSL
#endif

#ifdef HAVE_OPENSSL
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#include <openssl/x509v3.h>
#endif


#include "ppl7.h"
#include "ppl7-inet.h"
#include "socket.h"

namespace ppl7 {

typedef struct {
	ppl7::Mutex *mutex;
} MUTEX_STRUCT;

#ifdef HAVE_OPENSSL
static bool SSLisInitialized=false;
static bool PRNGIsSeed=false;
//static int  SSLRefCount=0;
static Mutex	SSLMutex;
static MUTEX_STRUCT *mutex_buf=NULL;


// seed PRNG (Pseudo Random Number Generator)
static void SeedPRNG()
{
	if (PRNGIsSeed) return;		// Muss nur einmal gemacht werden
	char *buf;
	time_t  t;
#ifdef HAVE_GETUID
	uid_t   uid;
#endif
#ifdef _WIN32
	int		pid;
#else
	pid_t   pid;
#endif

	int p=0;

	// allocate buffer
	#ifdef HAVE_GETUID
		buf=(char*) malloc(sizeof(t)+sizeof(uid)*2+sizeof(pid)*2+32);
	#else
		buf=(char*) malloc(sizeof(t)+sizeof(pid)*2+32);
	#endif

	time(&t);
	memcpy(buf+p,&t,sizeof(time_t));
	p+=sizeof(time_t);

	#ifdef HAVE_GETUID
		uid=getuid();
		memcpy(buf+p,&uid,sizeof(uid_t));
		p+=sizeof(uid_t);
	#endif
	#ifdef HAVE_GETEUID
        uid=geteuid();
		memcpy(buf+p,&uid,sizeof(uid_t));
		p+=sizeof(uid_t);
	#endif

	#ifdef HAVE_GETPID
		pid=getpid();
		memcpy(buf+p,&pid,sizeof(pid));
		p+=sizeof(pid);
	#else
		#ifdef _WIN32
			pid=_getpid();
			memcpy(buf+p,&pid,sizeof(pid));
			p+=sizeof(pid);
		#endif
	#endif

	#ifdef HAVE_GETPPID
		pid=getppid();
		memcpy(buf+p,&pid,sizeof(pid));
		p+=sizeof(pid);
	#endif

	::MD5((const unsigned char*)buf,p,(unsigned char*)buf+p);
	p+=32;
	RAND_seed(buf,p);
	free(buf);
}

static void locking_function(int mode, int n, const char *file, int line)
{
	if (!mutex_buf) return;
	if (mode & CRYPTO_LOCK)
		mutex_buf[n].mutex->lock();
	else
		mutex_buf[n].mutex->unlock();
}

static unsigned long id_function(void)
{
	return (unsigned long) ppl7::ThreadID();
}

/*
static CRYPTO_dynlock_value *dyn_create_function(char *file, int line)
{
	return (CRYPTO_dynlock_value *) new ppl7::Mutex;
}

static void dyn_lock_function (int mode, CRYPTO_dynlock_value *l, const char *file, int line)
{
	if (mode & CRYPTO_LOCK)
			((ppl7::Mutex*)l)->lock();
		else
			((ppl7::Mutex*)l)->unlock();
}

static void dyn_destroy_function (CRYPTO_dynlock_value *l, const char *file, int line)
{
	delete ((ppl7::Mutex*)l);
}
*/

/*
static void ssl_exit()
{
	SSL_Exit();
}
*/

static const char * ssl_geterror(SSL *ssl, int code)
{
	int e=SSL_get_error(ssl,code);
	switch (e) {
		case SSL_ERROR_NONE: return "SSL_ERROR_NONE";
		case SSL_ERROR_ZERO_RETURN: return "SSL_ERROR_ZERO_RETURN";
		case SSL_ERROR_WANT_READ: return "SSL_ERROR_WANT_READ";
		case SSL_ERROR_WANT_WRITE: return "SSL_ERROR_WANT_WRITE";
		case SSL_ERROR_WANT_CONNECT: return "SSL_ERROR_WANT_CONNECT";
		case SSL_ERROR_WANT_ACCEPT: return "SSL_ERROR_WANT_ACCEPT";
		case SSL_ERROR_WANT_X509_LOOKUP: return "SSL_ERROR_WANT_X509_LOOKUP";
		case SSL_ERROR_SYSCALL: return "SSL_ERROR_SYSCALL";
		case SSL_ERROR_SSL: return "SSL_ERROR_SSL";
	};
	return "SSL_ERROR_UNKNOWN";
}


#endif



/*!\ingroup PPLGroupInternet
 * \brief OpenSSL-Schnittstelle initialisieren
 *
 * \desc
 * Bevor eine der SSL-Funktionen in TCPSocket und die Klasse SSLContext verwendet werden kann, muss
 * nicht nur OpenSSL beim Kompilieren der Library eingebunden werden, sondern diese bei
 * Programmstart auch initialisiert werden. Dazu muss diese Funktion aufgerufen werden.
 * \par
 * Sie initialisiert den Zufallszahlengenerator, OpenSSL für die Verwendung in einer
 * multithreaded Umgebung, sowie die verschiedenen verschlüsselungs Algorithmen.
 * \par
 * Da durch Aufruf dieser Funktion Speicher allokiert wird, muss irgendwann, wenn OpenSSL
 * nicht mehr benötigt wird, die Funktion SSL_Exit aufgerufen werden. Dies geschieht
 * bei Programmende jedoch auch automatisch.
 *
 * @exception UnsupportedFeatureException
 * @exception OutOfMemoryException
 * @exception InitializationFailedException
 *
 * \see
 * - SSL_Exit
 * - SSLContext
 * - CTCPSocket
 *
 * \relates SSLContext
 * \relates CTCPSocket
 */
void SSL_Init()
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	SSLMutex.lock();
	if (SSLisInitialized) {
		SSLMutex.unlock();
		return;
	}
	SeedPRNG();
	int max_locks=CRYPTO_num_locks();
	mutex_buf=(MUTEX_STRUCT*)malloc(max_locks * sizeof(MUTEX_STRUCT));
	if (!mutex_buf) {
		SSLMutex.unlock();
		throw OutOfMemoryException();
	}
	if (!SSL_library_init()) {
		free(mutex_buf);
		SSLMutex.unlock();
		throw InitializationFailedException("OpenSSL");
	}
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	for (int i=0;i<max_locks;i++) {
		mutex_buf[i].mutex=new Mutex;
	}
	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(locking_function);
	/*
	CRYPTO_set_dynlock_create_callback(dyn_create_function);
	CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
	CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
	*/
	SSLisInitialized=true;
	atexit(SSL_Exit);
	SSLMutex.unlock();
#endif
}

/*!\ingroup PPLGroupInternet
 * \brief OpenSSL-Schnittstelle deinitialisieren
 *
 * \desc
 * Hat die Anwendung OpenSSL verwendet, muss nach Gebrauch diese Funktion aufgerufen werden,
 * um den durch OpenSSL belegten Speicher wieder freizugeben. Dies geschieht zu Programmende
 * jedoch automatisch, so dass man sich den Aufruf in der Regel sparen kann.
 *
 * @return Die Funktion gibt 1 zurück, wenn die OpenSSL-Schnittstelle erfolgreich
 * deinitialisiert wurde, im Fehlerfall 0.
 *
 * \see
 * - SSL_Init
 * - SSLContext
 * - CTCPSocket
 *
 * \relates SSLContext
 * \relates CTCPSocket
 *
 */
void SSL_Exit()
{
	#ifdef HAVE_OPENSSL
		SSLMutex.lock();
		if (!SSLisInitialized) {
			SSLMutex.unlock();
			return;
		}
		CRYPTO_set_id_callback(NULL);
		CRYPTO_set_locking_callback(NULL);
		CRYPTO_set_dynlock_create_callback(NULL);
		CRYPTO_set_dynlock_lock_callback(NULL);
		CRYPTO_set_dynlock_destroy_callback(NULL);
		if (mutex_buf) {
			int max_locks=CRYPTO_num_locks();
			for (int i=0;i<max_locks;i++) {
				delete mutex_buf[i].mutex;
			}
			free(mutex_buf);
			mutex_buf=NULL;
		}
		SSLisInitialized=false;
		EVP_cleanup();
		ERR_free_strings();
		SSLMutex.unlock();
	#endif
}


void ClearSSLErrorStack()
{
#ifdef HAVE_OPENSSL
	while ((ERR_get_error()));
#endif
}

int GetSSLError(SSLError &e)
{
#ifdef HAVE_OPENSSL
	unsigned long ec;
	const char *file, *data;
	char ebuffer[256];
	ec=ERR_get_error_line_data(&file,&e.Line,&data,&e.Flags);
	if (ec==0) return 0;
	ERR_error_string_n(ec,ebuffer,255);
	e.Text.set(ebuffer);
	e.Filename.set(file);
	e.Data.set(data);
	return 1;
#else
	return 0;
#endif
}

int GetSSLErrors(std::list<SSLError> &e)
{
#ifdef HAVE_OPENSSL
	e.clear();
	unsigned long ec;
	const char *file, *data;
	char ebuffer[256];
	SSLError se;

	while ((ec=ERR_get_error_line_data(&file,&se.Line,&data,&se.Flags))) {
		ERR_error_string_n(ec,ebuffer,255);
		se.Text.set(ebuffer);
		se.Filename.set(file);
		se.Data.set(data);
		e.push_back(se);
	}
	return (int) e.size();
#else
	return 0;
#endif
}

int GetSSLErrors(String &e)
{
	e.clear();
	std::list<SSLError> elist;
	if (!GetSSLErrors(elist)) return 0;
	std::list<SSLError>::const_iterator it;
	for (it=elist.begin();it!=elist.end();++it) {
		e.appendf("%s:%d:%llu:%s:%s, ",(const char*)(*it).Filename,
				(*it).Line,
				(*it).Code,
				(const char*)(*it).Text,
				(const char*)(*it).Data);
	}
	e.chopRight(2);
	return (int)elist.size();
}


/*
 * SSLContext-Klasse
 */

/*!\class SSLContext
 * \ingroup PPLGroupInternet
 * \relates CTCPSocket
 *
 * \brief SSL-Schnittstelle
 *
 * \example TCP-Server mit SSL-Verschlüsselung
 * \dontinclude socket_examples.cpp
 * \skip Socket_Example3
 * \until EOF
 *
 */


SSLContext::SSLContext()
{
	ctx=NULL;
	references=0;
}

/*!\brief SSL-Kontext initialisieren
 *
 * \desc
 *
 * @exception UnsupportedFeatureException
 * @exception OutOfMemoryException
 * @exception InitializationFailedException
 * @exception IllegalArgumentException
 */
SSLContext::SSLContext(SSL_METHOD method)
{
	ctx=NULL;
	references=0;
	init(method);
}

SSLContext::~SSLContext()
{
	clear();
}

void SSLContext::clear()
{
	shutdown();
}


#ifdef HAVE_OPENSSL
static void disable_ssl_on_ctx(SSL_CTX *ctx) {
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
	SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3);
}
#endif

/*!\brief SSL-Kontext initialisieren
 *
 * \desc
 *
 * @exception UnsupportedFeatureException
 * @exception OutOfMemoryException
 * @exception InitializationFailedException
 * @exception IllegalArgumentException
 */
void SSLContext::init(SSL_METHOD method)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	shutdown();
	mutex.lock();
	if (!method) method=SSLContext::TLS;
	while ((ERR_get_error()));	// Clear Error-Stack
	switch (method) {
		case SSLContext::TLS:
#ifdef HAVE_TLS_METHOD
			ctx=SSL_CTX_new(TLS_method());
#else
			ctx=SSL_CTX_new(SSLv23_method());
#endif
			break;
		case SSLContext::TLSclient:
#ifdef HAVE_TLS_CLIENT_METHOD
			ctx=SSL_CTX_new(TLS_client_method());
#else
			ctx=SSL_CTX_new(SSLv23_client_method());
#endif
			break;
		case SSLContext::TLSserver:
#ifdef HAVE_TLS_SERVER_METHOD
			ctx=SSL_CTX_new(TLS_server_method());
#else
			ctx=SSL_CTX_new(SSLv23_server_method());
#endif
			break;
		default:
			mutex.unlock();
			throw IllegalArgumentException("SSLContext::Init(int method=%i)",method);
			break;
	};
	if (!ctx) {
		mutex.unlock();
		throw InitializationFailedException("SSL_CTX_new");
	}
	mutex.unlock();
#endif
}

bool SSLContext::isInit()
{
	if (ctx) return true;
	return false;
}

/*!\brief SSL-Kontext deinitialisieren
 *
 * \desc
 *
 * @exception SSLContextInUseException
 */
void SSLContext::shutdown()
{
	#ifdef HAVE_OPENSSL
		if (!SSLisInitialized) return;
		mutex.lock();
		if (references) {
			mutex.unlock();
			throw SSLContextInUseException("%i",references);
		}
    	if (ctx) {
			SSL_CTX_free((SSL_CTX*)ctx);
			ctx=NULL;
    	}
    	while ((ERR_get_error()));	// Clear Error-Stack
    	mutex.unlock();
	#endif
}

/*!\brief create a new SSL structure for a connection
 *
 * \desc
 * creates a new SSL structure which is needed to hold the data for a TLS/SSL connection.
 * The new structure inherits the settings of the underlying SSLContext:
 * - connection method (SSLv2/v3/TLSv1)
 * - options
 * - verification settings
 * - timeout settings.
 * \return The function allocates memory to hold a SSL-Structure. It must be freed by
 * using SSLContext::releaseSSL
 *
 * @exception UnsupportedFeatureException
 * @exception SSLContextUninitializedException
 * @exception SSLException
 */
void *SSLContext::newSSL()
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	mutex.lock();
	if (!ctx) {
		mutex.unlock();
		throw SSLContextUninitializedException();
	}
	while ((ERR_get_error()));	// Clear Error-Stack
	void *ssl=SSL_new((SSL_CTX*)ctx);
	if (!ssl) {
		mutex.unlock();
		throw SSLException("SSL_new failed");
	}
	references++;
	mutex.unlock();
	return ssl;
#endif
}

/*!\brief free an allocated SSL structure
 *
 * \desc
 * Decrements the reference count of \p ssl, and removes the SSL
 * structure pointed to by \p ssl and frees up the allocated memory if the
 * reference count has reached 0.
 * @param ssl Pointer to SSL structure received by SSLContext::newSSL
 *
 * @exception UnsupportedFeatureException is thrown, when OpenSSL is not supported
 * @exception NullPointerException is thrown when \p ssl points to NULL
 * @exception SSLContextReferenceCounterMismatchException is thrown, when the internal reference
 * counter is already 0 when calling this function
 */
void SSLContext::releaseSSL(void *ssl)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	if (!ssl) throw NullPointerException();
	mutex.lock();
	SSL_free((SSL*)ssl);
	if (references<=0) throw SSLContextReferenceCounterMismatchException();
	references--;
	mutex.unlock();
#endif
}

void SSLContext::loadTrustedCAfromFile(const String &filename)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	mutex.lock();
	if (!ctx) {
		mutex.unlock();
		throw SSLContextUninitializedException();
	}
	while ((ERR_get_error()));	// Clear Error-Stack
	if (SSL_CTX_load_verify_locations((SSL_CTX*)ctx,filename,NULL)!=1) {
		mutex.unlock();
		throw InvalidSSLCertificateException(filename);
	}
	mutex.unlock();
#endif
}

void SSLContext::loadTrustedCAfromPath(const String &path)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else

	mutex.lock();
	if (!ctx) {
		mutex.unlock();
		throw SSLContextUninitializedException();
	}
	while ((ERR_get_error()));	// Clear Error-Stack
	if (SSL_CTX_load_verify_locations((SSL_CTX*)ctx,NULL,path)!=1) {
		mutex.unlock();
		throw InvalidSSLCertificateException(path);
	}
	mutex.unlock();
#endif
}


#ifdef HAVE_OPENSSL
static int pem_passwd_cb(char *buf, int size, int rwflag, void *password)
{
	strncpy(buf, (char *)(password), size);
	buf[size - 1] = 0;
	return(strlen(buf));
}
#endif

/*!\brief
 *
 * \desc
 * LoadCertificate wird benoetigt, wenn ein SSL-Server gestartet werden soll.
 * LoadCertificate laed ein Zertifikat im PEM-Format oder eine komplette Trustchain im
 * PEM-Format aus dem File "certificate". Wird "privatekey" angegeben, wird daraus der
 * Private Key geladen. Wenn nicht, wird der Private Key ebenfalls in der "certificate"-
 * Datei erwartet. Ist der Key durch ein Passwort geschuetzt, muss dieses als "password"
 * angegeben werden.
 *
 */
void SSLContext::loadCertificate(const String &certificate, const String &privatekey, const String &password)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	mutex.lock();
	if (!ctx) {
		mutex.unlock();
		throw SSLContextUninitializedException();
	}
	while ((ERR_get_error()));	// Clear Error-Stack

	//if (!SSL_CTX_use_certificate_file((SSL_CTX*)ctx,keyfile,SSL_FILETYPE_PEM)) {
	if (!SSL_CTX_use_certificate_chain_file((SSL_CTX*)ctx,certificate)) {
		mutex.unlock();
		throw SSLException("SSL_CTX_use_certificate_chain_file");
	}
	if (password.notEmpty()) {
		SSL_CTX_set_default_passwd_cb((SSL_CTX*)ctx,pem_passwd_cb);
		SSL_CTX_set_default_passwd_cb_userdata((SSL_CTX*)ctx,(void*)password.getPtr());
	}
	String key=privatekey;
	if (key.isEmpty()) key=certificate;
	if (!SSL_CTX_use_PrivateKey_file((SSL_CTX*)ctx,key,SSL_FILETYPE_PEM)) {
		mutex.unlock();
		throw SSLPrivatKeyException(key);
	}
	mutex.unlock();
#endif
}


void SSLContext::setCipherList(const String &cipherlist)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	mutex.lock();
	if (!ctx) {
		mutex.unlock();
		throw SSLContextUninitializedException();
	}
	while ((ERR_get_error()));	// Clear Error-Stack
	if (SSL_CTX_set_cipher_list((SSL_CTX*)ctx, cipherlist)!=1) {
		mutex.unlock();
		throw InvalidSSLCipherException(cipherlist);
	}
	mutex.unlock();
#endif
}

void SSLContext::setTmpDHParam(const String &dh_param_file)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	mutex.lock();
	if (!ctx) {
		mutex.unlock();
		throw SSLContextUninitializedException();
	}
	FILE *ff=NULL;
#ifdef WIN32
	WideString wideFilename=dh_param_file;
	WideString wideMode=L"r";
	if ((ff=(FILE*)_wfopen((const wchar_t *)wideFilename,(const wchar_t*)wideMode))==NULL) {

#else
	if ((ff=(FILE*)fopen((const char*)dh_param_file,"r"))==NULL) {
#endif
		int e=errno;
		mutex.unlock();
		throwExceptionFromErrno(e,dh_param_file);
	}
	DH *dh=PEM_read_DHparams(ff, NULL, NULL, NULL);
	if (!dh) {
		mutex.unlock();
		throw SSLFailedToReadDHParams(dh_param_file);
	}
	if (!SSL_CTX_set_tmp_dh((SSL_CTX*)ctx,dh)) {
		mutex.unlock();
		throw SSLFailedToReadDHParams(dh_param_file);
	}
	mutex.unlock();
#endif
}



/** @name SSL-Verschlüsselung
 *  Die nachfolgenden Befehle werden benötigt, wenn die Kommunikation zwischen Client
 *  und Server mit SSL verschlüsselt werden soll. Voraussetzung dafür ist, dass die PPL-Library
 *  mit OpenSSL-Unterstützung kompiliert wurde.
 *
 */
//@{


/*!\brief SSL-Kommunikation starten
 *
 * \desc
 * Durch Aufruf dieser Funktion wird die SSL-Kommunikation über eine bereits bestehende TCP-Verbindung
 * zu einem Server gestartet. Um wieder unverschlüsselt kommunizieren zu können, muss CTCPSocket::sslStop
 * aufgerufen werden.
 * @exception Diverse Falls die SSL-Kommunikation nicht gestartet werden kann, wird eine Exception
 * geworfen
 */
void TCPSocket::sslStart(SSLContext &context)
{
#ifndef HAVE_OPENSSL
	throw UnsupportedFeatureException("OpenSSL");
#else
	if (ssl) sslStop();
	ssl=context.newSSL();
	sslcontext=&context;
	if (!isConnected()) throw NotConnectedException();
	PPLSOCKET* s=(PPLSOCKET*)socket;
	SSL_set_fd((SSL*)ssl,s->sd);
	//SSL_set_mode((SSL*)ssl, SSL_MODE_AUTO_RETRY);
	SSL_set_connect_state((SSL*)ssl);

	if (connect_timeout_sec>0 || connect_timeout_usec>0) {
		bool blocking_before=isBlocking();
		setBlocking(false);
		struct timeval tval;
		fd_set rset, wset;
		int n;
		int sockfd=((PPLSOCKET*)socket)->sd;
		tval.tv_sec=connect_timeout_sec;
		tval.tv_usec=connect_timeout_usec;
		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		wset=rset;
		int res;
		while ((res=SSL_connect((SSL*)ssl))<1) {
			/*
			if (thread!=NULL && thread->ThreadShouldStop()) {
				SSL_shutdown((SSL*)ssl);
				SSL_free((SSL*)ssl);
				ssl=NULL;
				SetError(336);
				return 0;
			}
			*/
			if (res==0) {
				ppl7::String Error;
				Error.setf("SSL_connect: %s, State: %s",ssl_geterror((SSL*)ssl,res), SSL_state_string_long((SSL*)ssl));
				sslStop();
				throw SSLConnectionFailedException(Error);
			}
			int e=SSL_get_error((SSL*)ssl,res);
			//printf ("res=%i, e=%i, state=%x: %s\n",res,e,SSL_state((SSL*)ssl), SSL_state_string_long((SSL*)ssl));
			if (e==SSL_ERROR_WANT_READ) {
				if ((n=select(sockfd+1,&rset,NULL,NULL,&tval))==0) {
					ppl7::String Error;
					Error.setf("Socket not ready for reading. SSL_connect: %s, State: %s",ssl_geterror((SSL*)ssl,res), SSL_state_string_long((SSL*)ssl));
					sslStop();
					throw SSLConnectionFailedException(Error);
				}
			} else if (e==SSL_ERROR_WANT_WRITE) {
				if ((n=select(sockfd+1,NULL,&wset,NULL,&tval))==0) {
					ppl7::String Error;
					Error.setf("Socket not ready for writing. SSL_connect: %s, State: %s",ssl_geterror((SSL*)ssl,res), SSL_state_string_long((SSL*)ssl));
					sslStop();
					throw SSLConnectionFailedException(Error);
				}
			} else {
				ppl7::String Error;
				Error.setf("SSL_connect: %s, State: %s",ssl_geterror((SSL*)ssl,res), SSL_state_string_long((SSL*)ssl));
				sslStop();
				throw SSLConnectionFailedException(Error);
			}
		}
		setBlocking(blocking_before);
	} else {
		int res=SSL_connect((SSL*)ssl);
		if (res<1) {
			ppl7::String Error;
			Error.setf("SSL_connect: %s, State: %s",ssl_geterror((SSL*)ssl,res), SSL_state_string_long((SSL*)ssl));
			sslStop();
			throw SSLConnectionFailedException(Error);
		}
	}
#endif
}

/*!\brief SSL-Kommunikation stoppen
 *
 * \desc
 * Durch Aufruf dieser Funktion wird die SSL-Kommunikation zu einem Server wieder gestoppt. Sofern
 * die Verbindung dadurch nicht geschlossen wird, kann mit der Gegenstelle wieder unverschlüsselt
 * kommuniziert werden.
 *
 * @exception Keine Durch Aufruf dieser Funktion wird keine Exception geworfen.
 *
 */
void TCPSocket::sslStop()
{
#ifdef HAVE_OPENSSL
	if (ssl) {
		SSL_shutdown((SSL*)ssl);
		if (sslcontext) sslcontext->releaseSSL(ssl);
		else SSL_free((SSL*)ssl);
	}
	ssl=NULL;
	sslcontext=NULL;
#endif
}

/*!\brief Verschlüsselte Daten schreiben
 *
 * \desc
 * Diese private Funktion wird intern automatisch von den Write-Funktionen der Socketklasse
 * aufgerufen, wenn SSL-Verschlüsselung aktiviert wurde. Anstelle der Write-Funktionen der
 * Socket-Schnittstelle des Betriebssystems wird dann diese Funktion verwendet.
 * Sie schickt \p size Bytes aus dem Speicherbereich \p buffer verschlüsselt an die
 * Gegenstelle.
 *
 * @param[in] buffer Beginn des zu sendenden Speicherbereichs
 * @param[in] size Anzahl zu sendender Bytes.
 * @return Die Funktion gibt die Anzahl erfolgreich geschriebener Bytes zurück, was auch 0 sein kann.
 * @exception SSLException wird geworfen, wenn ein Fehler aufgetreten ist
 */
int TCPSocket::SSL_Write(const void *buffer, int size)
{
	#ifdef HAVE_OPENSSL
		int bytes=::SSL_write((SSL*)ssl,buffer,size);
		switch(::SSL_get_error((SSL*)ssl,bytes)) {
			case SSL_ERROR_NONE:
				return bytes;
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
            	return 0;
            default:
            	String sslerrorstack;
            	GetSSLErrors(sslerrorstack);
            	int e=SSL_get_error((SSL*)ssl,bytes);
            	throw SSLException("%s, %s [%s]",ssl_geterror((SSL*)ssl,bytes),
            			ERR_error_string(e,NULL),
						(const char*)sslerrorstack);
		}
		return bytes;
	#else
		throw UnsupportedFeatureException("OpenSSL");
	#endif
}

/*!\brief Verschlüsselte Daten lesen
 *
 * \desc
 * Diese private Funktion wird intern automatisch von den Read-Funktionen der Socketklasse
 * aufgerufen, wenn SSL-Verschlüsselung aktiviert wurde. Anstelle der Read-Funktionen der
 * Socket-Schnittstelle des Betriebssystems wird dann diese Funktion verwendet.
 * Sie liest \p size verschlüsselte Bytes von der Gegenstelle in den bereits allokierten
 * Speicherbereich \p buffer und entschlüsselt sie dabei.
 *
 * @param[in] buffer Bereits allokierter Speicherbereich, in den die gelesenen Daten
 * geschrieben werden sollen
 * @param[in] size Anzahl zu lesender Bytes.
 * @return Die Funktion gibt die Anzahl erfolgreich gelesener Bytes zurück, was auch 0 sein kann
 * @exception SSLException wird geworfen, wenn ein Fehler aufgetreten ist
 */
int TCPSocket::SSL_Read(void *buffer, int size)
{
	#ifdef HAVE_OPENSSL
		int bytes=::SSL_read((SSL*)ssl,buffer,size);
		switch(::SSL_get_error((SSL*)ssl,bytes)) {
			case SSL_ERROR_NONE:
				return bytes;
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
            	return 0;
            default:
            	String sslerrorstack;
            	GetSSLErrors(sslerrorstack);
            	int e=SSL_get_error((SSL*)ssl,bytes);
            	throw SSLException("%s, %s [%s]",ssl_geterror((SSL*)ssl,bytes),
            			ERR_error_string(e,NULL),
						(const char*)sslerrorstack);
		}
		return bytes;
	#else
		throw UnsupportedFeatureException("OpenSSL");
	#endif
}


/*!\brief Auf eine TLS/SSL-Handshake warten
 *
 * \desc
 * SSL_Accept wartet darauf, dass der mit dem Socket verbundene Client eine TLS/SSL Verbindung
 * startet. Ist der Socket auf "blocking" eingestellt, wartet die Funktion solange, bis
 * ein Handshake erfolgt, die Verbindung getrennt wird oder ein Timeout auftritt.
 * Die verwandte Funktion CTCPSocket::SSL_WaitForAccept wartet ebenfalls auf ein TLS/SSL Handshake,
 * jedoch kann hier zusätzlich ein Timeout angegeben werden und die Funktion beendet sich, wenn
 * der Thread beendet werden soll.
 *
 * @return Bei erfolgreichem Handshake liefert die Funktion 1 zurück, im Fehlerfall 0.
 */
void TCPSocket::sslAccept(SSLContext &context)
{
#ifdef HAVE_OPENSSL
	if (ssl) sslStop();
	ssl=context.newSSL();
	sslcontext=&context;
	if (!isConnected()) throw NotConnectedException();
	PPLSOCKET* s=(PPLSOCKET*)socket;
	if (1 != SSL_set_fd((SSL*)ssl,s->sd)) {
		String sslerrorstack;
		GetSSLErrors(sslerrorstack);
		context.releaseSSL(ssl);
		ssl=NULL;
		throw SSLException("SSL_set_fd failed [%s]", (const char*)sslerrorstack);
	}
	SSL_set_accept_state((SSL*)ssl);
	int res=SSL_accept((SSL*)ssl);
	if (res<1) {
		int e=SSL_get_error((SSL*)ssl,res);
		if (e==SSL_ERROR_WANT_READ || e==SSL_ERROR_WANT_WRITE) {
			// Non-Blocking
			context.releaseSSL(ssl);
			ssl=NULL;
			throw OperationBlockedException();
		} else {
			printf("e=%d\n",e);
			String sslerrorstack;
			GetSSLErrors(sslerrorstack);
			const char *errortext=ssl_geterror((SSL*)ssl,res);
			context.releaseSSL(ssl);
			ssl=NULL;
			throw SSLException("%s, %s [%s]",errortext,
					ERR_error_string(e,NULL),
					(const char*)sslerrorstack);
		}
	}
#else
	throw UnsupportedFeatureException("OpenSSL");
#endif
}

/*!\brief Auf eine TLS/SSL-Handshake warten

 * \desc
 * SSL_WaitForAccept wartet darauf, dass der mit dem Socket verbundene Client eine TLS/SSL Verbindung
 * startet. Vor Aufruf sollte der Socket auf "non-Blocking" gestellt werden (siehe TCPSocket::setBlocking).
 * Die Funktion wartet solange, bis entweder ein Handshake zustande kommt oder der angegebene Timeout
 * erreicht wurde, oder der Überwachungsthread (siehe CTCPSocket::WatchThread)
 * beendet werden soll.
 *
 * @param timeout_ms Ein Timeout in Millisekunden. Bei Angabe von 0, wartet die Funktion unbegenzt lange.
 */
void TCPSocket::sslWaitForAccept(SSLContext &context, int timeout_ms)
{
	try {
	ppluint64 tt=GetMilliSeconds()+timeout_ms;
	while (timeout_ms==0 || GetMilliSeconds()<=tt) {
		if (stoplisten) {

			printf ("stop\n");
			throw ppl7::OperationAbortedException("TCPSocket::sslWaitForAccept");
		}
		try {
			sslAccept(context);
			return;
		} catch (const ppl7::OperationBlockedException &exp) {
			printf ("Blocked\n");
			MSleep(10);
		}
	}
	throw ppl7::TimeoutException("Timeout while waiting for SSL handshake [TCPSocket::sslWaitForAccept]");
	} catch (const ppl7::Exception &exp) {
		exp.print();
				throw;
	}
}



/*!\brief SSL-Zertifikat der Gegenstelle prüfen
 *
 * \desc
 * Mit dieser Funktion kann nach Herstellung einer SSL-Verbindung das Zertifikat der Gegenstelle
 * geprüft werden. Dabei werden insbesondere auch die Signierungsketten bis zum Root-Zertifikat überprüft
 * und dabei nicht nur die gängigen Root-Zertifikate, die in der OpenSSL-Library enthalten sind, berücksichtigt,
 * sondern auch die, die zuvor manuell mit SSLContext::LoadTrustedCAfromFile oder SSLContext::LoadTrustedCAfromPath
 * geladen wurden.
 *
 * @param[in] hostname Der erwartete Name des Zertifikats. Wird NULL übergeben, wird der Name des Zertifikats
 * nicht überprüft
 * @param[in] AcceptSelfSignedCert Wird hier "true" angegeben, werden auch selbst-signierte Zertifikate
 * akzeptiert.
 * @return Ist das Zertifikat gültig und alle Prüfungen erfolgreich, gibt die Funktion 1 zurück,
 * ansonsten 0.
 */
void TCPSocket::sslCheckCertificate(const ppl7::String &name, bool AcceptSelfSignedCert)
{
#ifdef HAVE_OPENSSL
	if (!ssl) throw SSLNotStartedException();
	if (name.notEmpty()) {
		// Den Namen Überprüfen
		X509 *peer=SSL_get_peer_certificate((SSL*)ssl);
		char peer_CN[256];
		X509_NAME_get_text_by_NID(X509_get_subject_name(peer),NID_commonName,peer_CN,256);
		if (strcasecmp(peer_CN,(const char*)name)!=0) {
			X509_free(peer);
			throw InvalidSSLCertificateException("Name mismatch: %s != %s",(const char*)name,peer_CN);
		}
		X509_free(peer);
	}

	// Zertifikat Überprüfen
	int ret=SSL_get_verify_result((SSL*)ssl);
	//if (ret!=X509_V_OK && ret!=X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT) {
	if (ret!=X509_V_OK) {
		const char *a="unknown";
		switch (ret) {
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
			a="X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT";
			break;
		case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE:
			a="X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE";
			break;
		case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY:
			a="X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY";
			break;
		case X509_V_ERR_CERT_SIGNATURE_FAILURE:
			a="X509_V_ERR_CERT_SIGNATURE_FAILURE";
			break;
		case X509_V_ERR_CERT_NOT_YET_VALID:
			a="X509_V_ERR_CERT_NOT_YET_VALID";
			break;
		case X509_V_ERR_CERT_HAS_EXPIRED:
			a="X509_V_ERR_CERT_HAS_EXPIRED";
			break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
			a="X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD";
			break;
		case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
			a="X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD";
			break;
		case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT:
			if (AcceptSelfSignedCert) return;
			a="X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT";
			break;
		case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN:
			a="X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN";
			break;
		case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
			a="X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY";
			break;
		case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE:
			a="X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE";
			break;
		case X509_V_ERR_INVALID_CA:
			a="X509_V_ERR_INVALID_CA";
			break;
		case X509_V_ERR_PATH_LENGTH_EXCEEDED:
			a="X509_V_ERR_PATH_LENGTH_EXCEEDED";
			break;
		case X509_V_ERR_INVALID_PURPOSE:
			a="X509_V_ERR_INVALID_PURPOSE";
			break;
		case X509_V_ERR_CERT_UNTRUSTED:
			a="X509_V_ERR_CERT_UNTRUSTED";
			break;
		case X509_V_ERR_CERT_REJECTED:
			a="X509_V_ERR_CERT_REJECTED";
			break;
		case X509_V_ERR_SUBJECT_ISSUER_MISMATCH:
			a="X509_V_ERR_SUBJECT_ISSUER_MISMATCH";
			break;
		case X509_V_ERR_AKID_SKID_MISMATCH:
			a="X509_V_ERR_AKID_SKID_MISMATCH";
			break;
		case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH:
			a="X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH";
			break;
		case X509_V_ERR_KEYUSAGE_NO_CERTSIGN:
			a="X509_V_ERR_KEYUSAGE_NO_CERTSIGN";
			break;
		case X509_V_ERR_CERT_REVOKED:
			a="X509_V_ERR_CERT_REVOKED";
			break;
		}
		throw InvalidSSLCertificateException("SSL-Error: %s", a);
	}
#else
	throw UnsupportedFeatureException("OpenSSL");
#endif
}



bool TCPSocket::sslIsEncrypted() const
{
#ifdef HAVE_OPENSSL
	if (ssl) return true;
#endif
	return false;
}

String TCPSocket::sslGetCipherName() const
{
#ifdef HAVE_OPENSSL
	if (ssl) return SSL_get_cipher((SSL*)ssl);
#endif
	return String();
}

String TCPSocket::sslGetCipherVersion() const
{
#ifdef HAVE_OPENSSL
	if (ssl) return SSL_get_cipher_version((SSL*)ssl);
#endif
	return String();
}

int TCPSocket::sslGetCipherBits() const
{
#ifdef HAVE_OPENSSL
	int np=0;
	if (ssl) return SSL_get_cipher_bits((SSL*)ssl, &np);
#endif
	return 0;
}



//@}

} // EOF namespace ppl



