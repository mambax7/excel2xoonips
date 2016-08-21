AUTOMAKE_OPTIONS = 1.9

COLE_CFLAGS = -I$(top_srcdir)/cole -I$(top_builddir)/cole
COLE_LIBS = $(top_builddir)/cole/libcole.a

CEXCEL_CFLAGS = -I$(top_srcdir)/cexcel
CEXCEL_LIBS = $(top_builddir)/cexcel/libcexcel.a $(LIBICONV)

E2X_CFLAGS = \
  -DE2X_CONFIG_PATH="\"$(E2X_CONFIG_PATH)\"" \
  -DE2X_FILENAME_CODING_SYSTEM="\"$(E2X_FILENAME_CODING_SYSTEM)\""

bin_PROGRAMS = excel2xoonips

excel2xoonips_sources = \
  minizip/crypt.h \
  minizip/ioapi.h \
  minizip/ioapi.c \
  minizip/zip.h \
  minizip/zip.c \
  win32_dirent.h \
  win32_dirent.c \
  excel2xoonips.h \
  error.c \
  recursive.c \
  directory.c \
  strarray.c \
  onig.c \
  inifile.c \
  minizip.c \
  bsd_mktemp.c \
  tempdir.c \
  utility.c \
  tmpl_bool.c \
  tmpl_integer.c \
  tmpl_string.c \
  tmpl_select.c \
  tmpl_array.c \
  tmpl_dataset.c \
  tmpl_file.c \
  tmpl_rights.c \
  tmpl_indexes.c \
  template.c \
  main.c

excel2xoonips_SOURCES = $(excel2xoonips_sources)
excel2xoonips_CFLAGS = $(E2X_CFLAGS) $(COLE_CFLAGS) -DLIBSL4_STATICLIB $(LIBSL4_CFLAGS) $(CEXCEL_CFLAGS) $(ONIGURUMA_CFLAGS) $(ZLIB_CFLAGS)
excel2xoonips_LDADD = $(CEXCEL_LIBS) $(LIBSL4_LIBS) $(COLE_LIBS) $(ONIGURUMA_LIBS) $(ZLIB_LIBS)
