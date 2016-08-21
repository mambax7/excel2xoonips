#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT(excel2xoonips, 2.0.0, okumura@brain.riken.jp)
AC_REVISION($Id: configure.ac,v 1.2 2016/08/18 05:25:57 orrisroot Exp $)

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([config])
AC_PREFIX_DEFAULT([/usr/local/xoonips])

AC_GNU_SOURCE
AC_ISC_POSIX
AC_CYGWIN

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB(m, sin)

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_CHECK_SIZEOF(unsigned char)
AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(unsigned long long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(long double)

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID

# append -Wall flag.
if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall"
fi

# check pthread library
ACX_PTHREAD
# Some platforms use these, so just defineed them.  They can't hurt if they
# are not supported.
PTHREAD_CFLAGS="$PTHREAD_CFLAGS -D_REENTRANT -D_THREAD_SAFE -D_POSIX_PTHREAD_SEMANTICS"
# At this point, we don't want to muck with the compiler name for threading.
# Let's see who fails, perhaps AIX.  2004-04-23
if test "$PTHREAD_CC" != "$CC"; then
AC_MSG_ERROR([
satellite4 does not support platforms that require a special
compiler binary for thread-safety.
])
fi

# for libsl4
AC_BUILD_LIBSL4

# for cole
dnl     4a) Increment when remove or change interfaces.
dnl     [USER SHOULD UPDATE]
COLE_MAJOR=2
dnl     4a) 5) Increment when add interfaces.
dnl     6) Set to zero when remove (or change) interfaces.
dnl     [PROGRAMMERS SHOULD LOOK WHAT'S NEW]
COLE_MINOR=0
dnl     3) Increment when interfaces not changed at all,
dnl               only bug fixes or internal changes.
dnl     4b) Set to zero when add, remove or change interfaces.
dnl     [ONLY MAINTAINER SHOULD CARE]
COLE_MICRO=2
dnl
dnl     Set this too (I don't know how to do it automatically =) ):
COLE_MAJOR_PLUS_COLE_MINOR=2
dnl
AC_SUBST(COLE_MAJOR)
AC_SUBST(COLE_MINOR)
AC_SUBST(COLE_MICRO)

# for cexcel
CEXCEL_ICONV_LINK

# for oniguruma
AC_BUILD_ONIGURUMA

# for zlib
ZLIB_CFLAGS="-I\$(top_srcdir)/zlib"
ZLIB_LIBS="\$(top_builddir)/zlib/libz.a"
AC_SUBST([ZLIB_CFLAGS])
AC_SUBST([ZLIB_LIBS])

# for excel2xoonips
E2X_CONFIG_PATH=$sysconfdir
AC_SUBST([E2X_CONFIG_PATH])
AC_ARG_WITH(filename_coding_system,
            [  --with-filename-coding-system=encoding  set file name coding system],
            filename_coding_system="$withval", filename_coding_system="")
if test "x$filename_coding_system" = "x"; then
  case $host_os in
  mingw* | cygwin*)
    # set Japanese
    filename_coding_system="CP932"
    ;;
  darwin*)
    filename_coding_system="UTF-8-MAC"
    ;;
  *)
    filename_coding_system="UTF-8"
    ;;
  esac
fi
E2X_FILENAME_CODING_SYSTEM=$filename_coding_system
AC_SUBST([E2X_FILENAME_CODING_SYSTEM])

AC_CONFIG_FILES([Makefile etc/Makefile doc/Makefile sample/Makefile sample/data/Makefile cole/Makefile cole/version.c cole/cole.h libsl4/Makefile cexcel/Makefile oniguruma/Makefile zlib/Makefile excel2xoonips/Makefile])
AC_OUTPUT
