dnl Zusaetzliche Macros


#######################################################
# AX_FPOS_ISSTRUCT
# Prüft, ob fpos_t ein scalar oder eine structure ist
#######################################################
AC_DEFUN([AX_FPOS_ISSTRUCT],[
AC_MSG_CHECKING(if fpos_t is struct or scalar)
AC_LANG_PUSH(C++)


AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <stdio.h>
int main(void){
fpos_t f;
f.__pos=1;
}
]]) ],
AC_DEFINE(FPOS_T_STRUCT,1,)
AC_MSG_RESULT(struct),
AC_MSG_RESULT(scalar)

)

dnl #AC_DEFINE(FPOS_T_STRUCT,1,),)

AC_LANG_POP(C++)

])

#######################################################
# AX_TM_HAS_GMTOFF
# Prüft, ob die structure "tm" das Element "tm_gmtoff"
# hat
#######################################################
AC_DEFUN([AX_TM_HAS_GMTOFF],[
AC_MSG_CHECKING(if struct tm hast tm_gmtoff)
AC_LANG_PUSH(C++)

AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <time.h>
int main(void){
struct tm tt;
tt.tm_gmtoff=1;
}
]]) ],
AC_DEFINE(STRUCT_TM_HAS_GMTOFF,1,)
AC_MSG_RESULT(yes),
AC_MSG_RESULT(no)

)
AC_LANG_POP(C++)
])



AC_DEFUN([AC_FUNC_VSNPRINTF],
[AC_CACHE_CHECK(for working vsnprintf,
  ac_cv_func_vsnprintf,
[AC_TRY_RUN(
[#include <stdio.h>
#include <stdarg.h>

int
doit(char * s, ...)
{
  char buffer[32];
  va_list args;
  int r;

  buffer[5] = 'X';

  va_start(args, s);
  r = vsnprintf(buffer, 5, s, args);
  va_end(args);

  /* -1 is pre-C99, 7 is C99. */

  if (r != -1 && r != 7)
    exit(1);

  /* We deliberately do not care if the result is NUL-terminated or
     not, since this is easy to work around like this.  */

  buffer[4] = 0;

  /* Simple sanity check.  */

  if (strcmp(buffer, "1234"))
    exit(1);

  if (buffer[5] != 'X')
    exit(1);

  exit(0);
}

int
main(void)
{
  doit("1234567");
  exit(1);
}], ac_cv_func_vsnprintf=yes, ac_cv_func_vsnprintf=no, ac_cv_func_vsnprintf=no)])
dnl Note that the default is to be pessimistic in the case of cross compilation.
dnl If you know that the target has a sensible vsnprintf(), you can get around this
dnl by setting ac_func_vsnprintf to yes, as described in the Autoconf manual.
if test $ac_cv_func_vsnprintf = yes; then
  AC_DEFINE(HAVE_WORKING_VSNPRINTF, 1,
            [Define if you have a version of the `vsnprintf' function
             that honours the size argument and has a proper return value.])
fi
])# AC_FUNC_VSNPRINTF


