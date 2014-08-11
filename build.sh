#!/bin/bash

# This script will build the project 
#  ( - compiling all object files, 
#    - installing the library libpanini.a,
#    - run the importers to collect data from other projects,
#    - compiling the language files,
#    - installing the language files)


# The first thing to do is to build the library.
make libpanini.a

# Next, build the compiler.
make compiler

# We need to prepare the ekan import. Ekan is a program that takes the 
# excellent open-source KANJIDIC and EDICT projects and converts them
# to Panini source code.

# Because ekan is not that quick and EDICT is a huge file with many 
# thousands of entries, we'll grep away any lines that do not contain
# English words that we already know. 
cat /usr/share/edict/edict | iconv --from EUC-JP > edict.locale.txt
cat /usr/share/edict/kanjidic | iconv --from EUC-JP > imports/kanjidic.locale

make -C utils words
utils/words english > english.headwords.txt

rm -rf edict.2.txt
while read headword ; do
	grep $headword edict.locale.txt >> edict.2.txt
done < english.headwords.txt
rm imports/edict.locale
cat edict.2.txt | uniq | sort > imports/edict.locale

rm -f imports-japanese
make -C imports ekan.out
cat imports/ekan.out >> imports-japanese

# Lastly, we need to rebuild all the languages and reinstall
for lang in ainu czech english japanese nahuatl quenya swahili; do
	echo Compiling $lang
	./tc $lang > $lang
done
