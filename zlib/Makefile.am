AUTOMAKE_OPTIONS = 1.9

EXTRA_DIST = ChangeLog FAQ INDEX README XOONIPS-update.txt
noinst_LIBRARIES = libz.a

libz_a_headers = \
  zconf.h zlib.h zutil.h \
  crc32.h deflate.h gzguts.h inffast.h inffixed.h inflate.h inftrees.h trees.h

libz_a_sources = \
  adler32.c crc32.c deflate.c infback.c inffast.c inflate.c inftrees.c trees.c zutil.c \
  compress.c uncompr.c gzclose.c gzlib.c gzread.c gzwrite.c

libz_a_SOURCES = $(libz_a_headers) $(libz_a_sources)
