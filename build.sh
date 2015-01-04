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
make $MAKEOPTS libpanini.a

# Build the compiler and other tools
make $MAKEOPTS -C tools/pcomp pcomp
make $MAKEOPTS -C tools/pp    pp
make $MAKEOPTS -C tools/ekan  ekan

# And then build the languages.
make $MAKEOPTS ainu
make $MAKEOPTS algonquian
make $MAKEOPTS czech
make $MAKEOPTS english
make $MAKEOPTS japanese
make $MAKEOPTS nahuatl
make $MAKEOPTS pp
make $MAKEOPTS quenya
make $MAKEOPTS swahili
