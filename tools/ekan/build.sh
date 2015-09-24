#!/bin/bash

for file in /usr/panini/targets/english	/usr/lib/libpanini.a \
			/usr/share/edict/edict /usr/share/edict/kanjidic 
do
	if [ ! -e "$file" ]; then
		echo -n "$(basename $file) has not been installed, "
		echo "which means the EDICT tool cannot run."
		exit 1
	fi
done

if [ ! -e edict.txt ]; then
	iconv -f EUC-JP /usr/share/edict/edict > edict.txt
fi

if [ ! -e kanjidic.txt ]; then
	iconv -f EUC-JP /usr/share/edict/kanjidic > kanjidic.txt
fi

if [ ! -e ekan.out ]; then
	make ekan
	./ekan > ekan.out
fi
