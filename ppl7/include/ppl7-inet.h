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

#ifndef PPL7INET_H_
#define PPL7INET_H_

#ifndef _PPL7_INCLUDE
    #ifdef PPL7LIB
        #include "ppl7.h"
    #else
        #include <ppl7.h>
    #endif
#endif

#include <list>

namespace ppl7 {

PPL7EXCEPTION(NetworkException,Exception);
PPL7EXCEPTION(WinsockInitialisationFailed, Exception);
PPL7EXCEPTION(IdnConversionException, Exception);

PPL7EXCEPTION(NotConnectedException,NetworkException);
PPL7EXCEPTION(CouldNotOpenSocketException,NetworkException);				// 393
PPL7EXCEPTION(IllegalPortException,NetworkException);				// 393
PPL7EXCEPTION(CouldNotBindToInterfaceException,NetworkException);	// 394
PPL7EXCEPTION(ConnectionRefusedException,NetworkException);
PPL7EXCEPTION(NetworkDownException,NetworkException);
PPL7EXCEPTION(NetworkUnreachableException,NetworkException);
PPL7EXCEPTION(NetworkDroppedConnectionOnResetException,NetworkException);
PPL7EXCEPTION(SoftwareCausedConnectionAbortException,NetworkException);
PPL7EXCEPTION(ConnectionResetByPeerException,NetworkException);
PPL7EXCEPTION(NoBufferSpaceException,NetworkException);
PPL7EXCEPTION(SocketIsAlreadyConnectedException,NetworkException);
PPL7EXCEPTION(CantSendAfterSocketShutdownException,NetworkException);
PPL7EXCEPTION(TooManyReferencesException,NetworkException);
PPL7EXCEPTION(HostDownException,NetworkException);
PPL7EXCEPTION(NoRouteToHostException,NetworkException);
PPL7EXCEPTION(InvalidSocketException,NetworkException);
PPL7EXCEPTION(UnknownOptionException,NetworkException);
PPL7EXCEPTION(OutOfBandDataReceivedException,NetworkException);
PPL7EXCEPTION(BrokenPipeException,NetworkException);
PPL7EXCEPTION(SSLNotStartedException,NetworkException);
PPL7EXCEPTION(SSLConnectionFailedException,NetworkException);
PPL7EXCEPTION(SettingSocketOptionException,NetworkException);
PPL7EXCEPTION(InvalidIpAddressException,NetworkException);
PPL7EXCEPTION(InvalidNetworkAddressException,NetworkException);
PPL7EXCEPTION(InvalidNetmaskOrPrefixlenException,NetworkException);

PPL7EXCEPTION(ResolverException,Exception);
PPL7EXCEPTION(UnknownHostException,Exception);




enum ResolverFlags {
	af_unspec=0,
	af_inet=1,
	af_inet6=2,
	af_all=3
};


class IPAddress
{
	public:
		enum IP_FAMILY {
			UNKNOWN=0,
			IPv4=4,
			IPv6=6
		};
	private:
		unsigned char _addr[16];
		IP_FAMILY _family;
	public:
		IPAddress();
		IPAddress(const IPAddress &other);
		IPAddress(const String &other);
		IPAddress(IP_FAMILY family, void *addr, size_t addr_len);
		IPAddress &operator=(const IPAddress &other);
		IPAddress &operator=(const String &other);
		void set(const IPAddress &other);
		void set(const String &address);
		void set(IP_FAMILY family, void *addr, size_t addr_len);
		//void setFromSockAddr(const void *sockaddr, size_t sockaddr_len);
		IPAddress::IP_FAMILY family() const;
		const void *addr() const;
		size_t addr_len() const;
		String toString() const;
		operator String() const;

		IPAddress mask(int prefixlen) const;

		void toSockAddr(void *sockaddr, size_t sockaddr_len) const;
		int compare(const IPAddress &other) const;
		bool operator<(const IPAddress &other) const;
		bool operator<=(const IPAddress &other) const;
		bool operator==(const IPAddress &other) const;
		bool operator!=(const IPAddress &other) const;
		bool operator>=(const IPAddress &other) const;
		bool operator>(const IPAddress &other) const;
};

std::ostream& operator<<(std::ostream& s, const IPAddress &addr);

class IPNetwork
{
	public:
	private:
		IPAddress	_addr;
		int			_prefixlen;
	public:
		IPNetwork();
		IPNetwork(const IPNetwork &other);
		IPNetwork(const String &other);
		IPNetwork &operator=(const IPNetwork &other);
		IPNetwork &operator=(const String &other);
		void set(const IPAddress &other, int prefixlen);
		void set(const IPNetwork &other);
		void set(const String &network);
		IPAddress::IP_FAMILY family() const;
		IPAddress addr() const;
		IPAddress first() const;
		IPAddress last() const;
		IPAddress netmask() const;
		int prefixlen() const;
		String toString() const;
		operator String() const;
		bool contains(const IPAddress &addr) const;


