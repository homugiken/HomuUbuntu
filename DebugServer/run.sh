#!/bin/bash

TARGET="DebugServer"
OPT="--verbose=2 --count=100 --msg"
JOB="./$TARGET $OPT"

if [ -x $TARGET ]; then
        echo "$JOB"
        eval "$JOB"
else
        echo "can't find $TARGET"
fi

exit
