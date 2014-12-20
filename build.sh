#!/bin/bash

# This script will build the project by:
#    - compiling all object files, 
#    - installing the library libpanini.a,
#    - run the importers to collect data from other projects,
#    - compiling the language files,
#    - installing the language files

LANGUAGES="ainu czech english japanese nahuatl quenya swahili algonquian"
PPLANGS="spanish french"
MAKEOPTS="--no-print-directory -j4"

# This function installs a language
function install-lang {
	if [ -f $1 ]; then
		#echo Installing $1
		sudo cp $1 /usr/panini/languages/
	fi
}

# The first thing to do is to build the library and the compiler, and to
# put the library where anything else can find it.
make $MAKEOPTS libpanini.a compiler
echo I am going to install libpanini now. I may need your password.
sudo make install

# Next, we need to bootstrap each language and install
# (this is needed because some later stages of the build rely on libpanini
# already being installed)
for lang in $LANGUAGES pp; do
	echo Bootstrapping $lang
	./tc -d languages/$lang -i $lang > $lang
	install-lang $lang
done

# We need to do the ekan import. Ekan is a program that takes the 
# excellent open-source KANJIDIC and EDICT projects and converts them
# to Panini source code.
if [ ! -f imports-japanese ];
then
	rm -f imports/ekan/ekan.out
	make $MAKEOPTS -C imports/ekan ekan.out
	cat imports/ekan/ekan.out > imports-japanese
fi

# We need to do the pp import. Pp is a panini program (so it will 
# already have been installed by now) which has in it the rough rules 
# of the universal grammar (some rules which appear to apply to all 
# natural languages and therefore are theorised to be fundamental to a
# human language faculty.
# For now, all this does is use these to deduce the syntax of some
# simple Spanish.
make $MAKEOPTS -C imports/pp langs
for lang in $PPLANGS; do
	echo Compiling $lang, which was produced by the pp importer.
	./tc -d imports/pp/$lang -i $lang > $lang
	install-lang $lang
done

# Lastly, we need to rebuild all the languages and reinstall
for lang in $LANGUAGES; do
	echo Compiling $lang
	./tc -d languages/$lang -i $lang > $lang
	install-lang $lang
done
