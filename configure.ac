AC_PREREQ(2.69)
AC_INIT([dnsmeter], [1.0.0], [admin@dns-oarc.net], [dnsmeter], [https://github.com/DNS-OARC/dnsmeter/issues])
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

# Checks for support.
AX_PTHREAD
AC_CHECK_LIB([pcap], [pcap_open_live], [], [AC_MSG_ERROR([libpcap not found])])
AC_CHECK_LIB([m], [sqrt])

# Check for PPL7
AC_ARG_WITH([ppl7], [AS_HELP_STRING([--with-ppl7=PATH], [Specify PPL7 path])], [
  use_ppl7="$withval"
],[
  use_ppl7="yes"
])

AC_MSG_CHECKING([for PPL7])
if test $use_ppl7 = no; then
  AC_MSG_ERROR([PPL7 must be installed])
elif test $use_ppl7 = yes; then
  ppl7path="$PATH"
  AC_MSG_RESULT([using \$PATH])
else
  ppl7path="$withval/bin"
  AC_MSG_RESULT([using $ppl7path])
fi

AC_PATH_PROG(ac_cv_ppl7_config, [ppl7-config], [no], [$ppl7path])
if test "$ac_cv_ppl7_config" = "no"; then
  AC_MSG_ERROR([PPL7 must be installed])
fi

AS_VAR_APPEND(CXXFLAGS, [" `$ac_cv_ppl7_config --cflags release`"])
AS_VAR_APPEND(LDFLAGS, [" `$ac_cv_ppl7_config --libs release`"])

# Output Makefiles
AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
