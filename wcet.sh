#!/bin/bash
gcc -fprofile-arcs -ftest-coverage -o wcet wcet.c && ./wcet -a $1
gcov -a -b -c wcet.c
FILE="wcet.c.gcov"
echo "*** File - $FILE contents ***"
cat $FILE
