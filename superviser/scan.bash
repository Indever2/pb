#!/bin/bash
RES=`ps w | grep "$1" | grep -v grep | grep -v "sv start" | grep -v "scan.bash"`
PID=`echo $RES | cut -f1 -d " "`
NAME=`echo "$1" | cut -f1 -d " "`
itc $NAME pid $PID
