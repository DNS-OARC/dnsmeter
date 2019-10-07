# Copyright (c) 2019, OARC, Inc.
# Copyright (c) 2019, DENIC eG
# All rights reserved.
#
# This file is part of dnsmeter.
#
# dnsmeter is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dnsmeter is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dnsmeter.  If not, see <http://www.gnu.org/licenses/>.

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
CLEANFILES = dnsmeter.1 $(srcdir)/pplib/include/ppl7-config.h \
  $(srcdir)/pplib/release/libppl7.a

SUBDIRS =

AM_CXXFLAGS = -I$(srcdir) \
  -I$(top_srcdir) \
  -I$(srcdir)/pplib/include \
  $(PTHREAD_CFLAGS)

bin_PROGRAMS = dnsmeter

dnsmeter_SOURCES = dns_receiver_thread.cpp dns_sender.cpp \
  dns_sender_thread.cpp main.cpp packet.cpp payload_file.cpp query.cpp \
  raw_socket_receiver.cpp raw_socket_sender.cpp system_stat.cpp
dist_dnsmeter_SOURCES = dns_receiver_thread.h dns_sender.h \
  dns_sender_thread.h exceptions.h packet.h payload_file.h query.h \
  raw_socket_receiver.h raw_socket_sender.h system_stat.h
dnsmeter_LDADD = $(PTHREAD_LIBS) $(srcdir)/pplib/release/libppl7.a

BUILT_SOURCES = pplib/include/ppl7-config.h

pplib/include/ppl7-config.h:
	cd "$(srcdir)/pplib" && ./configure --disable-oggtest \
--disable-freetypetest --disable-sdltest --disable-sdlframework \
--without-jpeg --without-libjpegturbo --without-libpng \
--without-libmicrohttpd --without-libmcrypt-prefix \
--without-libtiff --without-libcurl --without-mpg123 \
--without-lame --without-ogg --without-libcdio --without-libiconv-prefix \
--without-imlib --without-mysql --without-postgresql \
--without-sqlite3 --without-libldn --without-nasm

$(srcdir)/pplib/release/libppl7.a: pplib/include/ppl7-config.h
	cd "$(srcdir)/pplib" && make

man1_MANS = dnsmeter.1

dnsmeter.1: dnsmeter.1.in Makefile
	sed -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
-e 's,[@]PACKAGE_URL[@],$(PACKAGE_URL),g' \
-e 's,[@]PACKAGE_BUGREPORT[@],$(PACKAGE_BUGREPORT),g' \
< $(srcdir)/dnsmeter.1.in > dnsmeter.1