		int compare(const IPNetwork &other) const;
		bool operator<(const IPNetwork &other) const;
		bool operator<=(const IPNetwork &other) const;
		bool operator==(const IPNetwork &other) const;
		bool operator!=(const IPNetwork &other) const;
		bool operator>=(const IPNetwork &other) const;
		bool operator>(const IPNetwork &other) const;

		static int getPrefixlenFromNetmask(const String &netmask);

};

std::ostream& operator<<(std::ostream& s, const IPNetwork &net);

class SockAddr
{
	private:
		void		*saddr;
		size_t		addrlen;
	public:
		SockAddr();
		SockAddr(const SockAddr &other);
		SockAddr(const void *addr, size_t addrlen);
		SockAddr(const IPAddress &addr, int port);
		~SockAddr();
		SockAddr &operator=(const SockAddr &other);
		void setAddr(const void *addr, size_t addrlen);
		void setAddr(const IPAddress &ip, int port);
		void setAddr(const IPAddress &ip);
		void setPort(int port);
		void *addr() const;
		size_t size() const;
		int port() const;
		IPAddress toIPAddress() const;
};


void InitSockets();
String GetHostname();
size_t GetHostByName(const String &name, std::list<IPAddress> &result,ResolverFlags flags=af_unspec);
size_t GetHostByAddr(const IPAddress &addr, String &hostname);
ppluint32 Ntohl(ppluint32 net);
ppluint32 Htonl(ppluint32 host);
ppluint16 Ntohs(ppluint16 net);
ppluint16 Htons(ppluint16 host);
bool IsBigEndian();
bool IsLittleEndian();


class Resolver
{
        private:

        public:
                // Quelle: http://en.wikipedia.org/wiki/List_of_DNS_record_types
                enum Type {
                        A=1,
                        NS=2,
                        CNAME=5,
                        MX=15,
                        SOA=6,
                        PTR=12,
                        TXT=16,
                        AAAA=28,
                        NAPTR=35,
                        SRV=33,
                        DS=43,
                        DNSKEY=48,
                        NSEC=47,
                        NSEC3=50,
                        RRSIG=46,
                        OPT=41,
                        TSIG=250
                };

                enum Class {
                        /** the Internet */
                        CLASS_IN    = 1,
                        /** Chaos class */
                        CLASS_CH    = 3,
                        /** Hesiod (Dyer 87) */
                        CLASS_HS    = 4,
                        /** None class, dynamic update */
                        CLASS_NONE      = 254,
                        /** Any class */
                        CLASS_ANY   = 255,
                        CLASS_FIRST     = 0,
                        CLASS_LAST      = 65535,
                        CLASS_COUNT     = CLASS_LAST - CLASS_FIRST + 1
                };

                static String typeName(Type t);
                static String className(Class c);
                static void query(Array &r, const String &label, Type t=A, Class c=CLASS_IN);
};




String Idn2Ace(const String &idn);
WideString Idn2Ace(const WideString &idn);
String Ace2Idn(const String &ace);
WideString Ace2Idn(const WideString &ace);

String ToQuotedPrintable (const String &source);


class TCPSocket;

void SSL_Init();
void SSL_Exit();

//! \brief SSL-Verschlüsselung
class SSLContext
{
	friend class CTCPSocket;
	private:
		Mutex		mutex;
		void		*ctx;
		int			references;
		void 		clear();
	public:
		enum SSL_METHOD {
			TLS,				// Verwendet SSLv23, SSL wird aber disabled, so dass nur TLS unterstuetzt wird
			TLSclient,
			TLSserver,
		};

