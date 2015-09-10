#!/bin/bash

REPO="$PANINI/tools/wals/repo/"

function includefile {
	echo mkdir -p src/$2/ >> $1
	echo cp $REPO/$2 ./src/$2/$3 >> $1
	echo "echo \"(include $2/$3)\" >> src/autoincludes" >> $1
}

function adpositions {
	# Argument list:
	# $1 - name of the output file
	# $2 - 85A Order of Adposition and Noun Phrase
	# $3 - 48A Person Marking on Adpositions
	
	# Prepositions
	if [ "$2" == "1 Postpositions" ] ; then
		echo >> $1
		echo "#" Postpositions. >> $1
		includefile $1 adpositions postpositions 
		includefile $1 adpositions adpositions
	fi

	# Postpositions
	if [ "$2" == "2 Prepositions" ] ; then
		echo >> $1
		echo "#" Prepositions. >> $1
		includefile $1 adpositions prepositions
		includefile $1 adpositions adpositions
	fi
}


function adjectives {
	# Argument list:
	# $1 - name of the output file
	# $2 - 118A Predicative Adjectives
	
	if [ "$2" == "1 Verbal encoding" ] ; then
		echo >> $1
		echo "#" This language apparently has no adjectives. >> $1
	fi
	
	if [ "$2" == "2 Nonverbal encoding" ] ; then
		echo >> $1
		echo "#" Adjectives. >> $1
		includefile $1 adjectives/adjectives adjectives/
	fi
}

function initialisescript {
	echo "#!/bin/bash" > $1
	echo "rm -rf src/autoincludes" >> $1
}

for lang in ain cze prh sno wel
do
	echo -n "Interrogating the World Atlas of Languages Structures for data on "
	./parselang language.csv Name $lang

	# 48A Person Marking on Adpositions
	F48A="$(./parselang language.csv 48A $lang)"

	# 52A Comitatives and Instrumentals
	F52A="$(./parselang language.csv 52A $lang)"
	
	# 85A Order of Adposition and Noun Phrase
	F85A="$(./parselang language.csv 85A $lang)"

	# 118A Predicative Adjectives
	F118A="$(./parselang language.csv 118A $lang)"
	
	initialisescript $lang
	adpositions $lang "$F85A" "$F48A"
	adjectives $lang "$F118A"
done
