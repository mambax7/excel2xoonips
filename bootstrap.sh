#!/bin/sh

AUTOCONF=autoconf
AUTOMAKE=automake
ACLOCAL=aclocal
AUTOHEADER=autoheader

set -x
$ACLOCAL -I macros -I config
$AUTOHEADER -I macros -I config
$AUTOMAKE --foreign --add-missing --copy
$AUTOCONF

if [ -f config.h.in ]; then
  touch config.h.in
fi

if [ -f stamp-h.in ]; then
  touch stamp-h.in
fi

tmp=`find . -name 'autom4te.cache'`
if [ x"$tmp" != x"" ]; then
  rm -rf $tmp;
fi
tmp=`find . -name 'configure.lineno'`
if [ x"$tmp" != x"" ]; then
  rm -f $tmp;
fi
tmp=`find . -name '*~'`
if [ x"$tmp" != x"" ]; then
  rm -f $tmp;
fi

# (cd modules/gpm && sh ./bootstrap.sh)
