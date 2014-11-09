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

# We need to do the ekan import. Ekan is a program that takes the 
# excellent open-source KANJIDIC and EDICT projects and converts them
# to Panini source code.
if [ ! -f imports-japanese ];
then
	rm -f imports/ekan/ekan*
	make $MAKEOPTS -C imports/ekan ekan.out
	cat imports/ekan/ekan.out > imports-japanese
fi

# Lastly, we need to rebuild all the languages and reinstall
for lang in $LANGUAGES; do
	echo Compiling $lang
	./tc $lang > $lang
done

echo I am going to reinstall libpanini now. I may need your password.
sudo make install
