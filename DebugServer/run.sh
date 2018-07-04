#!/bin/bash

TARGET="DebugServer"
OPT="--msg"
JOB="./$TARGET $OPT"

if [ -x $TARGET ]; then
        echo "$JOB"
        eval "$JOB"
else
        echo "can't find $TARGET"
fi

exit
