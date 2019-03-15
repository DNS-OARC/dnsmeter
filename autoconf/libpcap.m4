dnl AX_PATH_LIB_PCAP [(A/NA)]
AC_DEFUN([AX_PATH_LIB_PCAP],[dnl
AC_MSG_CHECKING([libpcap])
AC_ARG_WITH([libpcap],
	[  --with-libpcap[[=prefix]]   Prefix where libpcap is installed (optional)],,
     with_libpcap="yes")
if test ".$with_libpcap" = ".no" ; then
  AC_MSG_RESULT([disabled])
  m4_ifval($2,$2)
else
  AC_MSG_RESULT([(testing)])
  OLDLIBS="$LIBS"
  AC_CHECK_LIB(pcap, pcap_open_offline)
  if test "$ac_cv_lib_pcap_pcap_open_offline" = "yes" ; then
     PCAP_LIBS="-lpcap"
     AC_MSG_CHECKING([libpcap])
     AC_MSG_RESULT([$PCAP_LIBS])
     m4_ifval($1,$1)
  else
     OLDLDFLAGS="$LDFLAGS" ; LDFLAGS="$LDFLAGS -L$with_libpcap/lib"
     OLDCPPFLAGS="$CPPFLAGS" ; CPPFLAGS="$CPPFLAGS -I$with_libpcap/include"
     AC_CHECK_LIB(pcap, pcap_open_offline)
     CPPFLAGS="$OLDCPPFLAGS"
     LDFLAGS="$OLDLDFLAGS"
     if test "$ac_cv_lib_pcap_pcap_open_offline" = "yes" ; then
        AC_MSG_RESULT(.setting PCAP_LIBS -L$with_libpcap/lib -lpcap)
        PCAP_LIBS="-L$with_libpcap/lib -lpcap"
        test -d "$with_libpcap/include" && PCAP_CFLAGS="-I$with_libpcap/include"
        AC_MSG_CHECKING([libpcap])
        AC_MSG_RESULT([$PCAP_LIBS])
        m4_ifval($1,$1)
     else
        AC_PATH_PROG([PKGCONFIG], [pkg-config], [no])
        AC_MSG_CHECKING([libpcap found with $PKGCONFIG])
        if test "$PKGCONFIG" != "no" && `$PKGCONFIG --exists libpcap`
        then
          AC_MSG_RESULT([yes])
          PCAP_LIBS=`$PKGCONFIG --libs libpcap`
          PCAP_CFLAGS=`$PKGCONFIG --cflags libpcap`
          m4_ifval($1,$1)
        else
          AC_MSG_RESULT([no])
          m4_ifval($2,$2)
        fi
     fi
  fi
  LIBS="$OLDLIBS"
fi
AC_SUBST([PCAP_LIBS])
AC_SUBST([PCAP_CFLAGS])
])