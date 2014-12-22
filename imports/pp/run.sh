#!/bin/bash

echo I\'m using pp to learn $1 now.
rm -rf $1-pp-out
touch $1/pp-out
./pp $1
cat $1-pp-out | sort | uniq > $1/pp-out
rm $1-pp-out
