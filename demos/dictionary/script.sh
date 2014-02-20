#!/bin/bash

make

rm -rf dictionary*.txt wordlist*.txt learned.out

LANGS="ainu english nahuatl czech quenya swahili"

echo GENERATING THE BILINGUAL DICTIONARIES
for i in $LANGS; do
	for j in $LANGS; do
		if [ "$i" != "$j" ]; then
			FN="dictionary-"
			FN+="$i"
			FN+="-"
			FN+="$j"
			FN+=".txt"
			./dictionary bilingual $i $j | sort > dictionary-$i-$j.txt
		fi
	done
done

echo GENERATING WORDLISTS
for i in $LANGS; do
	./dictionary wordlist $i > wordlist-$i
done

echo LEARNING WORDS
for i in $LANGS; do
	for j in $LANGS; do
		if [ "$i" != "$j" ]; then
			./dictionary learn $i $j
			touch learned.out
			sudo cat /usr/tranny/learned/$j learned.out | uniq >> tmp
			sudo mv tmp /usr/tranny/learned/$j
			rm learned.out
		fi
	done
done
