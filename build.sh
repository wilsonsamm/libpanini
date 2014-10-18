#!/bin/bash

# This script will build the project by:
#    - compiling all object files, 
#    - installing the library libpanini.a,
#    - run the importers to collect data from other projects,
#    - compiling the language files,
#    - installing the language files

LANGUAGES="ainu czech english japanese nahuatl quenya swahili algonquian"

MAKEOPTS="--no-print-directory -j4"
# The first thing to do is to build the library and the compiler.
make $MAKEOPTS libpanini.a compiler

# Next, we need to bootstrap each language and install
# (this is needed because some later stages of the build rely on libpanini
# already being installed)
for lang in $LANGUAGES; do
	echo Bootstrapping $lang
	./tc $lang > $lang
done
echo I am going to install libpanini now. I may need your password.
sudo make install

# We need to prepare the ekan import. Ekan is a program that takes the 
# excellent open-source KANJIDIC and EDICT projects and converts them
# to Panini source code.
if [ ! -f imports-japanese ];
then
	if [ -f  /usr/share/edict/kanjidic ];
	then
		cp /usr/share/edict/kanjidic ./
	else
		echo It seems that kanjidic is not installed on your system. Well,
		echo I will just download it then.
		wget http://ftp.monash.edu.au/pub/nihongo/kanjidic.gz
		rm kanjidic
		gunzip kanjidic.gz
	fi
	if [ -f  /usr/share/edict/edict ];
	then
		cp /usr/share/edict/edict ./
	else
		echo It seems that edict is not installed on your system. Well,
		echo I will just download it then.
		wget http://ftp.monash.edu.au/pub/nihongo/edict.gz
		rm edict
		gunzip edict.gz
	fi
	if [ ! -f imports/kanjidic.txt ]; then
		cat kanjidic | iconv --from EUC-JISX0213 > imports/kanjidic.txt
	fi
	if [ ! -f imports/edict.txt ]; then
		cat edict | iconv --from EUC-JISX0213 > imports/edict.txt
	fi

	rm -f imports-japanese
	rm -f imports/ekan*
	make $MAKEOPTS -C imports ekan.out
	cat imports/ekan.out >> imports-japanese
fi

# Lastly, we need to rebuild all the languages and reinstall
for lang in $LANGUAGES; do
	echo Compiling $lang
	./tc $lang > $lang
done

echo I am going to install libpanini now. I may need your password.
sudo make install
