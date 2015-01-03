#! /bin/bash

set LOGNAME mburo
export LOGNAME

at now +1day -f bin/at-job.sh

make trunc-log
make backup
make export
