#! /bin/tcsh 

if ( "$1" == "-h" || "$1" == "--help" ) then
  echo $0" <remote=/mnt/sam.hdc1/GGS> [-v] .."
  exit
else if ( "$1" == "" ) then
  set remote = /mnt/sam.hdc1/GGS
else
  set remote = $1 ; shift
endif

bin/set.mode.sh

set FLAGS = "-v -c -a -r -z --delete $*"

foreach a ( config Client bin txt misc ) # pkg config )
  echo $a
  rsync -e "ssh -p 31415"  $FLAGS $a/.	$remote/$a/.
end

foreach a ( Server/* Service/* )
  foreach b ( .src msg )
    echo $a/$b
    rsync -e "ssh -p 31415" $FLAGS $a/$b/. $remote/$a/$b/.
  end
   rsync -e "ssh -p 31415" $FLAGS $a/Makefile $remote/$a/.
end

rsync -e "ssh -p 31415" $FLAGS Welty/GDK/htm/.  		$remote/Welty/GDK/htm/.
rsync -e "ssh -p 31415" $FLAGS Welty/GDK/.src/. 		$remote/Welty/GDK/.src/.
rsync -e "ssh -p 31415" $FLAGS Welty/GDK/Makefile 	$remote/Welty/GDK/.
rsync -e "ssh -p 31415" $FLAGS Welty/GDK/Makefile.Bin 	$remote/Welty/GDK/.
rsync -e "ssh -p 31415" $FLAGS Welty/GDK/Makefile.Lib 	$remote/Welty/GDK/.
rsync -e "ssh -p 31415" $FLAGS Welty/GDK/Doxyfile     	$remote/Welty/GDK/.
	     				        
rsync -e "ssh -p 31415" $FLAGS Welty/TD/txt/.   		$remote/Welty/TD/txt/.
rsync -e "ssh -p 31415" $FLAGS Welty/TD/.src/.  		$remote/Welty/TD/.src/.