		SSLContext();
		SSLContext(SSL_METHOD method);
		~SSLContext();
		void 	init(SSL_METHOD method=TLS);
		bool	isInit();
		void 	shutdown();
		void 	*newSSL();
		void	releaseSSL(void *ssl);
		void	loadTrustedCAfromFile(const String &filename);
		void	loadTrustedCAfromPath(const String &path);
		void	loadCertificate(const String &certificate, const String &privatekey=String(), const String &password=String());
		void	setCipherList(const String &cipherlist);		// "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"
		void	setTmpDHParam(const String &dh_param_file);
};

class SSLError
{
	public:
		SSLError() {
			Code=0;
			Line=Flags=0;
		}
		String			Text;
		String			Filename;
		String			Data;
		unsigned long	Code;
		int				Line;
		int				Flags;
};

int GetSSLError(SSLError &e);
int GetSSLErrors(std::list<SSLError> &e);
int GetSSLErrors(String &e);
void ClearSSLErrorStack();


//! \brief TCP-Socket-Nachrichten
class SocketMessage
{
	friend class TCPSocket;
	private:
		void		*payload;
		size_t		payload_size;
		int			payload_type;
		int			commandId;
		int			Id;
		int			Version;
		bool		UseCompression;
		bool		SupportMsgChannel;

		void compilePacketHeader(char *buffer, size_t *buffer_size, const void *payload, size_t payload_size, bool is_compressed) const;
		void readFromPacketHeader(const char *msgbuffer, int &flags);

	public:
		PPL7EXCEPTION(NoDataAvailableException, Exception);
		PPL7EXCEPTION(DataInOtherFormatException, Exception);
		PPL7EXCEPTION(InvalidProtocolVersion, Exception);
		PPL7EXCEPTION(InvalidPacketException, Exception);
		PPL7EXCEPTION(PayloadTooBigException, Exception);


		bool		ClientSupportsCompression;

		SocketMessage();
		SocketMessage(const SocketMessage &other);

		virtual ~SocketMessage();
		void copy(const SocketMessage &other);

		void clear();


		void setCommandId(int id);
		int getCommandId() const;
		void setId(int id);
		int getId() const;
		void setPayload(const String &msg);
		void setPayload(const AssocArray &msg);
		void setPayload(const ByteArrayPtr &msg);
		void getPayload(String &msg) const;
		void getPayload(AssocArray &msg) const;
		void getPayload(ByteArray &msg) const;
		int getPayloadType();
		void enableCompression(bool flag=true);
		void enableMsgChannel(bool flag=true);
		bool isCompressionSupported() const;
		bool isMsgChannelSupported() const;
};


//! \brief TCP-Socket-Klasse
class TCPSocket
{
	private:
		Mutex	mutex;
		SSLContext		*sslcontext;
		void *socket;
		void *ssl;
		bool	connected;
		bool	islisten;
		bool	blocking;
		//int		BytesWritten;
		//int		BytesRead;
		int		connect_timeout_sec;
		int		connect_timeout_usec;
        bool    stoplisten;
        String	HostName;
        int			PortNum;
        String		SourceInterface;
        int			SourcePort;

        int		SSL_Write(const void *buffer, int size);
        int		SSL_Read(void *buffer, int bytes);

	public:
		TCPSocket();
		virtual ~TCPSocket();
		//! @name TCP-Client functions
		//@{
		void setSource(const String &interface_name, int port=0);
		void connect(const String &host_and_port);
		void connect(const String &host, int port);
		void setTimeoutConnect(int seconds, int useconds);
		bool isConnected() const;
		int	port() const;
		const String& hostname() const;
		//@}

		//! @name TCP-Server functions
		//@{
		void bind(const String &host, int port);
		virtual int receiveConnect(TCPSocket *socket, const String &host, int port);
		bool isListening() const;
        void stopListen();
		void signalStopListen();
        void listen(int backlog=64, int timeout=100);

		//@}

		//! @name Common functions for client and server
		//@{
		void setTimeoutRead(int seconds, int useconds);
		void setTimeoutWrite(int seconds, int useconds);

        void disconnect();
        void shutdown();
		size_t write(const String &str, size_t bytes=0);
		size_t write(const WideString &str, size_t bytes=0);
		size_t write(const ByteArrayPtr &bin, size_t bytes=0);
		size_t write(const void *buffer, size_t bytes);
		size_t writef(const char *fmt, ...);
		size_t write(const SocketMessage &msg);
		size_t read(void *buffer, size_t bytes);
		size_t read(String &buffer, size_t bytes);
		size_t read(ByteArray &buffer, size_t bytes);

		void readLoop(void *buffer, size_t bytes, int timeout_seconds=0, Thread *watch_thread=NULL);

		int getDescriptor();
		void setBlocking(bool value);
		bool isBlocking() const;
		bool isWriteable();
		bool isReadable();
		bool waitForIncomingData(int seconds, int useconds);
		bool waitForOutgoingData(int seconds, int useconds);
		SockAddr getSockAddr() const;
		SockAddr getPeerAddr() const;
		//@}

