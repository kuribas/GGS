#!/bin/sh

f=`find . -name '*.C' -or -name '*.H' -or -name '*.sh' -or -name '*.c' -or -name '*.h' -or -name 'Makefile' -or -name 'makefile'`

f="$f txt/* config/* misc/* doxygen/* README"

echo $f | xargs svn propset svn:eol-style native
echo $f | xargs svn propset svn:keywords Id
