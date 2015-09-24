#!/bin/bash
for file in /usr/lib/libpanini.a /usr/include/panini.h 
do
	if [ ! -e "$file" ]; then
		echo -n $file has not been installed,
		echo which means that the Wiktionary cannot "(yet)" be queried.
		exit 1
	fi
done

# First we need to remove any kfiles. These might contain some old stuff, and if
# anything is still relevant, a new kfile will get created.
rm -rf kfile-*
rm -rf english

# We'll need to build the executables.
make

# Next, various semantically loaded words should be passed to ./getpage.sh.
# ./getpage.sh is a script that looks the word up on wiktionary and ./parse-wt
# to extract information from the page.
# (Eventually, I'll write something that queries Panini for words known in one
# language and not another. These are the words that needs to be looked up.)
for i in January February March April May June July August September October \
			November December
do
	./getpage.sh $i noun
done
./getpage.sh "axe" noun
./getpage.sh "hammer" noun
