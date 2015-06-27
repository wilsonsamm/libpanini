all: pinyin jyutping simplified

Unihan.zip:
	wget http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip

Unihan_Readings.txt: Unihan.zip
	unzip -u Unihan.zip Unihan_Readings.txt

Unihan_Variants.txt: Unihan.zip
	unzip -u Unihan.zip Unihan_Variants.txt

lookup.o: lookup.c
	$(CCOMPILER) $(CCOPTS) lookup.c

lookup: lookup.o
	$(CCOMPILER) -o lookup lookup.o

define.o: define.c
	$(CCOMPILER) $(CCOPTS) define.c

define: define.o
	$(CCOMPILER) -o define define.o

pinyin: Unihan_Readings.txt define lookup
	./lookup Unihan_Readings.txt kMandarin | ./define pinyin > pinyin

jyutping: Unihan_Readings.txt define lookup
	./lookup Unihan_Readings.txt kCantonese | ./define jyutping > jyutping

simplified: Unihan_Variants.txt define lookup
	./lookup Unihan_Variants.txt kTraditionalVariant | ./define utf8 | ./define simplified > simplified

simplified: Unihan_Variants.txt define lookup
	./lookup Unihan_Variants.txt kTraditionalVariant | ./define utf8 | ./define simplified > simplified

clean:
	rm -rf *.o *.txt lookup define pinyin jyutping simplified

deepclean: clean
	rm -rf *.zip
