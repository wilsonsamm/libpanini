#!/bin/bash

# First we need to remove any kfiles. These might contain some old stuff, and if
# anything is still relevant, a new kfile will get created.
rm -rf kfile-*
rm -rf english


# Next, various semantically loaded words should be passed to ./getpage.sh.
# ./getpage.sh is a script that looks the word up on wiktionary and ./parse-wt
# to extract information from the page.
for i in January February March April May June July August September October \
			November December
do
	./getpage.sh $i noun
done

./getpage.sh "axe" noun
./getpage.sh "hammer" noun
