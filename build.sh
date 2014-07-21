#!/bin/bash

# The first thing to do is to build the library.
make libpanini.a

# Next, build the compiler.
make compiler

# We need to prepare the Kanji import.
make -C imports panini-kanji
rm -f imports-japanese
cat imports/panini-kanji | sort | uniq >> imports-japanese

# Lastly, we need to build all the languages.
for lang in ainu czech english japanese nahuatl quenya swahili; do
	echo Compiling $lang
	./tc $lang > $lang
done
