#!/bin/bash

OUTFILE=wt-$1
PARAMS="?action=query&prop=revisions&rvprop=content&format=php"
URL="https://en.wiktionary.org/w/api.php/"$PARAMS"&titles="$1

echo $1

# Download the definition from wiktionary if it doesn't already exist.
if [ ! -f $OUTFILE ];
then
	wget -q -O $OUTFILE $URL
fi

# And then parse the file.
./parse-wt -s $OUTFILE -l $1
