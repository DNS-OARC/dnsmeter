dnl AC_FIND_FUNC_NO_LIBS(func, libraries, includes, arguments)
AC_DEFUN([AC_FIND_FUNC_NO_LIBS], [

AC_MSG_CHECKING([for $1])
AC_CACHE_VAL(ac_cv_funclib_$1,
[
if eval "test \"\$ac_cv_func_$1\" != yes" ; then
	ac_save_LIBS="$LIBS"
	for ac_lib in "" $2; do
		if test -n "$ac_lib"; then 
			ac_lib="-l$ac_lib"
			LIBS="$ac_lib $ac_save_LIBS"
		fi
		AC_TRY_LINK([$3],[$1($4)],eval "if test -n \"$ac_lib\";then ac_cv_funclib_$1=$ac_lib; else ac_cv_funclib_$1=yes; fi";break)
	done
	eval "ac_cv_funclib_$1=\${ac_cv_funclib_$1-no}"
	LIBS="$ac_save_LIBS"
fi
])

eval "ac_res=\$ac_cv_funclib_$1"

# autoheader tricks *sigh*
: << END
@@@funcs="$funcs $1"@@@
@@@libs="$libs $2"@@@
END

changequote(, )dnl
eval "ac_tr_func=HAVE_`echo $1 | tr '[a-z]' '[A-Z]'`"
eval "ac_tr_lib=HAVE_LIB`echo $ac_res | sed -e 's/-l//' | tr '[a-z]' '[A-Z]'`"
eval "LIB_$1=$ac_res"
changequote([, ])dnl

case "$ac_res" in
	yes)
	eval "ac_cv_func_$1=yes"
	eval "LIB_$1="
	AC_DEFINE_UNQUOTED($ac_tr_func)
	AC_MSG_RESULT([yes])
	;;
	no)
	eval "ac_cv_func_$1=no"
	eval "LIB_$1="
	AC_MSG_RESULT([no])
	;;
	*)
	eval "ac_cv_func_$1=yes"
	eval "ac_cv_lib_`echo "$ac_res" | sed 's/-l//'`=yes"
	AC_DEFINE_UNQUOTED($ac_tr_func)
	AC_DEFINE_UNQUOTED($ac_tr_lib)
	AC_MSG_RESULT([yes, in $ac_res])
	;;
esac
AC_SUBST(LIB_$1)
])

dnl AC_FIND_FUNC(func, libraries, includes, arguments)
AC_DEFUN([AC_FIND_FUNC], [
AC_FIND_FUNC_NO_LIBS([$1], [$2], [$3], [$4])
if test -n "$LIB_$1"; then
	LIBS="$LIB_$1 $LIBS"
fi
])
