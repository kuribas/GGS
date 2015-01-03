#!/bin/bash -f

# generates link generation statements from link

f=`ls -l $1`
l=`echo $f | gawk '{ print $11; }'`
echo '(' cd `dirname $1` ';' ln -s $l `basename $1` ')'

 