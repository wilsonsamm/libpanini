#!/bin/bash

function getfeature {
	RETVAL="$(./parselang language.csv $1 $2)"
	if [ "$RETVAL" == "" ] ; then
		RETVAL="$(./parselang supplement.csv $1 $2)"
	fi
	echo $RETVAL
}

# 98A Alignment of Case Marking of Nouns
F98A="$(getfeature 99A $1)"

# 99A Alignment of Case Marking of Pronouns
F99A="$(getfeature 99A $1)"

echo
echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;"
echo ";;;; Case. "
echo "; 98A Alignment of Case Marking of Nouns: " $F98A
echo "; 99A Alignment of Case Marking of Pronouns: " $F99A

case "$F98A" in
	"1 Neutral")
		echo "(df argS (into (theta s) call pronoun))"
		echo "(df argA (into (theta a) call pronoun))"
		echo "(df argO (into (theta o) call pronoun))"
		;;
	
	"2 Nominative - accusative (standard)")
		echo "(df argS (into (theta s) call pronoun nominative))"
		echo "(df argA (into (theta a) call pronoun nominative))"
		echo "(df argO (into (theta o) call pronoun accusative))"
		;;
		
	"3 Nominative - accusative (marked nominative)")
		echo "(df argS (into (theta s) call pronoun nominative))"
		echo "(df argA (into (theta a) call pronoun nominative))"
		echo "(df argO (into (theta o) call pronoun accusative))"
		;;
	
	"4 Ergative - absolutive")
		echo "(df argS (into (theta s) call pronoun ergative))"
		echo "(df argA (into (theta a) call pronoun absolutive))"
		echo "(df argO (into (theta o) call pronoun absolutive))"
		;;

	"5 Tripartite")
		echo "(df argS (into (theta s) call pronoun ergative))"
		echo "(df argA (into (theta a) call pronoun absolutive))"
		echo "(df argO (into (theta o) call pronoun accusative))"
		;;

	*)
		echo "; Problematic value for feature 99A: $F99A"
esac

case "$F99A" in
	"1 Neutral")
		echo "(df argS (into (theta s) call dp))"
		echo "(df argA (into (theta a) call dp))"
		echo "(df argO (into (theta o) call dp))"
		;;
	
	"2 Nominative - accusative (standard)")
		echo "(df argS (into (theta s) call dp nominative))"
		echo "(df argA (into (theta a) call dp nominative))"
		echo "(df argO (into (theta o) call dp accusative))"
		;;
	
	"3 Nominative - accusative (marked nominative)")
		echo "(df argS (into (theta s) call dp nominative))"
		echo "(df argA (into (theta a) call dp nominative))"
		echo "(df argO (into (theta o) call dp accusative))"
		;;
	
	"4 Ergative - absolutive")
		echo "(df argS (into (theta s) call dp ergative))"
		echo "(df argA (into (theta a) call dp absolutive))"
		echo "(df argO (into (theta o) call dp absolutive))"
		;;

	"5 Tripartite")
		echo "(df argS (into (theta s) call dp ergative))"
		echo "(df argA (into (theta a) call dp absolutive))"
		echo "(df argO (into (theta o) call dp accusative))"
		;;

	*)
		echo "; Problematic value for feature 99A: $F99A"
esac


for casus in alienable-poss inalienable-poss illative inessive elative allative\
			 essive
do
	mark="$(./parselang cases.csv $casus $1)"
	case $mark in
		"adp")
			echo "(df adjunct (call adpositional-phrase $casus))"
			;;
		"vp")
			echo "(df adjunct (call verb-phrase $case))"
			;;
		"") ;;
		*)
			echo "(df adjunct (into $casus call $mark))"
			;;
	esac
done
