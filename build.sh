#!/bin/bash
if [ -z $PANINI ]; then
	echo You need to set some variables up first. The best way to do that is
	echo to type "source setvar.sh" at your prompt.
	exit 1
fi

# If kanjidic or edict are not installed on the computer and it was requested
# that they be included in the build, you'll get a warning here.
ls /usr/share/edict/kanjidic /usr/share/edict/edict &> /dev/null
if [ $? != 0 ]; then
	echo Warning: Either edict, kanjidic or both are not installed on your puter.
	if [ -n $PANINI_EKAN ];  then
		echo WARNING: It was requested that this build include data from the
		echo edict and kanjidic files, but these files are not both installed.
	fi
	export PANINI_EKAN=""
fi

# If the requested compiler is not found, then stop.
which $CCOMPILER  &> /dev/null
if [ $? != 0 ]; then
	echo ERROR: $CCOMPILER: command not found. Look in setvar.sh to choose
	echo another C compiler.
	exit 1
fi

which $CXXCOMPILER  &> /dev/null
if [ $? != 0 ]; then
	echo ERROR: $CXXCOMPILER: command not found. Look in setvar.sh to choose
	echo another C++ compiler.
fi

echo Building the system in $PANINI

# First, we need to build the runtime library
echo Building the runtime library libpanini.a.
make $MAKEOPTS libpanini.a

# Then, build the compiler and other tools, which get used to build language
# modules later.
echo Building the compiler.
make $MAKEOPTS -C tools/pcomp pcomp
echo Building the machine learning tools.
make $MAKEOPTS -C tools/learn
echo Building the EDICT and KANJIDIC importer.
make $MAKEOPTS -C tools/ekan ekan
echo Building the interface to WordNet.
make $MAKEOPTS -C tools/wn

if [ $PP_SWORD -eq 1 ]; then
	echo Building the SWORD API interface.
	make $MAKEOPTS -C tools/sword
fi

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
