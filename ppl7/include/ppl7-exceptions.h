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

#ifndef PPL7EXCEPTIONS_H_
#define PPL7EXCEPTIONS_H_
#include <exception>
#include <ostream>

namespace ppl7 {



class String;

void throwExceptionFromErrno(int e,const String &info);
void throwExceptionFromEaiError(int ecode, const String &info);

class Exception : std::exception
{
	private:
		char *ErrorText;
	public:
		Exception() throw();
		Exception(const Exception &other) throw();
		Exception& operator= (const Exception &other) throw();
		Exception(const char *msg, ...) throw();
		virtual ~Exception() throw();
		virtual const char* what() const throw();
		const char* text() const throw();
		String toString() const throw();
		void print() const;
		void copyText(const char *str) throw();
		void copyText(const char *fmt, va_list args) throw();
};

std::ostream& operator<<(std::ostream& s, const Exception &e);


#define STR_VALUE(arg)      #arg
#define PPL7EXCEPTION(name,inherit)	class name : public ppl7::inherit { public: \
	name() throw() {}; \
	name(const char *msg, ...) throw() {  \
		va_list args; va_start(args, msg); copyText(msg,args); \
		va_end(args); } \
		virtual const char* what() const throw() { return (STR_VALUE(name)); } \
	};

PPL7EXCEPTION(UnknownException, Exception);
PPL7EXCEPTION(OutOfMemoryException, Exception);
PPL7EXCEPTION(NullPointerException, Exception);
PPL7EXCEPTION(UnsupportedFeatureException, Exception);
PPL7EXCEPTION(CharacterEncodingException, Exception);
PPL7EXCEPTION(UnsupportedCharacterEncodingException, Exception);
PPL7EXCEPTION(OutOfBoundsEception, Exception);
PPL7EXCEPTION(EmptyDataException, Exception);
PPL7EXCEPTION(TypeConversionException, Exception);
PPL7EXCEPTION(IllegalArgumentException, Exception);
PPL7EXCEPTION(MissingArgumentException, Exception);
PPL7EXCEPTION(IllegalRegularExpressionException, Exception);
PPL7EXCEPTION(OperationFailedException, Exception);
PPL7EXCEPTION(OperationAbortedException, Exception);
PPL7EXCEPTION(DuplicateInstanceException, Exception);
PPL7EXCEPTION(ConnectionFailedException, Exception);
PPL7EXCEPTION(SocketException, Exception);
PPL7EXCEPTION(LoginRefusedException, Exception);
PPL7EXCEPTION(AlreadyConnectedException, Exception);
PPL7EXCEPTION(NoConnectionException, Exception);
PPL7EXCEPTION(TooManyInstancesException, Exception);
PPL7EXCEPTION(InvalidDateException, Exception);
PPL7EXCEPTION(DateOutOfRangeException, Exception);
PPL7EXCEPTION(NoThreadSupportException, Exception);
PPL7EXCEPTION(ThreadStartException, Exception);
PPL7EXCEPTION(ThreadAlreadyRunningException, Exception);
PPL7EXCEPTION(ThreadOperationFailedException, Exception);
PPL7EXCEPTION(ThreadAlreadyInPoolException, Exception);
PPL7EXCEPTION(ThreadNotInPoolException, Exception);
PPL7EXCEPTION(ItemNotFoundException, Exception);
PPL7EXCEPTION(DuplicateItemException, Exception);
PPL7EXCEPTION(UnsupportedDataTypeException, Exception);
PPL7EXCEPTION(ItemNotFromThisListException, Exception);
PPL7EXCEPTION(EndOfListException, Exception);
PPL7EXCEPTION(IllegalMemoryAddressException, Exception);
PPL7EXCEPTION(UnimplementedVirtualFunctionException, Exception);
PPL7EXCEPTION(UnknownCompressionMethodException, Exception);
PPL7EXCEPTION(IllegalChunkException, Exception);
PPL7EXCEPTION(ChunkNotFoundException, Exception);
PPL7EXCEPTION(EmptyFileException, Exception);
PPL7EXCEPTION(CompressionFailedException, Exception);
PPL7EXCEPTION(DecompressionFailedException, Exception);
PPL7EXCEPTION(InvalidFormatException, Exception);
PPL7EXCEPTION(AccessDeniedByInstanceException, Exception);
PPL7EXCEPTION(BufferTooSmallException, Exception);
PPL7EXCEPTION(CorruptedDataException, Exception);
PPL7EXCEPTION(FailedToLoadResourceException, Exception);
PPL7EXCEPTION(InvalidResourceException, Exception);
PPL7EXCEPTION(ResourceNotFoundException, Exception);
PPL7EXCEPTION(OperationUnavailableException, Exception);
PPL7EXCEPTION(UnavailableException, Exception);
PPL7EXCEPTION(InitializationFailedException, Exception);
PPL7EXCEPTION(KeyNotFoundException, Exception);
PPL7EXCEPTION(InvalidTimezoneException, Exception);
PPL7EXCEPTION(CharacterEncodingNotInitializedException, Exception);
PPL7EXCEPTION(MutexException, Exception);
PPL7EXCEPTION(MutexLockingException, MutexException);
PPL7EXCEPTION(MutexNotLockedException, MutexLockingException);
PPL7EXCEPTION(UnexpectedEndOfDataException, Exception);
PPL7EXCEPTION(InvalidEscapeSequenceException, Exception);
PPL7EXCEPTION(UnexpectedCharacterException, Exception);
PPL7EXCEPTION(SyntaxException, Exception);

