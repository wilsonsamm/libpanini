#!/bin/bash

function SubjectPlacementHigh {
	echo "(df main (call s))"
	echo "(df s    (call ArgS) (call auxp))"
	echo "(df s    (call ArgA) (call auxp))"
	echo "(df auxp (call aux finite) (call auxp infinitive))"
	echo "(df auxp (call aux finite) (call auxp infinitive))"
}

function SubjectPlacementLow {
	echo "(df main (call auxp))"
	echo "(df auxp (call verb finite) (call s))"
	echo "(df s    (call ArgS) (call vp))"
	echo "(df s    (call ArgA) (call vp))"
}

function VPHeadFinal {
	echo "(df vp (call ArgO) (call verb transitive))"
	echo "(df vp (call verb intransitive))"
}

function VPHeadInitial {
	echo "(df vp (call verb transitive) (call ArgO))"
	echo "(df vp (call verb intransitive))"
}

echo
echo ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;"
echo ";;;; Auxiliary phrase."

F81A=$(./parselang language.csv 81A $1)
echo "; 81A: Order of subject, object and verb: " $F81A


if [ "$F81A" == "1 SOV" ]
then
	SubjectPlacementHigh
	VPHeadFinal
fi

if [ "$F81A" == "2 SVO" ]
then
	SubjectPlacementHigh
	VPHeadInitial
fi

if [ "$F81A" == "3 VSO" ]
then
	SubjectPlacementLow
	VPHeadInitial
fi

