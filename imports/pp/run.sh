#!/bin/bash

rm -rf $1-pp-out
touch $1/pp-out
./pp $1
cat $1-pp-out | sort | uniq > $1/pp-out
rm $1-pp-out
