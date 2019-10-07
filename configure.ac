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

AC_PREREQ(2.69)
AC_INIT([dnsmeter], [1.0.1], [admin@dns-oarc.net], [dnsmeter], [https://github.com/DNS-OARC/dnsmeter/issues])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_CANONICAL_HOST
LT_INIT([disable-static])

# Check --enable-warn-all
AC_ARG_ENABLE([warn-all], [AS_HELP_STRING([--enable-warn-all], [Enable all compiler warnings])], [AX_CFLAGS_WARN_ALL()])

# Check --with-extra-cflags
AC_ARG_WITH([extra-cflags], [AS_HELP_STRING([--with-extra-cflags=CFLAGS], [Add extra CFLAGS/CXXFLAGS])], [
  AC_MSG_NOTICE([appending extra CFLAGS/CXXFLAGS... $withval])
  AS_VAR_APPEND(CFLAGS, [" $withval"])
  AS_VAR_APPEND(CXXFLAGS, [" $withval"])
])

# Check --with-extra-ldflags
AC_ARG_WITH([extra-ldflags], [AS_HELP_STRING([--with-extra-ldflags=CFLAGS], [Add extra LDFLAGS])], [
  AC_MSG_NOTICE([appending extra LDFLAGS... $withval])
  AS_VAR_APPEND(LDFLAGS, [" $withval"])
])

# Checks for support.
AX_PTHREAD
AC_CHECK_LIB([pcap], [pcap_open_live], [], [AC_MSG_ERROR([libpcap not found])])
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([bind], [ns_initparse], [], [AC_CHECK_LIB([bind], [__ns_initparse])])
AC_CHECK_LIB([resolv], [res_mkquery], [], [
	AC_CHECK_LIB([resolv], [__res_mkquery], [], [
		AC_CHECK_LIB([resolv], [res_9_mkquery])
	])
])
AC_CHECK_LIB([idn], [idna_to_ascii_4z])
AC_CHECK_LIB([idn2], [idn2_to_ascii_4z])
AC_CHECK_LIB([pcre], [pcre_exec])

# Check for OS specific libraries
case "$host_os" in
    freebsd*)
		AC_CHECK_LIB([kvm], [kvm_open])
        ;;
esac

# Output Makefiles
AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