PPL7EXCEPTION(NoSectionSelectedException, Exception);
PPL7EXCEPTION(UnknownSectionException, Exception);

PPL7EXCEPTION(SSLException, Exception);
PPL7EXCEPTION(SSLContextInUseException, Exception);
PPL7EXCEPTION(SSLContextUninitializedException, Exception);
PPL7EXCEPTION(SSLContextReferenceCounterMismatchException, Exception);
PPL7EXCEPTION(InvalidSSLCertificateException, Exception);
PPL7EXCEPTION(InvalidSSLCipherException, Exception);
PPL7EXCEPTION(SSLPrivatKeyException, Exception);
PPL7EXCEPTION(SSLFailedToReadDHParams, Exception);


//! @name IO-Exceptions
//@{
PPL7EXCEPTION(IOException,Exception);

PPL7EXCEPTION(FileNotOpenException, IOException);
PPL7EXCEPTION(FileSeekException, IOException);
PPL7EXCEPTION(ReadException, IOException);
PPL7EXCEPTION(WriteException, IOException);
PPL7EXCEPTION(EndOfFileException, IOException);
PPL7EXCEPTION(FileOpenException, IOException);
PPL7EXCEPTION(FileNotFoundException, IOException);					// ENOENT
PPL7EXCEPTION(InvalidArgumentsException, IOException);				// EINVAL
PPL7EXCEPTION(InvalidFileNameException, IOException);				// ENOTDIR, ENAMETOOLONG, ELOOP
PPL7EXCEPTION(PermissionDeniedException, IOException);				// EACCESS, EPERM
PPL7EXCEPTION(ReadOnlyException, IOException);						// EROFS
PPL7EXCEPTION(NoRegularFileException, IOException);					// EISDIR
PPL7EXCEPTION(TooManyOpenFilesException, IOException);				// EMFILE
PPL7EXCEPTION(UnsupportedFileOperationException, IOException);		// EOPNOTSUPP
PPL7EXCEPTION(TooManySymbolicLinksException, IOException);			// ELOOP
PPL7EXCEPTION(FilesystemFullException, IOException);					// ENOSPC
PPL7EXCEPTION(QuotaExceededException, IOException);					// EDQUOT
PPL7EXCEPTION(IOErrorException, IOException);						// EIO
PPL7EXCEPTION(BadFiledescriptorException, IOException);				// EABDF
PPL7EXCEPTION(BadAddressException, IOException);						// EFAULT
PPL7EXCEPTION(OverflowException, IOException);						// EOVERFLOW
PPL7EXCEPTION(FileExistsException, IOException);						// EEXIST
PPL7EXCEPTION(OperationBlockedException, IOException);				// EAGAIN
PPL7EXCEPTION(DeadlockException, IOException);						// EDEADLK
PPL7EXCEPTION(OperationInterruptedException, IOException);			// EINTR
PPL7EXCEPTION(TooManyLocksException, IOException);					// ENOLCK
PPL7EXCEPTION(IllegalOperationOnPipeException, IOException);			// ESPIPE
PPL7EXCEPTION(BufferExceedsLimitException, IOException);

//@}


PPL7EXCEPTION(HostNotFoundException, Exception);
PPL7EXCEPTION(TryAgainException, Exception);
PPL7EXCEPTION(NoResultException, Exception);
PPL7EXCEPTION(TimeoutException, Exception);

PPL7EXCEPTION(QueryFailedException, Exception);
PPL7EXCEPTION(EscapeFailedException, Exception);
PPL7EXCEPTION(FieldNotInResultSetException, Exception);



}	// EOF namespace ppl7

#endif /* PPL7EXCEPTIONS_H_ */
