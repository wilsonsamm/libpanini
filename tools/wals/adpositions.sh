#!/bin/bash

function getfeature {
	RETVAL="$(./parselang language.csv $1 $2)"
	if [ "$RETVAL" == "" ] ; then
		RETVAL="$(./parselang supplement.csv $1 $2)"
	fi
	echo $RETVAL
}

# 85A Order of Adposition and Noun Phrase
F85A="$(getfeature 85A $1)"

echo
echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;"
echo ";;;; Adpositions."
echo "; 85A Order of Adposition and Noun Phrase:" $F85A

if [ "$F85A" == "1 Postpositions" ]; then
	DEF="postposition"
elif [ "$F85A" == "2 Prepositions" ]; then
	DEF="preposition"
else
	echo "; Bad value: " $F85A
	exit
fi

echo "(df $DEF"
echo "	(open $DEF)"
echo "	(call guess-syllables)"
echo "	(add-record-to-df $DEF)"
echo "	(add-flags-to-df $DEF $mark)"
echo "	(add-ns-to-df $DEF theta)"
echo "	(add-tag-to-df $DEF))"
