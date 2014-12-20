#!/bin/bash
for lang in spanish french;
do
	echo I\'m using pp to learn $lang now.
	rm -rf $lang-pp-out
	touch $lang/pp-out
	./pp $lang
	cat $lang-pp-out | sort | uniq > $lang/pp-out
	rm $lang-pp-out
done
