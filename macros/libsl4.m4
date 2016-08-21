# Check build dependencies for libsl4
# Yoshihiro OKUMURA
#
AC_DEFUN([AC_BUILD_LIBSL4],
[dnl

libsl4_dirname=libsl4

  AC_REQUIRE([ACX_PTHREAD])dnl

dnl Checks for programs.
  AC_PROG_CC
  AC_PROG_RANLIB

dnl Checks for header files.
  AC_HEADER_STDC
  AC_CHECK_HEADERS([direct.h pthread.h pwd.h sys/param.h])

dnl Checks for typedefs, structures, and compiler characteristics.
  AC_C_CONST
  AC_TYPE_MODE_T
  AC_TYPE_OFF_T
  AC_TYPE_PID_T
  AC_TYPE_SIZE_T
  AC_STRUCT_TM
  AC_TYPE_UID_T

dnl Checks for library functions.
  AC_FUNC_LSTAT
  AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
  AC_FUNC_VPRINTF
  AC_CHECK_FUNCS([endpwent getcwd memmove memset mkdir putenv rmdir setenv strstr])
  AC_CHECK_FUNCS([snprintf vasprintf vsnprintf wvsprintf _vscprintf])

dnl Define build environments for depending libraries and programs
AC_DEFINE(LIBSL4_STATICLIB,1,[Define to 1 if libsl4 is static library])
LIBSL4_CFLAGS="-I\$(top_srcdir)/$libsl4_dirname \$(PTHREAD_CFLAGS)"
LIBSL4_LIBS="\$(top_builddir)/$libsl4_dirname/libsl4.a \$(PTHREAD_LIBS)"
AC_SUBST([LIBSL4_CFLAGS])
AC_SUBST([LIBSL4_LIBS])

])
