#!/bin/bash
for file in /usr/lib/libpanini.a /usr/include/panini.h
do
	if [ ! -e "$file" ]; then
		echo -n "$file has not been installed, "
		echo "which means that the Wiktionary cannot (yet) be queried."
		exit 1
	fi
done

# First we need to remove any kfiles. These might contain some old stuff, and if
# anything is still relevant, a new kfile will get created.
rm -rf kfile-*
rm -rf english

# We'll need to build the executables.
make -s all

# Next, various semantically loaded words should be passed to ./getpage.sh.
# ./getpage.sh is a script that looks the word up on wiktionary and ./parse-wt
# to extract information from the page.
# (Eventually, I'll write something that queries Panini for words known in one
# language and not another. These are the words that needs to be looked up.)
echo -ne Querying for words ...
./allwords english "(call noun singular)"
echo done.
CURR=0
for lexeme in $(cat AllEnglishWords.txt ExtraWords.txt)
do
	echo $lexeme | xargs >> AllWords.txt
done
cat AllWords.txt | sort | uniq > AllWordsSorted.txt
TOTAL=$(cat AllWordsSorted.txt | wc -l) 
while IFS='' read -r line || [[ -n "$line" ]]
do
	CURR=$(($CURR + 1))

	echo -ne Looking at Wiktionary data "$((CURR * 100 / TOTAL))%" 
	echo -ne " $line          " "\r"
	./getpage.sh $line
done < AllWordsSorted.txt
echo
