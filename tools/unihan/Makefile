all: UnihanOutput
	
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
	rm -rf UnihanOutput

deepclean: clean
	rm -rf Unihan.zip
