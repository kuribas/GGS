#! /bin/tcsh -f

if ( "$1" == "-h" || "$1" == "--help" ) then
  echo $0" <remote=/mnt/sam.hdc1/GGS> [-v] .."
  exit
else if ( "$1" == "" ) then
  set remote = /mnt/sam.hdc1/GGS
else
  set remote = $1; shift
endif

set FLAGS = "-v -c -a -r -z --delete $*"

foreach a ( config Client bin txt misc ) # pkg config )
    echo $a
    rsync -e "ssh -p 31415" $FLAGS $remote/$a/.	$a/.
end

foreach a ( Server/* Service/* )
  foreach b ( .src msg )
    echo $a/$b
    rsync -e "ssh -p 31415" $FLAGS $remote/$a/$b/. $a/$b/. 
  end
  rsync -e "ssh -p 31415" $FLAGS $remote/$a/Makefile $a/.
end

rsync -e "ssh -p 31415" $FLAGS $remote/Welty/GDK/htm/.  	Welty/GDK/htm/.
rsync -e "ssh -p 31415" $FLAGS $remote/Welty/GDK/.src/. 	Welty/GDK/.src/.
rsync -e "ssh -p 31415" $FLAGS $remote/Welty/GDK/Makefile 	Welty/GDK/.
rsync -e "ssh -p 31415" $FLAGS $remote/Welty/GDK/Makefile.Bin 	Welty/GDK/.
rsync -e "ssh -p 31415" $FLAGS $remote/Welty/GDK/Makefile.Lib 	Welty/GDK/.
rsync -e "ssh -p 31415" $FLAGS $remote/Welty/GDK/Doxyfile     	Welty/GDK/.

rsync -e "ssh -p 31415" $FLAGS $remote/Welty/TD/txt/.   	Welty/TD/txt/.
rsync -e "ssh -p 31415" $FLAGS $remote/Welty/TD/.src/.  	Welty/TD/.src/.
