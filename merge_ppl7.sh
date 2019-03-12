#!/bin/bash
PPL7_DIR=~/git/ppl7

WORK=`pwd`
if [ ! -d $PPL7_DIR ] ; then
	echo "ERROR: ppl7 not found!"
	exit 1
fi

rm -rf ppl7/* && mkdir -p ppl7/include

cd $PPL7_DIR
find include/compat.h include/ppl7.h include/ppl7-algorithms.h include/ppl7-exceptions.h \
	include/ppl7-inet.h include/ppl7-types.h include/prolog.h include/socket.h \
	include/ppl7-crypto.h include/crypto.h \
	include/threads.h include/config.h.in include/ppl7-config.h.in $WORK/ppl7/include \
	src/core/Compat.cpp src/core/Dir* src/core/Exceptions* src/core/File* \
	src/core/Functions* src/core/MemFile* src/core/Mutex* src/core/Signal* \
	src/core/String* src/core/Thread* src/core/Time* src/core/AVL* \
	src/core/Memory* src/internet/resolver.cpp \
	src/internet/inet* src/internet/ip* src/internet/openssl* \
	src/internet/sock* src/internet/*Socket* src/math  src/types \
	| cpio -pdmv $WORK/ppl7
cp LICENSE.TXT $WORK/ppl7

rm -rf $WORK/ppl7/src/core/Resourcen.cpp

cp genMakefile.in $WORK/ppl7

cp autoconf/ax_cxx_compile_stdcxx.m4 $WORK/autoconf
cp autoconf/ax_path_lib_pcre.m4 $WORK/autoconf
cp autoconf/ax_pthread.m4 $WORK/autoconf
cp autoconf/lib-link.m4 $WORK/autoconf
cp autoconf/checkfuncs.m4 $WORK/autoconf
cp autoconf/ax_gcc_x86_cpuid.m4 $WORK/autoconf
cp autoconf/ax_check_compiler_flags.m4 $WORK/autoconf
cp autoconf/ax_gcc_archflag.m4 $WORK/autoconf
cp autoconf/libbind.m4 $WORK/autoconf
cp autoconf/config.rpath $WORK/autoconf
cp acinclude.m4 $WORK 

cd $WORK/ppl7
./genMakefile.in

cd $WORK
./genConfigure
   
