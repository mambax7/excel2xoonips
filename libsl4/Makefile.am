AUTOMAKE_OPTIONS = 1.9

noinst_LIBRARIES = libsl4.a

libsl4_a_sources = \
  libsl4.h \
  libsl4.c \
  array.c \
  env.c \
  error.c \
  file.c \
  hash.c \
  list.c \
  mutex.c \
  string.c \
  time.c \
  user.c \
  win32.c

libsl4_a_SOURCES = $(libsl4_a_sources)
libsl4_a_CFLAGS = $(PTHREAD_CFLAGS) -DLIBSL4_STATICLIB
