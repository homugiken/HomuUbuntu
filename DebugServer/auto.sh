#!/bin/bash

JOB="./make.sh"
echo "$JOB"
eval "$JOB"
sleep 1

JOB="./run.sh"
echo "$JOB"
eval "$JOB"

exit