		//! @name SSL Encryption
		//@{
		void	sslStart(SSLContext &context);
		void	sslStop();
		void	sslCheckCertificate(const ppl7::String &name, bool AcceptSelfSignedCert=false);
		void	sslAccept(SSLContext &context);
		void	sslWaitForAccept(SSLContext &context, int timeout_ms=0);
		bool	sslIsEncrypted() const;
		String	sslGetCipherName() const;
		String	sslGetCipherVersion() const;
		int		sslGetCipherBits() const;

		//@}


		//! @name TODO
		//@{
        bool waitForMessage(SocketMessage &msg, int timeout_seconds=0, Thread *watch_thread=NULL);
		//@}

};

class UDPSocket
{
	private:
		void	*socket;
		int		timeout_sec;
		int		timeout_usec;
        String	SourceInterface;
        int		SourcePort;
		int		connect_timeout_sec;
		int		connect_timeout_usec;
		bool	connected;

	public:
		UDPSocket();
		virtual ~UDPSocket();
		void setTimeoutRead(int seconds, int useconds);
		void setTimeoutWrite(int seconds, int useconds);

		//! @name Server functions
		//@{
		void bind(const String &host, int port);
		virtual int receiveConnect(UDPSocket *socket, const String &host, int port);
		bool isListening() const;
        void stopListen();
		void signalStopListen();
        void listen(int timeout=100);
        //@}

        //! @name Misc
		int getDescriptor();
		void setBlocking(bool value);
		bool isWriteable();
		bool isReadable();
		bool waitForIncomingData(int seconds, int useconds);
		bool waitForOutgoingData(int seconds, int useconds);
		SockAddr getSockAddr() const;
		SockAddr getPeerAddr() const;
		//@}

		//! @name Client functions
		size_t sendTo(const String &host, int port, const void *buffer, size_t bytes);
		size_t sendTo(const String &host, int port, const String &buffer);
		void setTimeoutConnect(int seconds, int useconds);
		void setSource(const String &interface_name, int port=0);
		void connect(const String &host_and_port);
		void connect(const String &host, int port);
		bool isConnected() const;
		void disconnect();
		size_t write(const String &str, size_t bytes=0);
		size_t write(const WideString &str, size_t bytes=0);
		size_t write(const ByteArrayPtr &bin, size_t bytes=0);
		size_t write(const void *buffer, size_t bytes);
		size_t writef(const char *fmt, ...);
		//@}


		int RecvFrom(void *buffer, int maxlen);
		int RecvFrom(void *buffer, int maxlen, String &host, int *port);
		int RecvFrom(String &buffer, int maxlen);
		int SetReadTimeout(int seconds, int useconds);
		int Bind(const char *host, int port);
		int GetDescriptor();

};

class Webserver
{
	private:
		SSLContext	SSL;
		void		*daemon;
		int			port;
		AssocArray	res;
		TCPSocket	Socket;
		bool		basicAuthentication;
		bool		SSLEnabled;
		String		realm;
		String		sslkey, sslcert;

	public:
		class Request {
			friend class Webserver;
			private:

			public:
				void *connection;
				void *postprocessor;
				String url;
				String method;
				String version;
				AssocArray header;
				AssocArray data;
				AssocArray auth;
		};
		Webserver();
		virtual ~Webserver();
		void bind(const String &adr, int port);
		void loadCertificate(const String &certificate, const String &privatekey, const String &password);
		void enableSSL(bool enable);
		bool sslEnabled() const;
		void start();
		void stop();
		void requireBasicAuthentication(bool enable, const String &realm);
		bool useBasicAuthentication() const;
		int queueResponse(const Request &req, const String &text, int httpStatus=200);
		int queueBasicAuthFailedResponse(const Request &req);

		virtual int request(Request &req);
		virtual int authenticate(const String &username, const String &password, Request &req);
		virtual String getDenyMessage();




		PPL7EXCEPTION(CouldNotBindToSocket, Exception);
		PPL7EXCEPTION(InvalidSSLCertificate, Exception);
		PPL7EXCEPTION(NoAddressSpecified, Exception);
		PPL7EXCEPTION(CouldNotStartDaemon, Exception);
		PPL7EXCEPTION(SSLInitializationFailed, Exception);

};

//! \brief CURL-Klasse
class Curl
{
	private:
		void *handle;
		void *httppost, *last_httppost;
		void *headers;
		char *resultbuffer;
		Logger *log;
		size_t resultbuffer_size;
		String Header;
		String Browser, Url, UserPassword, Referer, Proxy;
		String GetCall;
		char *errorbuffer;
		bool	aboard;
		void curlResultOk(int ret) const;



