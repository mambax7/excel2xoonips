AUTOMAKE_OPTIONS = 1.9

EXTRA_DIST = TODO

noinst_LIBRARIES = libcexcel.a

COLE_CFLAGS = -I$(top_srcdir)/cole -I$(top_builddir)/cole

libcexcel_a_sources = \
  cexcel.h \
  cexcel_internal.h \
  hash.c \
  iconv.c \
  file.c \
  stream.c \
  cell.c \
  sheet.c \
  book.c \
  record.c \
  xf.c \
  format.c \
  context.c \
  ctx_format.c \
  ctx_sst.c \
  ctx_xf.c \
  ctx_sheet.c \
  biff_blank.c \
  biff_bof.c \
  biff_boolerr.c \
  biff_boundsheet.c \
  biff_codepage.c \
  biff_eof.c \
  biff_filepass.c \
  biff_format.c \
  biff_integer.c \
  biff_label.c \
  biff_labelsst.c \
  biff_mulblank.c \
  biff_mulrk.c \
  biff_number.c \
  biff_rk.c \
  biff_rstring.c \
  biff_sst.c \
  biff_xf.c \
  cexcel.c

libcexcel_a_SOURCES = $(libcexcel_a_sources)
libcexcel_a_CFLAGS = $(COLE_CFLAGS) $(LIBSL4_CFLAGS)
