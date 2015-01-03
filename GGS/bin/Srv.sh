#! /bin/tcsh -f

cd $GSAHOME/Server/Central
rm -f log/out
nohup main $* >& log/out &
