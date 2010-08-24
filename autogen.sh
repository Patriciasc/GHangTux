#!/bin/sh -e

test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

echo "Running intltoolize"
intltoolize --force --copy --automake

gtkdocize --copy --docdir docs --flavour no-tmpl || exit 1

autoreconf --force --install --verbose "$srcdir"
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"
