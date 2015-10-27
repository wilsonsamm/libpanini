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

# Go and download all the resources we can make use of. (These jobs will be
# running in the background
for dir in $(ls resources/)
do
	make -s -C resources/$dir download &
done

# We can build the runtime library while we wait for the downloads to finish.
echo Building the system in $PANINI
echo Building the runtime library libpanini.a.
make $MAKEOPTS libpanini.a

# Wait until all the downloads have finished
echo Waiting for downloads to finish ...
for dir in $(ls resources/)
do
	make -sq -C resources/$dir
	while [ $? != 0 ]
	do
		sleep 1
		make -sq -C resources/$dir download
	done
done

# Go and build all the various tools that help with machine learning,
# compilation, and so on. These are all found in the tools/ directory.
for dir in $(ls tools/)
do
	(cd tools/$dir && ./build.sh)
done

# Lastly, build all of the targets.
for dir in $(ls targets)
do
	(cd targets/$dir && make)
done
