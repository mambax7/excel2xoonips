# Check build dependencies for oniguruma
# Yoshihiro OKUMURA
#
AC_DEFUN([AC_BUILD_ONIGURUMA],
[dnl

oniguruma_dirname=oniguruma

dnl check for COMBINATION_EXPLOSION
comb_expl_check=no
if test $comb_expl_check = yes; then
  AC_DEFINE(USE_COMBINATION_EXPLOSION_CHECK,1,[Define if combination explosion check])
fi

dnl check for CRNL_AS_LINE_TERMINATOR
crnl_as_line_terminator=yes
if test $crnl_as_line_terminator = yes; then
  AC_DEFINE(USE_CRNL_AS_LINE_TERMINATOR,1,[Define if enable CR+NL as line terminator])
fi

dnl Checks for programs.
AC_PROG_CC

AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h string.h strings.h sys/time.h unistd.h sys/times.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(long, 4)
AC_C_CONST
AC_HEADER_TIME

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP

AC_CACHE_CHECK(for prototypes, _cv_have_prototypes,
  [AC_TRY_COMPILE([int foo(int x) { return 0; }], [return foo(10);],
        _cv_have_prototypes=yes,
        _cv_have_prototypes=no)])
if test "$_cv_have_prototypes" = yes; then
  AC_DEFINE(HAVE_PROTOTYPES,1,[Define if compilerr supports prototypes])
fi

AC_CACHE_CHECK(for variable length prototypes and stdarg.h, _cv_stdarg,
  [AC_TRY_COMPILE([
#include <stdarg.h>
int foo(int x, ...) {
        va_list va;
        va_start(va, x);
        va_arg(va, int);
        va_arg(va, char *);
        va_arg(va, double);
        return 0;
}
], [return foo(10, "", 3.14);],
        _cv_stdarg=yes,
        _cv_stdarg=no)])
if test "$_cv_stdarg" = yes; then
  AC_DEFINE(HAVE_STDARG_PROTOTYPES,1,[Define if compiler supports stdarg prototy
pes])
fi

dnl Define build environments for depending libraries and programs
ONIGURUMA_CFLAGS="-I\$(top_srcdir)/$oniguruma_dirname"
ONIGURUMA_LIBS="\$(top_builddir)/$oniguruma_dirname/libonig.a"
AC_SUBST([ONIGURUMA_CFLAGS])
AC_SUBST([ONIGURUMA_LIBS])

])
