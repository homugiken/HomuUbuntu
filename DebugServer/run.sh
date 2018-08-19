#!/bin/bash

TARGET="DebugServer"
OPT="-v=3 -p=/root/.dbg -m"
JOB="./$TARGET $OPT"

if [ -x $TARGET ]; then
        echo "$JOB"
        eval "$JOB"
else
        echo "can't find $TARGET"
fi

exit
