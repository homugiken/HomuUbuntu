#!/bin/bash

echo 100 > /proc/sys/fs/mqueue/msg_max

TARGET="DebugServer"
OPT="-v=3 -m -t"
JOB="./$TARGET $OPT"

if [ -x $TARGET ]; then
        echo "$JOB"
        eval "$JOB"
else
        echo "can't find $TARGET"
fi

exit
