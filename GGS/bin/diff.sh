#! /bin/tcsh -f

if ( "$1" == "-h" || "$1" == "--help" ) then
  echo $0" <remote=/rhdr/igord/GGS>"
  exit
else if ( "$1" == "" ) then
  set remote = "/rhdr/igord/GGS"
else
  set remote = $1
endif

set FLAGS = "-r --brief"

diff $FLAGS $remote/bin/.    bin/. 
diff $FLAGS $remote/txt/.    txt/. 
diff $FLAGS $remote/pkg/.    pkg/. 
diff $FLAGS $remote/misc/.   misc/. 
diff $FLAGS $remote/config/. config/. 

foreach a ( Server/* Service/* )
  foreach b ( .src msg )
    diff $FLAGS $remote/$a/$b/. $a/$b/. 
  end
end

foreach a ( Client/* )
  diff $FLAGS $remote/$a/. $a/.
end
