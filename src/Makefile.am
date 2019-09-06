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
#CLEANFILES = dnsmeter.1

SUBDIRS =

AM_CXXFLAGS = -I$(srcdir) \
  -I$(top_srcdir) \
  -I$(srcdir)/pplib/include \
  $(PTHREAD_CFLAGS)

EXTRA_DIST = pplib #dnsmeter.1.in

BUILT_SOURCES = $(srcdir)/pplib/release/libppl7.a

bin_PROGRAMS = dnsmeter

dnsmeter_SOURCES = dns_receiver_thread.cpp dns_sender.cpp \
  dns_sender_thread.cpp main.cpp packet.cpp payload_file.cpp query.cpp \
  raw_socket_receiver.cpp raw_socket_sender.cpp system_stat.cpp
dist_dnsmeter_SOURCES = dns_receiver_thread.h dns_sender.h \
  dns_sender_thread.h exceptions.h packet.h payload_file.h query.h \
  raw_socket_receiver.h raw_socket_sender.h system_stat.h
dnsmeter_LDADD = $(PTHREAD_LIBS) $(srcdir)/pplib/release/libppl7.a

$(srcdir)/pplib/release/libppl7.a:
	cd $(srcdir)/pplib && ./configure --disable-oggtest \
--disable-freetypetest --disable-sdltest --disable-sdlframework \
--without-jpeg --without-libjpegturbo --without-libpng \
--without-libmicrohttpd --without-libmcrypt-prefix --without-libidn \
--without-libidn2 --without-libtiff --without-libcurl --without-mpg123 \
--without-lame --without-ogg --without-libcdio --without-libiconv-prefix \
--without-pcre --without-imlib --without-mysql --without-postgresql \
--without-sqlite3 --without-libldn --without-nasm && \
( make -j 4 || make )

#man1_MANS = dnsmeter.1
#
#dnsmeter.1: dnsmeter.1.in Makefile
#	sed -e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
#-e 's,[@]PACKAGE_URL[@],$(PACKAGE_URL),g' \
#-e 's,[@]PACKAGE_BUGREPORT[@],$(PACKAGE_BUGREPORT),g' \
#< $(srcdir)/dnsmeter.1.in > dnsmeter.1
