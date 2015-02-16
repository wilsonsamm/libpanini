#!/bin/bash

if [[ -z $PANINI ]]; then
	echo You need to set some variables up. The best way to do that is by 
	echo editing ./setvar.sh and then typing
	echo source ./setvar.sh
	echo at your Bash prompt.
	exit
fi

# This script will build the project by:
#    - compiling all object files, 
#    - installing the library libpanini.a,
#    - run the importers to collect data from other projects,
#    - compiling the language files,
#    - installing the language files

echo Building the system in $PANINI

# Build the runtime library
echo Building the runtime library libpanini.a.
make $MAKEOPTS libpanini.a

# Build the compiler and other tools
echo Building the compiler.
make $MAKEOPTS -C tools/pcomp pcomp
echo Building the machine learning tools.
make $MAKEOPTS -C tools/learn
echo Building the EDICT and KANJIDIC importer.
make $MAKEOPTS -C tools/ekan  ekan

# Some languages depend on others to already have been built. So build those
# first.
make $MAKEOPTS pp
make $MAKEOPTS english

# And then build the languages.
make $MAKEOPTS ainu
make $MAKEOPTS algonquian
make $MAKEOPTS czech
make $MAKEOPTS english
make $MAKEOPTS japanese
make $MAKEOPTS nahuatl
make $MAKEOPTS quenya
make $MAKEOPTS spanish
make $MAKEOPTS swahili
make $MAKEOPTS world
