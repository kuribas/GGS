#! /bin/tcsh -f

cd $GSAHOME/Server/Multiplexor
#make run
nohup main $* >& log/out.$1 &
