## Process this file with automake to produce Makefile.in

EXTRA_DIST = COPYING ChangeLog DOWNLOAD NEWS THANKS TODO cole-config.in

MYDIST_SUBDIRS = doc examples utils

dist-hook:
	for i in $(MYDIST_SUBDIRS); do \
	  cp -r $(srcdir)/$$i $(distdir); \
	done

noinst_LIBRARIES = libcole.a 
dist_libcole_a_SOURCES = cole.c internal.c oledecod.c olecod.c support.c internal.h support.h 
nodist_libcole_a_SOURCES = version.c cole.h
#AM_CFLAGS = -Wall -Wcast-align -Wpointer-arith 

