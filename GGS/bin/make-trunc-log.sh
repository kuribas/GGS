#! /bin/tcsh -f

if (! -d log ) exit
if (! -d bak ) exit

set OLD = `/bin/ls -r log/*log* | tail -n 2`

if ( "$OLD" == "" ) exit

mkdir -p bak/log

foreach a ( $OLD )
  cp $a bak/log/
  rm -rf $a
end

