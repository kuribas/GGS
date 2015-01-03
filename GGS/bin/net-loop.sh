#! /bin/bash

if [ "$2" == "" ] ; then
   echo "$0 <host> <port>"
   exit 0
fi

while [ "" == "" ] ; do
telnet $1 $2 <<EOF
EOF

sleep 60
done
