#!/bin/bash

REPO="$PANINI/tools/wals/repo/"

for lang in ain cze prh sno wel grw
do
	echo -n "Interrogating the World Atlas of Languages Structures for data on "
	./parselang language.csv Name $lang

	fn=$lang-autogen.panini
	rm -rf $fn

	./adpositions.sh $lang >> $fn
	./pronouns.sh $lang >> $fn
	./case.sh $lang >> $fn
	./auxp.sh $lang >> $fn
done