	public:

		PPL7EXCEPTION(InitializationFailedException, Exception);
		PPL7EXCEPTION(MiscException, Exception);
		PPL7EXCEPTION(StringEscapeException, Exception);
		PPL7EXCEPTION(InvalidURLException, Exception);
		PPL7EXCEPTION(NoResultException, Exception);
		PPL7EXCEPTION(OperationFailedException, Exception);


		enum HTTPVERSION {
			HTTP_1_0=1,
			HTTP_1_1,
			HTTP_2_0,
		};

		static bool isSupported();

		Curl();
		Curl(const Curl &other);
		~Curl();

		//TODO: CNotifyHandler call_receive;
		//TODO: CNotifyHandler call_send;
		//TODO: CNotifyHandler call_done;

		void	debugHandler(int type, const char *data, size_t size);
		size_t	storeResult(void *ptr, size_t bytes, int type);
		void	setLogger(Logger *log);

		void	setBrowser(const String &browser);
		void	setURL(const String &url);
		void	setReferer(const String &url);
		void	setUserPassword(const String &username, const String &password);
		void	setUsername(const String &username);
		void	setPassword(const String &password);
		void	setUserPassword(const String &userpassword);	// Format: username:password
		void	setProxy(const String &proxy, int port);
		void	setHttpVersion(Curl::HTTPVERSION version);
		void	setTimeout(int seconds);
		void	setHeader(const String &name, const String &value);
		void	setMaximumPersistantConnects(int value);
		void	enableSignals(bool enable);
		void	reset();
		void	clearHeader();
		void	get();
		void	get(const String &parameter);
		void	get(const AssocArray &param);
		void	getResultBuffer(void **buffer, size_t *size) const;
		ByteArrayPtr getResultBuffer() const;
		void	copyResultBuffer(ByteArray &bin) const;
		String	getResultBufferAsString() const;
		String	getHeader() const;
		String	getURL() const;
		String	getLastURL() const;
		void	escape(String &target, const AssocArray &source);
		void	escape(String &string);
		void	addPostVar(const String &name, const String &data, const String &contenttype=String());
		void	addPostVar(const String &name, int val, const String &contenttype=String());
		void	addPostVar(const AssocArray &param, const String &prefix=String());
		void	post();
		void	post(const AssocArray &param);
		void 	postFields(const AssocArray &param);
		void	clear();

		void	addCAFile(const String &filename);
		void	verifyPeer(bool verify);

		void *	getCurlHandle() const;

		static String getUri(const String &uri);
};



class WikiParser
{
	private:
		int ispre;
		int ullevel;
		size_t ollevel;
		int indexcount;
		int intable;
		int inrow;
		int indentlevel;
		bool doxyparamsStarted;
		bool indexenabled;

		AssocArray index;
		String incol;
		String ret;
		String BaseURI;
		Array nowiki;
		int nowikicount;
		bool nobr;

		int precount;
		AssocArray pre;
		int sourcecount;
		Array source;

		Array diagrams;


		void init();
		void renderInternal(const String &Source, String &Html);
		void extractNoWiki(String &Text);
		void extractSourcecode(String &Text);
		void extractDiagrams(String &Text);
		void parseHeadlines(String &Line);
		int parseUL(String &Line);
		int parseOL(String &Line);
		int parseIndent(String &Line);

		void parseDoxygen(String &Line);
		void doxygenChapter(String &Line, const String &Name, const Array &Matches);
		void parseAutoPRE(String &Line);
		void parseTable(String &Line);
		void parseLinks(String &Line);
		void buildIndex(String &Html);
		void finalize();
		void finalizeNoWiki();
		void finalizePRE();
		void finalizeSource();
		void finalizeDiagrams();

	protected:
		virtual void customParseLinks(String &Line);

	public:

		WikiParser();
		virtual ~WikiParser();
		void render(const String &Source, String &Html);
		void renderBody(const String &Source, String &Html);
		String render(const String &Source);
		String renderBody(const String &Source);
		String header();
		void setIndexEnabled(bool enabled);
		void setBaseURI(const String &Uri);
		static String xmlDiagram2HTML(const String &xml);
		virtual void getHeader(String &Html);
};


} // EOF namespace ppl7

#endif /* PPL7INET_H_ */
