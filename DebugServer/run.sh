#!/bin/bash

TARGET="DebugServer"
JOB="./$TARGET"

if [ -x $TARGET ]; then
        echo "$JOB"
        eval "$JOB"
else
        echo "can't find $TARGET"
fi

exit
