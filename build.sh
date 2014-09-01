#!/bin/bash

# This script will build the project by:
#    - compiling all object files, 
#    - installing the library libpanini.a,
#    - run the importers to collect data from other projects,
#    - compiling the language files,
#    - installing the language files

LANGUAGES="ainu czech english japanese nahuatl quenya swahili algonquian"

# The first thing to do is to build the library and the compiler.
make libpanini.a
make compiler

# Next, we need to bootstrap each language and install
# (this is needed because some later stages of the build rely on libpanini
# already being installed)
rm -f imports-*
for lang in $LANGUAGES; do
	echo Bootstrapping $lang
	./tc $lang > $lang
done
echo I am going to install libpanini now. I may need your password.
sudo make install

# We need to prepare the ekan import. Ekan is a program that takes the 
# excellent open-source KANJIDIC and EDICT projects and converts them
# to Panini source code.

make -C imports kanjidic.locale
make -C imports edict.locale

rm -f imports-japanese
make -C imports ekan.out
cat imports/ekan.out >> imports-japanese

# Lastly, we need to rebuild all the languages and reinstall
for lang in $LANGUAGES; do
	echo Compiling $lang
	./tc $lang > $lang
done

echo I am going to install libpanini now. I may need your password.
sudo make install
