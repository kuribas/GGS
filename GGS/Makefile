# igor: blah

%::
	+ cd Server/Central;		make $@
	+ cd Server/Multiplexor;	make $@
	+ cd Service/GameLib;		make $@
	+ cd Service/Amazons;		make $@
	+ cd Service/Ataxx;		make $@
	+ cd Service/Checkers;		make $@
	+ cd Service/Chess;		make $@
	+ cd Service/Go;		make $@
	+ cd Service/Hex;		make $@
	+ cd Service/Othello;		make $@
	+ cd Service/Phutball;		make $@
	+ cd Service/DotsAndBoxes;	make $@
	+ cd Service/Domineering;	make $@
	+ cd Service/Snort;		make $@
	+ cd Service/GDK;		make $@
	+ cd Service/TD;		make $@

help::
	@ echo "make all   -- compile everything"
	@ echo "make bak   -- removes all ~ backup files"
	@ echo "make clean -- removes all mkf obj bin files"
	@ echo "make run   -- removes all log files"
	@ echo "make log   -- removes all but last few log files"
	@ echo "make dir   -- creates mkf obj bin directories"
	@ echo "make zip   -- creates .tar.bz2"
	@ echo "make clone -- creates GGS clone including dbs (clone.tgz)"


zip::	tar-gsa tar-pkg tar-welty
	rm -f *.bz2
	bzip2 -9 *.tar

clone::
	rm -f exclude
	cd ..; find GGS | egrep "/bin/|/obj/|/mkf/|exclude|/bak/|/log/|/pkg/|\.gz|\.tar|\.bz2|latest|/Client/" > GGS/exclude
	cd ..; tar cfzv clone.tgz GGS/ -X GGS/exclude; mv clone.tgz GGS

tar-gsa::
	rm -f exclude
	cd ..; find GGS | egrep "Welty|/lib/|/bin/|/obj/|/mkf/|exclude|/db/|/bak/|/log/|/pkg/|\.gz|\.tar|\.bz2|latest|/Client/" > GGS/exclude
	cd ..; tar -cf GGS.tar GGS/ -X GGS/exclude; mv GGS.tar GGS


tar-pkg::
	cd ..; tar -cf PKG.tar GGS/pkg ; mv PKG.tar GGS

tar-welty::
	cd ..; tar -cf WELTY.tar GGS/Welty/*/Make* GGS/Welty/*/.src GGS/Welty/*/htm ; mv WELTY.tar GGS
