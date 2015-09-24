#!/bin/bash

# If the user forgot to initialise some variables, then we'll tell him off and
# give up.
if [ -z $PANINI ]; then
	echo You need to set some variables up first. The best way to do that is
	echo to type "source setvar.sh" at your prompt.
	exit 1
fi

# If the requested compiler is not found, then stop.
which $CCOMPILER  &> /dev/null
if [ $? != 0 ]; then
	echo ERROR: $CCOMPILER: command not found. Look in setvar.sh to choose
	echo another C compiler.
	exit 1
fi

# If kanjidic and edict and panini are all installed on the computer, then it
# may be possible to use them to learn some extra Japanese.
(cd tools/ekan && ./build.sh)

# Run the Unihan tools to build various CJK script definitions.
(cd tools/unihan && ./build.sh)

# Query the Wiktionary for unknown words.
(cd tools/wiktionary && ./build.sh)

# We need to build the runtime library
echo Building the system in $PANINI
echo Building the runtime library libpanini.a.
make $MAKEOPTS libpanini.a

# Then, build the compiler, which will get used to build language modules later.
echo Building the compiler.
make $MAKEOPTS -C tools/pcomp pcomp

# Some languages depend on others to already have been built. So build those
# first.
make $MAKEOPTS pp
make $MAKEOPTS english
make $MAKEOPTS world

# And then build the rest of languages.
make $MAKEOPTS ainu
make $MAKEOPTS algonquian
make $MAKEOPTS czech
make $MAKEOPTS japanese
make $MAKEOPTS nahuatl
make $MAKEOPTS quenya
make $MAKEOPTS swahili
make $MAKEOPTS kalaallisut
