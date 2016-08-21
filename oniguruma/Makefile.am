## Makefile.am for Oniguruma
AUTOMAKE_OPTIONS = 1.9

encdir    = $(top_srcdir)/oniguruma/enc
libname = libonig.a

AM_CPPFLAGS = -DNOT_RUBY

noinst_HEADERS = oniguruma.h oniggnu.h onigposix.h
noinst_LIBRARIES = $(libname)

libonig_a_SOURCES = regint.h regparse.h regenc.h st.h \
	regerror.c regparse.c regext.c regcomp.c regexec.c reggnu.c \
	regenc.c regsyntax.c regtrav.c regversion.c st.c \
	regposix.c regposerr.c \
	$(encdir)/unicode.c $(encdir)/ascii.c $(encdir)/utf8.c \
	$(encdir)/utf16_be.c $(encdir)/utf16_le.c \
	$(encdir)/utf32_be.c $(encdir)/utf32_le.c \
	$(encdir)/euc_jp.c $(encdir)/sjis.c $(encdir)/iso8859_1.c \
	$(encdir)/iso8859_2.c  $(encdir)/iso8859_3.c \
	$(encdir)/iso8859_4.c  $(encdir)/iso8859_5.c \
	$(encdir)/iso8859_6.c  $(encdir)/iso8859_7.c \
	$(encdir)/iso8859_8.c  $(encdir)/iso8859_9.c \
	$(encdir)/iso8859_10.c $(encdir)/iso8859_11.c \
	$(encdir)/iso8859_13.c $(encdir)/iso8859_14.c \
	$(encdir)/iso8859_15.c $(encdir)/iso8859_16.c \
	$(encdir)/euc_tw.c $(encdir)/euc_kr.c $(encdir)/big5.c \
	$(encdir)/gb18030.c $(encdir)/koi8_r.c $(encdir)/cp1251.c

EXTRA_DIST = HISTORY README.ja index.html index_ja.html \
	doc/API doc/API.ja doc/RE doc/RE.ja doc/FAQ doc/FAQ.ja \
	$(encdir)/koi8.c $(encdir)/mktable.c \
	XOONIPS-update.txt

dist-hook:
	cp -r $(srcdir)/orig $(distdir)
	rm -rf `find $(distdir)/orig -name 'CVS'`

## END OF FILE
