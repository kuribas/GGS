#! /bin/tcsh -f

if (! -d db ) exit

set BACKUP   = "bak"
set DAY      = `date +"%w"`
set DIR      = "d."`date +"%Y.%m.%d_%H:%M:%S"`
set NO_WEEKS = 4
set NO_DAYS  = 7

set DIRECTORIES = ( msg db )

# end of customization

if (! -d $BACKUP ) then
  echo "$BACKUP not found."
  exit 0
endif

if ( $DAY == 0 ) then
   set LAST = `/bin/ls -td $BACKUP/d.* | head -1`
   if ( "$LAST" != "" ) then
      echo "Trim weekly backups."
      /bin/ls -td $BACKUP/w.* | tail -n +$NO_WEEKS | xargs rm -rf

      echo "New weekly backup (Sunday)."
      mv $LAST $LAST:s/d./w./
   endif
endif

echo "Trim daily backups."
/bin/ls -td $BACKUP/d.* | tail -n +$NO_DAYS | xargs rm -rf

foreach d ( $DIRECTORIES )
  mkdir -p $BACKUP/$DIR/$d
  echo "$d/. -> $BACKUP/$DIR/$d/."
  cp -aRx $d/. $BACKUP/$DIR/$d/.
end

