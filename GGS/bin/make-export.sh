#! /bin/tcsh -f

if (! -d db ) exit

cd db

set arc = `ls *.game`
if ( $arc == "" ) exit

set cnt = `wc -l $arc | awk '{ print $1; }'`
set svc = $arc:r
set lat = $svc.latest.$cnt
cp $arc ../$lat

cd ..

bzip2 -9 $lat

chmod og=r $lat.bz2
chmod u=rw $lat.bz2

set expdir="/home/mic/GGS/game-archive"

rm -f $expdir/$svc/*latest*
echo "$lat.bz2 -> $expdir/$svc"
mv $lat.bz2 $expdir/$svc/

# sync html page
cd $expdir
rsync -avhe ssh --delete . mic@192.168.1.1:/home/mic/html/mburo/ggs/game-archive/  > /dev/null