EXTRA_DIST = dnsmeter.1.in \
  pplib/Makefile.in \
  pplib/src/types/ByteArrayPtr.cpp \
  pplib/src/types/ByteArray.cpp \
  pplib/src/types/Array.cpp \
  pplib/src/types/Pointer.cpp \
  pplib/src/types/WideString.cpp \
  pplib/src/types/AssocArray.cpp \
  pplib/src/types/String.cpp \
  pplib/src/types/Variant.cpp \
  pplib/src/types/DateTime.cpp \
  pplib/src/math/crc32.cpp \
  pplib/src/math/calc.cpp \
  pplib/src/math/md5.cpp \
  pplib/src/math/random.cpp \
  pplib/src/database/MySQL.cpp \
  pplib/src/database/ResultSet.cpp \
  pplib/src/database/Database.cpp \
  pplib/src/database/PostgreSQL.cpp \
  pplib/src/database/DBPool.cpp \
  pplib/src/database/DBPoolOfPools.cpp \
  pplib/src/database/Sqlite3.cpp \
  pplib/src/audio/AudioCD.cpp \
  pplib/src/audio/AudioEncoder_MP3.cpp \
  pplib/src/audio/AudioDecoder_Ogg.cpp \
  pplib/src/audio/AudioInfo.cpp \
  pplib/src/audio/Cddb.cpp \
  pplib/src/audio/AudioDecoder_MP3.cpp \
  pplib/src/audio/AudioDecoder_Aiff.cpp \
  pplib/src/audio/AudioEncoder_Wave.cpp \
  pplib/src/audio/Icecast.cpp \
  pplib/src/audio/AudioEncoder_Ogg.cpp \
  pplib/src/audio/ID3Tag.cpp \
  pplib/src/audio/AudioDecoder_Wave.cpp \
  pplib/src/audio/Mp3.cpp \
  pplib/src/audio/AudioEncoder_Aiff.cpp \
  pplib/src/toolkit/Divider.cpp \
  pplib/src/toolkit/WindowManager.cpp \
  pplib/src/toolkit/Window.cpp \
  pplib/src/toolkit/LineInput.cpp \
  pplib/src/toolkit/WindowManager_SDL2.cpp \
  pplib/src/toolkit/Layout.cpp \
  pplib/src/toolkit/Widget.cpp \
  pplib/src/toolkit/Surface.cpp \
  pplib/src/toolkit/Frame.cpp \
  pplib/src/toolkit/Label.cpp \
  pplib/src/toolkit/Button.cpp \
  pplib/src/toolkit/Event.cpp \
  pplib/src/asm/common.asm \
  pplib/src/asm/colors.asm \
  pplib/src/asm/pixel.asm \
  pplib/src/asm/chromakey.asm \
  pplib/src/asm/cpu.asm \
  pplib/src/asm/blt_blend.asm \
  pplib/src/asm/rect.asm \
  pplib/src/asm/fonts.asm \
  pplib/src/asm/blt.asm \
  pplib/src/internet/WikiParser.cpp \
  pplib/src/internet/Webserver.cpp \
  pplib/src/internet/ipaddress.cpp \
  pplib/src/internet/sockaddr.cpp \
  pplib/src/internet/resolver.cpp \
  pplib/src/internet/UDPSocket.cpp \
  pplib/src/internet/ipnetwork.cpp \
  pplib/src/internet/curl.cpp \
  pplib/src/internet/SocketMessage.cpp \
  pplib/src/internet/inet_functions.cpp \
  pplib/src/internet/openssl.cpp \
  pplib/src/internet/TCPSocket.cpp \
  pplib/src/crypto/Digest.cpp \
  pplib/src/crypto/Crypt.cpp \
  pplib/src/crypto/MCrypt.cpp \
  pplib/src/core/Signal.cpp \
  pplib/src/core/Compat.cpp \
  pplib/src/core/Mutex.cpp \
  pplib/src/core/ThreadPool.cpp \
  pplib/src/core/Functions.cpp \
  pplib/src/core/MemoryGroup.cpp \
  pplib/src/core/MemFile.cpp \
  pplib/src/core/StringFunctions.cpp \
  pplib/src/core/Dir.cpp \
  pplib/src/core/File.cpp \
  pplib/src/core/ConfigParser.cpp \
  pplib/src/core/DirEntry.cpp \
  pplib/src/core/Resourcen.cpp \
  pplib/src/core/Exceptions.cpp \
  pplib/src/core/FileObject.cpp \
  pplib/src/core/PerlHelper.cpp \
  pplib/src/core/Json.cpp \
  pplib/src/core/Iconv.cpp \
  pplib/src/core/AVLTree.cpp \
  pplib/src/core/Logger.cpp \
  pplib/src/core/Threads.cpp \
  pplib/src/core/PythonHelper.cpp \
  pplib/src/core/Compression.cpp \
  pplib/src/core/Time.cpp \
  pplib/src/core/cpu.cpp \
  pplib/src/core/MemoryHeap.cpp \
  pplib/src/core/PFPFile.cpp \
  pplib/src/core/Resource.cpp \
  pplib/src/core/GzFile.cpp \
  pplib/src/grafix/Point.cpp \
  pplib/src/grafix/ImageFilter_PPM.cpp \
  pplib/src/grafix/Font5.cpp \
  pplib/src/grafix/ImageFilter_JPEG.cpp \
  pplib/src/grafix/Rect.cpp \
  pplib/src/grafix/DrawableBlit.cpp \
  pplib/src/grafix/Point3D.cpp \
  pplib/src/grafix/RGBFormat.cpp \
  pplib/src/grafix/ImageFilter_GIF.cpp \
  pplib/src/grafix/Fonts.cpp \
  pplib/src/grafix/ImageFilter_TGA.cpp \
  pplib/src/grafix/ImageFilter_PNG.cpp \
  pplib/src/grafix/FontFreeType.cpp \
  pplib/src/grafix/Grafix.cpp \
  pplib/src/grafix/Font6.cpp \
  pplib/src/grafix/DrawablePixel.cpp \
  pplib/src/grafix/ImageFilter_BMP.cpp \
  pplib/src/grafix/Sprite.cpp \
  pplib/src/grafix/ImageFilter_TIFF.cpp \
  pplib/src/grafix/Size.cpp \
  pplib/src/grafix/ImageList.cpp \
  pplib/src/grafix/DrawableLines.cpp \
  pplib/src/grafix/DrawableColor.cpp \
  pplib/src/grafix/DrawableShapes.cpp \
  pplib/src/grafix/Image.cpp \
  pplib/src/grafix/ImageFilter.cpp \
  pplib/src/grafix/ImageFilter_ImageMagick.cpp \
  pplib/src/grafix/Font4.cpp \
  pplib/src/grafix/Drawable.cpp \
  pplib/src/grafix/Color.cpp \
  pplib/genConfigure \
  pplib/genMakefile.in \
  pplib/aclocal.m4 \
  pplib/conf.sh \
  pplib/include/crypto.h \
  pplib/include/ppl7-ppl6compat.h \
  pplib/include/ppl7.h \
  pplib/include/ppl7-tk.h \
  pplib/include/ppl7-visualc-config.h \
  pplib/include/ppl7-config.h.in \
  pplib/include/config.h.in \
  pplib/include/ppl7-inet.h \
  pplib/include/ppl7-audio.h \
  pplib/include/threads.h \
  pplib/include/ppl7-grafix.h \
  pplib/include/socket.h \
  pplib/include/ppl7-crypto.h \
  pplib/include/prolog.h \
  pplib/include/ppl7-types.h \
  pplib/include/ppl7-algorithms.h \
  pplib/include/ppl7-exceptions.h \
  pplib/include/ppl7-db.h \
  pplib/include/compat.h \
  pplib/tests/Makefile.in \
  pplib/tests/src/toolkit.h \
  pplib/tests/src/stringspeed.cpp \
  pplib/tests/src/database/db_mysql.cpp \
  pplib/tests/src/database/db_sqlite.cpp \
  pplib/tests/src/database/db_postgres.cpp \
  pplib/tests/src/audio/id3tag.cpp \
  pplib/tests/src/audio/audio_decoder_aiff.cpp \
  pplib/tests/src/audio/audio_decoder_mp3.cpp \
  pplib/tests/src/audio/audio_encoder_mp3.cpp \
  pplib/tests/src/audio/audioinfo.cpp \
  pplib/tests/src/audio/audio_encoder_aiff.cpp \
  pplib/tests/src/audio/audio_encoder_wave.cpp \
  pplib/tests/src/audio/audio_decoder_wave.cpp \
  pplib/tests/src/ppl6/strings.cpp \
  pplib/tests/src/ppl6/assocarray.cpp \
  pplib/tests/src/crypto/mcrypt.cpp \
  pplib/tests/src/crypto/crypto.cpp \
  pplib/tests/src/crypto/digest.cpp \
  pplib/tests/src/wordlist.cpp \
  pplib/tests/src/core/stringfunctions.cpp \
  pplib/tests/src/core/iconv.cpp \
  pplib/tests/src/core/functions.cpp \
  pplib/tests/src/core/json.cpp \
  pplib/tests/src/core/filestatic.cpp \
  pplib/tests/src/core/strings.cpp \
  pplib/tests/src/core/gzfile.cpp \
  pplib/tests/src/core/pythonhelper.cpp \
  pplib/tests/src/core/memoryheap.cpp \
  pplib/tests/src/core/avltree.cpp \
  pplib/tests/src/core/variant.cpp \
  pplib/tests/src/core/pointer.cpp \
  pplib/tests/src/core/math.cpp \
  pplib/tests/src/core/dir.cpp \
  pplib/tests/src/core/configparser.cpp \
  pplib/tests/src/core/time.cpp \
  pplib/tests/src/core/bytearray.cpp \
  pplib/tests/src/core/datetime.cpp \
  pplib/tests/src/core/memorygroup.cpp \
  pplib/tests/src/core/bytearrayptr.cpp \
  pplib/tests/src/core/widestrings.cpp \
  pplib/tests/src/core/array.cpp \
  pplib/tests/src/core/perlhelper.cpp \
  pplib/tests/src/core/file.cpp \
  pplib/tests/src/core/logger.cpp \
  pplib/tests/src/core/assocarray.cpp \
  pplib/tests/src/core/list.cpp \
  pplib/tests/src/loggertest.cpp \
  pplib/tests/src/grafix/grafix_image.cpp \
  pplib/tests/src/grafix/grafix_rect.cpp \
  pplib/tests/src/grafix/grafix_imagefilter.cpp \
  pplib/tests/src/grafix/grafix_size.cpp \
  pplib/tests/src/grafix/grafix_font.cpp \
  pplib/tests/src/grafix/grafix_rgbformat.cpp \
  pplib/tests/src/grafix/grafix_point3d.cpp \
  pplib/tests/src/grafix/grafix_point.cpp \
  pplib/tests/src/grafix/grafix.cpp \
  pplib/tests/src/grafix/grafix_color.cpp \
  pplib/tests/src/grafix/grafix_drawable.cpp \
  pplib/tests/src/gfxreftest.cpp \
  pplib/tests/src/toolkit.cpp \
  pplib/tests/src/textsnippets.cpp \
  pplib/tests/src/main.cpp \
  pplib/tests/src/threadtest.cpp \
  pplib/tests/src/inet/tcpsocket.cpp \
  pplib/tests/src/inet/ipaddress.cpp \
  pplib/tests/src/inet/wikiparser.cpp \
  pplib/tests/src/inet/sockaddr.cpp \
  pplib/tests/src/inet/resolver.cpp \
  pplib/tests/src/inet/inet.cpp \
  pplib/tests/src/inet/ipnetwork.cpp \
  pplib/tests/create_postgres_db.sh \
  pplib/tests/valgrind.suppressions \
  pplib/tests/gcovr \
  pplib/tests/test.conf \
  pplib/tests/ppl7-tests.h \
  pplib/tests/testdata/test_192cbr_taggedWithCover.mp3 \
  pplib/tests/testdata/test_192vbr.mp3 \
  pplib/tests/testdata/unittest.png \
  pplib/tests/testdata/ppl7-icon-64x64.png \
  pplib/tests/testdata/test_44kHz_tagged.wav \
  pplib/tests/testdata/test.pcx \
  pplib/tests/testdata/unicodeUSASCII.txt \
  pplib/tests/testdata/test-pal-trans.png \
  pplib/tests/testdata/fonts/liberationsans8.fnt5 \
  pplib/tests/testdata/fonts/LiberationSans-Bold.ttf \
  pplib/tests/testdata/fonts/freesans4.fnt5 \
  pplib/tests/testdata/fonts/segoeui4.fnt5 \
  pplib/tests/testdata/filenameUSASCII.txt \
  pplib/tests/testdata/database/postgresql.sql \
  pplib/tests/testdata/database/mysql.sql \
  pplib/tests/testdata/database/sqlite3.sql \
  pplib/tests/testdata/test_192cbr_tagged.mp3 \
  pplib/tests/testdata/test.tga \
  pplib/tests/testdata/test.png \
  pplib/tests/testdata/jsontest2.json \
  pplib/tests/testdata/jsontest4.json \
  pplib/tests/testdata/compression.txt \
  pplib/tests/testdata/mpg123.h \
  pplib/tests/testdata/test.gif \
  pplib/tests/testdata/cover.jpg \
  pplib/tests/testdata/unittest.bmp \
  pplib/tests/testdata/screenshot1.png \
  pplib/tests/testdata/lame.h \
  pplib/tests/testdata/example.conf \
  pplib/tests/testdata/test.tif \
  pplib/tests/testdata/dirwalk/testfile.txt \
  pplib/tests/testdata/dirwalk/zfile.txt \
  pplib/tests/testdata/dirwalk/file1.txt \
  pplib/tests/testdata/dirwalk/LICENSE.TXT \
  pplib/tests/testdata/dirwalk/file3.txt \
  pplib/tests/testdata/dirwalk/file4äöü.txt \
  pplib/tests/testdata/dirwalk/afile.txt \
  pplib/tests/testdata/dirwalk/file2.txt \
  pplib/tests/testdata/test.jpg \
  pplib/tests/testdata/reference.png \
  pplib/tests/testdata/lame_API.txt \
  pplib/tests/testdata/filenameUTF8äöü.txt \
  pplib/tests/testdata/tags_at_eof.aiff \
  pplib/tests/testdata/test_44kHz.aiff \
  pplib/tests/testdata/test_44kHz_tagged.aiff \
  pplib/tests/testdata/compression.txt.gz \
  pplib/tests/testdata/test_44kHz_taggedWithCover.aiff \
  pplib/tests/testdata/test_192cbr.mp3 \
  pplib/tests/testdata/test_44kHz.wav \
  pplib/tests/testdata/test.ppm \
  pplib/tests/testdata/jsontest1.json \
  pplib/tests/testdata/unicodeUtf8äöü.txt \
  pplib/tests/testdata/jsontest3.json \
  pplib/tests/testdata/test_320cbr.mp3 \
  pplib/tests/testdata/test.bmp \
  pplib/tests/testdata/test-pal.png \
  pplib/LICENSE.TXT \
  pplib/configure \
  pplib/autoconf/iconv.m4 \
  pplib/autoconf/imlib.m4 \
  pplib/autoconf/libidn.m4 \
  pplib/autoconf/imagemagick.m4 \
  pplib/autoconf/freetds.m4 \
  pplib/autoconf/ax_lib_gcrypt.m4 \
  pplib/autoconf/checkfuncs.m4 \
  pplib/autoconf/libtiff.m4 \
  pplib/autoconf/ldns.m4 \
  pplib/autoconf/bzip2.m4 \
  pplib/autoconf/ax_lib_crypto.m4 \
  pplib/autoconf/ax_lib_sqlite3.m4 \
  pplib/autoconf/lib-link.m4 \
  pplib/autoconf/ax_check_compiler_flags.m4 \
  pplib/autoconf/ax_path_lib_pcre.m4 \
  pplib/autoconf/nasm.m4 \
  pplib/autoconf/missing \
  pplib/autoconf/libcdio.m4 \
  pplib/autoconf/libjpeg-turbo.m4 \
  pplib/autoconf/libmicrohttpd.m4 \
  pplib/autoconf/ax_gcc_x86_cpuid.m4 \
  pplib/autoconf/ogg.m4 \
  pplib/autoconf/compile \
  pplib/autoconf/ax_pthread.m4 \
  pplib/autoconf/sdl2.m4 \
  pplib/autoconf/ax_cxx_compile_stdcxx.m4 \
  pplib/autoconf/ax_check_openssl.m4 \
  pplib/autoconf/ax_lib_mysql.m4 \
  pplib/autoconf/ax_lib_postgresql.m4 \
  pplib/autoconf/ax_have_qt.m4 \
  pplib/autoconf/ax_gcc_archflag.m4 \
  pplib/autoconf/gtest.m4 \
  pplib/autoconf/ax_check_zlib.m4 \
  pplib/autoconf/shout.m4 \
  pplib/autoconf/jpeg.m4 \
  pplib/autoconf/libcurl.m4 \
  pplib/autoconf/config.rpath \
  pplib/autoconf/ax_cc_maxopt.m4 \
  pplib/autoconf/freetype.m4 \
  pplib/autoconf/install-sh \
  pplib/autoconf/config.sub \
  pplib/autoconf/lame.m4 \
  pplib/autoconf/libbind.m4 \
  pplib/autoconf/config.guess \
  pplib/autoconf/libidn2.m4 \
  pplib/autoconf/png.m4 \
  pplib/autoconf/mpg123.m4 \
  pplib/autoconf/libmcrypt.m4 \
  pplib/autoconf/ax_cxx_compile_stdcxx_11.m4 \
  pplib/TODO.TXT \
  pplib/docs/threads.dox \
  pplib/docs/credits.dox \
  pplib/docs/CWikiParser_diagram.png \
  pplib/docs/groups.dox \
  pplib/docs/formate.dox \
  pplib/docs/pcrenote.dox \
  pplib/docs/CWStringSetFunctions.dox \
  pplib/docs/pregexpr.dox \
  pplib/docs/verwendung.dox \
  pplib/docs/win32-vc9-clr.png \
  pplib/docs/header-bg.png \
  pplib/docs/ppl7style.css \
  pplib/docs/CDrawable.png \
  pplib/docs/Class_Point.png \
  pplib/docs/datentypen.dox \
  pplib/docs/main.dox \
  pplib/docs/DoxygenLayout.xml \
  pplib/docs/sprintf.dox \
  pplib/docs/strftime.dox \
  pplib/docs/examples/nasm.rules \
  pplib/docs/examples/db_examples.cpp \
  pplib/docs/examples/CThread_ThreadMain.cpp \
  pplib/docs/examples/IcecastExample.cpp \
  pplib/docs/examples/ppl7.m4 \
  pplib/docs/installation.dox \
  pplib/docs/gtest.dox \
  pplib/docs/sockets.dox \
  pplib/docs/icecast_status.png \
  pplib/docs/formate/PFPVersion1.dox \
  pplib/docs/formate/PFPVersion2.dox \
  pplib/docs/formate/SocketMessage.dox \
  pplib/docs/formate/PFPVersion3.dox \
  pplib/docs/formate/PPLResVersion6.dox \
  pplib/docs/structures.dox \
  pplib/docs/header.html \
  pplib/docs/win32-vc9-rl.png \
  pplib/docs/datenbanken.dox \
  pplib/Doxyfile \
  pplib/configure.ac \
  pplib/HISTORY.TXT \
  pplib/acinclude.m4 \
  pplib/ppl7-config.in \
  pplib/resource/ppl7-icon-16x16.png \
  pplib/resource/ppl7-icon-64x64.png \
  pplib/resource/toolbar.png \
  pplib/resource/wikistyle.css \
  pplib/resource/liberationsans2.fnt6 \
  pplib/resource/resourcen.lst \
  pplib/resource/ppl7.ico \
  pplib/resource/ppl7-icon-256x256.png \
  pplib/resource/ppl7-icon-32x32.png \
  pplib/resource/ButtonSymbols.png \
  pplib/resource/ppl7-icon-128x128.png \
  pplib/resource/ppl7-icon-48x48.png \
  pplib/resource/cursor.png \
  pplib/resource/makefonts.sh \
  pplib/resource/mimetypes.png \
  pplib/resource/liberationmono2.fnt6 \
  pplib/resource/res.h \
  pplib/resource/SymbolsAlpha.png \
  pplib/resource/icons.png \
  pplib/README.TXT
