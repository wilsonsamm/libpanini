#!/bin/bash

function printdfpronoun {
	echo
	echo "(df pronoun"
	[ -z "$1" ] || echo "	(rection (case $1))"
	echo "	(open pronoun)"
	echo "	(call guess-syllables)"
	echo "	(add-record-to-df pronoun)"
	[ -z "$1" ] || echo "	(add-flags-to-df pronoun $1)"
	echo "	(add-ns-to-df pronoun seme)"
	echo "	(add-ns-to-df pronoun rection)"
	echo "	(add-tag-to-df pronoun))"
}

echo
echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;"
echo ";;;; Pronouns."

for casus in $(./parselang cases.csv pronoun-cases $1)
do
	if [ $casus == "same" ]
	then
		for casus in $(./parselang cases.csv noun-cases $1)
		do
			printdfpronoun $casus
		done
	else
		printdfpronoun $casus
	fi
done
