# Configure paths for Athena Widget
# Yoshihiro OKUMURA
#

dnl CEXCEL_ICONV_LINK()
dnl test for iconv library
dnl
AC_DEFUN([CEXCEL_ICONV_LINK],
[dnl
  AM_ICONV

  save_libs="$LIBS"
  LIBS="$LIBS $LIBICONV"

dnl Check error no support
  AC_MSG_CHECKING([if iconv supports errno])
    AC_TRY_RUN([
#include <iconv.h>
#include <errno.h>

int main() {
  iconv_t cd;
  cd = iconv_open( "*blahblah*", "*blahblah*" );
  if (cd == (iconv_t)(-1)) {
    if (errno == EINVAL) {
      return 0;
        } else {
      return 1;
    }
  }
  iconv_close( cd );
  return 2;
}
    ],[
      AC_MSG_RESULT(yes)
    ],[
      AC_MSG_RESULT(no)
      AC_MSG_ERROR([cexcel library required to support error no in iconv()])
    ],[
      AC_MSG_RESULT([skip for cross compiling])
    ])

dnl Check Japanese CP932 charsets support
  AC_MSG_CHECKING([if iconv supports Japanese CP932 charsets])
    AC_TRY_RUN([
#include <iconv.h>
#include <errno.h>

int main() {
  iconv_t cd;
  cd = iconv_open( "CP932", "UTF-8" );
  if (cd == (iconv_t)(-1)) {
    if (errno == EINVAL) {
      return 1;
        } else {
      return 1;
    }
  }
  iconv_close( cd );
  return 0;
}
    ],[
      AC_MSG_RESULT(yes)
    ],[
      AC_MSG_RESULT(no)
      AC_MSG_ERROR([cexcel library required to support Japanese CP932 conversion on iconv()])
    ],[
      AC_MSG_RESULT([skip for cross compiling])
    ])

  LIBS="$save_libs"

])
