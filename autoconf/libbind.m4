dnl AM_CHECK_LIBBIND()
AC_DEFUN([AM_CHECK_LIBBIND],[
AC_ARG_WITH([libbind],
	[  --with-libbind[[=PATH]]     Prefix where libbind is installed (optional)],
	[libbind_prefix="$withval"],
	[libbind_prefix=""])

if test "x$libbind_prefix" != "xno"
then

dnl res_query
AC_SEARCH_LIBS(res_query,[resolv bind],have_res_query=yes,
	[dnl Have to include resolv.h as res_query is sometimes defined as a macro
	AC_MSG_CHECKING([for res_query in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[res_query(0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_res_query=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_res_query" = "yes"
then
	AC_DEFINE(HAVE_RES_QUERY,1, [Define if you have res_query function])
fi


dnl res_search
AC_SEARCH_LIBS(res_search,[resolv bind],have_res_search=yes,
	[dnl Have to include resolv.h as res_query is sometimes defined as a macro
	AC_MSG_CHECKING([for res_search in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[res_search(0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_res_search=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_res_search" = "yes"
then
	AC_DEFINE(HAVE_RES_SEARCH,1, [Define if you have res_search function])
fi

dnl res_querydomain
AC_SEARCH_LIBS(res_querydomain,[resolv bind],have_res_querydomain=yes,
	[dnl Have to include resolv.h as res_query is sometimes defined as a macro
	AC_MSG_CHECKING([for res_querydomain in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[res_querydomain(0,0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_res_querydomain=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_res_querydomain" = "yes"
then
	AC_DEFINE(HAVE_RES_QUERYDOMAIN,1, [Define if you have res_querydomain function])
fi

dnl res_mkquery
AC_SEARCH_LIBS(res_mkquery,[resolv bind],have_res_mkquery=yes,
	[dnl Have to include resolv.h as res_query is sometimes defined as a macro
	AC_MSG_CHECKING([for res_mkquery in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[res_mkquery(0,0,0,0,0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_res_mkquery=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_res_mkquery" = "yes"
then
	AC_DEFINE(HAVE_RES_MKQUERY,1, [Define if you have res_mkquery function])
fi

dnl res_send
AC_SEARCH_LIBS(res_send,[resolv bind],have_res_send=yes,
	[dnl Have to include resolv.h as res_query is sometimes defined as a macro
	AC_MSG_CHECKING([for res_send in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[res_send(0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_res_send=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_res_send" = "yes"
then
	AC_DEFINE(HAVE_RES_SEND,1, [Define if you have res_send function])
fi

dnl dn_comp
AC_SEARCH_LIBS(dn_comp,[resolv bind],have_dn_comp=yes,
	[dnl Have to include resolv.h as res_query is sometimes defined as a macro
	AC_MSG_CHECKING([for dn_comp in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[dn_comp(0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_dn_comp=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_dn_comp" = "yes"
then
	AC_DEFINE(HAVE_DN_COMP,1, [Define if you have dn_comp function])
fi

dnl dn_expand
AC_SEARCH_LIBS(dn_expand,[resolv bind],have_dn_expand=yes,
	[dnl Have to include resolv.h as dn_expand is sometimes defined as a macro
	AC_MSG_CHECKING([for dn_expand in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[dn_expand(0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_dn_expand=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_dn_expand" = "yes"
then
	AC_DEFINE(HAVE_DN_EXPAND,1, [Define if you have dn_expand function])
fi

dnl ns_initparse
AC_SEARCH_LIBS(ns_initparse,[resolv bind],have_ns_initparse=yes,
	[dnl Have to include resolv.h as ns_initparse is sometimes defined as a macro
	AC_MSG_CHECKING([for ns_initparse in -lresolv (with resolv.h if present)])
	saved_libs="$LIBS"
	LIBS="-lresolv $LIBS"
	AC_TRY_LINK([
		#ifdef HAVE_RESOLV_H
		#include <resolv.h>
		#endif],
		[ns_initparse(0,0,0,0,0)],
	[AC_MSG_RESULT(yes)
		have_ns_initparse=yes],
	[AC_MSG_RESULT(no)
		LIBS="$saved_libs"])
	]
)

if test "$have_ns_initparse" = "yes"
then
	AC_DEFINE(HAVE_NS_INITPARSE,1, [Define if you have ns_initparse function])
fi

fi
])
