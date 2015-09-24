#!/bin/bash

if [ ! -e $PANINI/libpanini.a ]; then
	echo "libpanini.a does not exist, which means the Unihan tool cannot run."
	exit 1
fi

if [ ! -e "Unihan.zip" ]; then
	wget http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip
fi

for f in Unihan_Readings.txt Unihan_Variants.txt
do
	if [ ! -e "$f" ]; then
		unzip -qq -u Unihan.zip $f
	fi
done

for f in kMandarin
do
	if [ ! -e "$f" ]; then
		grep $f Unihan_Readings.txt > $f
	fi
done

for f in kTraditionalVariant kSimplifiedVariant
do
	if [ ! -e "$f" ]; then
		grep $f Unihan_Readings.txt > $f
	fi
done

if [ ! -e unihan_lookup ]; then
	make unihan_lookup
fi

if [ ! -e UnihanOutput ]; then
	./unihan_lookup kMandarin kSimplifiedVariant kTraditionalVariant
fi
