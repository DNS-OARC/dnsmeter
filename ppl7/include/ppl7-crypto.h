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

#ifndef PPL7CRYPTO_H_
#define PPL7CRYPTO_H_

#ifndef _PPL7_INCLUDE
    #ifdef PPL7LIB
        #include "ppl7.h"
    #else
        #include <ppl7.h>
    #endif
#endif

namespace ppl7 {

PPL7EXCEPTION (InvalidAlgorithmException, Exception);
PPL7EXCEPTION (NoAlgorithmSpecifiedException, Exception);
PPL7EXCEPTION (InvalidBlocksizeException, Exception);
PPL7EXCEPTION (HashFailedException, OperationFailedException);
PPL7EXCEPTION (NoKeySpecifiedException, Exception);
PPL7EXCEPTION (NoIVSpecifiedException, Exception);
PPL7EXCEPTION (EncryptionFailedException, OperationFailedException);
PPL7EXCEPTION (DecryptionFailedException, OperationFailedException);

class MCrypt
{

	private:
		void *mcrypt;
		ByteArray IV;
		ByteArray Key;


	public:
		enum Mode {
			Mode_STREAM,
			Mode_ECB,
			Mode_CBC,
			Mode_CFB,
			Mode_OFB,
			Mode_nOFB,
			Mode_nCFB,
			Mode_CTR
		};


		enum Algorithm {
			Algo_DES,
			Algo_TRIPLE_DES,
			Algo_CAST_128,
			Algo_CAST_256,
			Algo_xTEA,
			Algo_THREE_WAY,
			Algo_SKIPJACK,
			Algo_BLOWFISH,
			Algo_TWOFISH,
			Algo_LOKI97,
			Algo_RC2,
			Algo_ARCFOUR,
			Algo_RC6,
			Algo_RIJNDAEL_128,
			Algo_RIJNDAEL_192,
			Algo_RIJNDAEL_256,
			Algo_MARS,
			Algo_PANAMA,
			Algo_WAKE,
			Algo_SERPENT,
			Algo_IDEA,
			Algo_ENIGMA,
			Algo_GOST,
			Algo_SAFER_SK64,
			Algo_SAFER_SK128,
			Algo_SAFER_PLUS
		};

		MCrypt();
		MCrypt(Algorithm algo, Mode mode);
		~MCrypt();
		void setAlgorithm(Algorithm algo=MCrypt::Algo_TWOFISH, Mode mode=MCrypt::Mode_CFB);
		int getIVSize() const;
		int getMaxKeySize() const;
		void setIV(const void *buffer, size_t size);
		void setIV(const Variant &object);
		void setKey(const void *buffer, size_t size);
		void setKey(const Variant &object);
		bool needIV() const;
		//int SupportKeySize(size_t size);

		void crypt(void *buffer, size_t size);
		void crypt(const Variant &in, ByteArray &out);
		void crypt(ByteArrayPtr &buffer);

		void decrypt(void *buffer, size_t size);
		void decrypt(const ByteArrayPtr &in, ByteArray &out);
		void decrypt(ByteArrayPtr &buffer);

		static void crypt(ByteArrayPtr &buffer, const Variant &key, Algorithm algo, Mode mode);
		static void crypt(ByteArrayPtr &buffer, const Variant &key, const Variant &IV, Algorithm algo, Mode mode);
		static void decrypt(ByteArrayPtr &buffer, const Variant &key, Algorithm algo, Mode mode);
		static void decrypt(ByteArrayPtr &buffer, const Variant &key, const Variant &IV, Algorithm algo, Mode mode);

};

class Digest
{
	private:
		const void *m;
		void *ctx;
		unsigned char *ret;
		ppluint64 bytecount;

	public:
		enum Algorithm {
			Algo_MD4,
			Algo_MD5,
			Algo_SHA1,
			Algo_SHA224,
			Algo_SHA256,
			Algo_SHA384,
			Algo_SHA512,
			Algo_WHIRLPOOL,
			Algo_RIPEMD160
		};


		Digest();
		Digest(const String &name);
		Digest(Algorithm algorithm);
		~Digest();

		void setAlgorithm(Algorithm algorithm);
		void setAlgorithm(const String &name);
		void addData(const void *data, size_t size);
		void addData(const ByteArrayPtr &data);
		void addData(const String &data);
		void addData(const WideString &data);
		void addData(FileObject &file);
		void addFile(const String &filename);
		ByteArray getDigest();
		void saveDigest(ByteArray &result);
		void saveDigest(String &result);
		void saveDigest(WideString &result);

		void reset();
		ppluint64 bytesHashed() const;

		static ByteArray hash(const ByteArrayPtr &data, Algorithm algorithm);
		static ByteArray hash(const ByteArrayPtr &data, const String &algorithmName);
		static ByteArray md4(const ByteArrayPtr &data);
		static ByteArray md5(const ByteArrayPtr &data);
		static ByteArray sha1(const ByteArrayPtr &data);
		static ByteArray sha224(const ByteArrayPtr &data);
		static ByteArray sha256(const ByteArrayPtr &data);
		static ByteArray sha384(const ByteArrayPtr &data);
		static ByteArray sha512(const ByteArrayPtr &data);
		static ppluint32 crc32(const ByteArrayPtr &data);
		static ppluint32 adler32(const ByteArrayPtr &data);

};


} // EOF namespace ppl7

#endif /* PPL7CRYPTO_H_ */
