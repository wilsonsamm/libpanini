#!/bin/bash

# Do we have kanjidic installed?
# Then just convert it now to the local character encoding.
if [ -f ./kanjidic.txt ];
then
	echo -n
#	echo Not going to create kanjidic.txt because it already exists.
else 
	if [ -f /usr/share/edict/kanjidic ];
	then
#		echo Yes you have kanjidic installed, so I\'m going to use it.
		cat /usr/share/edict/kanjidic | iconv --from EUC-JISX0213 > kanjidic.txt

# Otherwise, download it and then convert it.
	else
#		echo It seems that kanjidic is not installed on your system. Well,
#		echo I will just download it then.
		wget http://ftp.monash.edu.au/pub/nihongo/kanjidic.gz
		rm -f kanjidic
		gunzip kanjidic.gz
		cat kanjidic | iconv --from EUC-JISX0213 > kanjidic.txt
	fi
fi

# Do we have edict installed?
# Then just convert it now to the local character encoding.
if [ -f ./edict.txt ];
then
	echo -n
#	echo Not going to create edict.txt because it already exists.
else 
	if [ -f /usr/share/edict/edict ];
	then
#		echo Yes you have edict installed, so I\'m going to use it.
		cat /usr/share/edict/edict | iconv --from EUC-JISX0213 > edict.txt

# Otherwise, download it and then convert it.
	else
#		echo It seems that edict is not installed on your system. Well,
#		echo I will just download it then.
		wget http://ftp.monash.edu.au/pub/nihongo/edict.gz
		rm -f edict
		gunzip edict.gz
		cat edict | iconv --from EUC-JISX0213 > edict.txt
	fi
fi
