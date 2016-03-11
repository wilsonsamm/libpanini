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

if [ "$1" == "push" ]; then
	./build.sh deepclean
#	./build.sh forfree
	./build.sh clean
	git commit -a && git push
	exit 0
fi

if [ "$1" == "all" ]; then
	./build.sh libpanini
	./build.sh resources
	./build.sh tools
	./build.sh targets
	exit 0
fi

if [ "$1" == "quick" ]; then
	./build.sh libpanini
	./build.sh resources
	./build.sh tools
	./build.sh targets
	exit 0
fi

if [ "$1" == "clean" ]; then
	for dir in resources targets tools demos forfree
	do
		for subdir in $(ls $dir/)
		do
			make -C $dir/$subdir clean
		done
	done
	make clean
	exit 0
fi

if [ "$1" == "deepclean" ]; then
	for dir in resources targets tools demos
	do
		for subdir in $(ls $dir/)
		do
			make -s -C $dir/$subdir deepclean
		done
	done
	exit 0
fi

if [ "$1" == "forfree" ]; then
# To rebuild all the stuff we get for free
# (for now, just a couple of dictionaries, more will be along later)
	for dir in $(ls forfree/)
	do
		echo Building $dir
		(cd forfree/$dir && ./build.sh > /dev/null)
	done
	exit 0
fi

if [ "$1" == "resources" ]; then
# Go and download all the resources we can make use of. (These jobs will be
# running in the background
	for dir in $(ls resources/)
	do
		echo Downloading $dir
		(make -s -C resources/$dir download) &
	done

# Wait until all the downloads have finished
	echo -n "Waiting for downloads to finish: "
	for dir in $(ls resources/)
	do
		echo -n "$dir "
		make -sq -C resources/$dir download
		while [ $? != 0 ]
		do
			sleep 1
			make -sq -C resources/$dir download
		done
	done
	echo
fi

if [ "$1" == "libpanini" ]; then
	echo Building the system in $PANINI
	echo Building the runtime library libpanini.a.
	make $MAKEOPTS libpanini.a
fi

if [ "$1" == "tools" ]; then
# Go and build all the various tools that help with machine learning,
# compilation, and so on. These are all found in the tools/ directory.
	for dir in $(ls tools/)
	do
		echo Building $dir
		(cd tools/$dir && ./build.sh)
	done
fi

if [ "$1" == "targets" ]; then
	# Lastly, build all of the targets.
	for dir in $(ls targets)
	do
		echo Compiling $dir
		(cd targets/$dir && make -s)
	done
fi
