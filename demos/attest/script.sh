#!/bin/bash

# Make the program.
make

# For each language, load the text and attest any tentative rules that actually turn out correct.
# (These rules are copied to a file in /usr/tranny/attest
for lang in english ainu; do
	./attest $lang -- $(cat $lang) > $lang.tmp
	cat /usr/tranny/attested/$lang >> $lang.tmp
	cat $lang.tmp | uniq > $lang.out
	rm $lang.tmp
	cat $lang.out > /usr/tranny/attested/$lang
	rm $lang.out
done
