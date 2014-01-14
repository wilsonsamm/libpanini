#!/bin/bash

make

for i in ainu-english english-ainu swahili-english; do
	echo
	rm -rf $i
	./dictionary $i
	mv $i.txt $i.tmp
	cat $i.tmp | sort > $i.txt
	rm $i.tmp
done

for i in ainu english swahili nahuatl quenya czech; do
	echo
	rm -rf $i.txt
	./wordlist $i | sort | uniq > $i.txt
	wordcount=$(cat $i.txt | wc -l)
	echo The $i wordlist has $wordcount words.
done
