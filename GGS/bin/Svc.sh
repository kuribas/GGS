#! /bin/tcsh -f

set limit coredumpsize unlimited

setenv LD_LIBRARY_PATH ../../lib:$LD_LIBRARY_PATH

echo "LD_LIBRARY_PATH=" $LD_LIBRARY_PATH

foreach a ( Service/* )
  pushd .
  chdir $a
  make clean-log
  if ( -f main ) then
    rm -f log/out
    echo $a
    nohup main -pw $GGSPASSW $* >& log/out &
    sleep 1
  endif
  popd
end
