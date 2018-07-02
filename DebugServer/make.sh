#!/bin/bash

VERSION="Debug"
TARGET="DebugServer"
JOBS=4

echo "make $VERSION"
cd $VERSION

JOB="make clean"
echo "$JOB"
eval "$JOB"

JOB="make --jobs=$JOBS"
echo "$JOB"
eval "$JOB"

if [ -x $TARGET ]; then
        echo "make $TARGET@$VERSION success"
        cp -v $TARGET ../
        ls -lh ../$TARGET
else
        echo "make $TARGET@$VERSION failed!"
fi

exit
