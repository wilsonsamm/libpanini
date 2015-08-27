all: UnihanOutput

Unihan.zip:
	wget http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip
	
Unihan_Readings.txt: Unihan.zip
	rm -f Unihan_Readings.txt
	unzip -u Unihan.zip Unihan_Readings.txt
	touch Unihan_Readings.txt

Unihan_Variants.txt: Unihan.zip
	rm -f Unihan_Variants.txt
	unzip -u Unihan.zip Unihan_Variants.txt
	touch Unihan_Variants.txt

kMandarin: Unihan_Readings.txt
	grep kMandarin Unihan_Readings.txt > kMandarin

kTraditionalVariant: Unihan_Variants.txt
	grep kTraditionalVariant Unihan_Variants.txt > kTraditionalVariant

kSimplifiedVariant: Unihan_Variants.txt
	grep kSimplifiedVariant Unihan_Variants.txt > kSimplifiedVariant
	
pinyin.o: pinyin.c pinyin.h
	gcc $(CCOPTS) -o pinyin.o pinyin.c
	
unihan_lookup.o: unihan_lookup.c pinyin.h
	gcc $(CCOPTS) -o unihan_lookup.o unihan_lookup.c

unihan_lookup: unihan_lookup.o pinyin.o pinyin.h
	gcc -o unihan_lookup unihan_lookup.o pinyin.o $(PANINI)/list.o $(PANINI)/list-tokenise.o

UnihanOutput: unihan_lookup kMandarin kTraditionalVariant kSimplifiedVariant
	./unihan_lookup kMandarin kSimplifiedVariant kTraditionalVariant
	
clean:
	rm -rf kMandarin kSimplifiedVariant kTraditionalVariant
	rm -rf Unihan_Readings.txt Unihan_Variants.txt
	rm -rf pinyin.o unihan_lookup.o unihan_lookup
	rm -rf Unihan.zip UnihanOutput
