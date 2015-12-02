download: kanjidic.gz

kanjidic.txt: kanjidic.euc-jp
	iconv -f EUC-JP edict.euc-jp > kanjidic.txt

kanjidic.euc-jp: kanjidic.gz
	zcat kanjidic.gz > kanjidic.euc-jp

kanjidic.gz:
	wget -q http://ftp.monash.edu.au/pub/nihongo/kanjidic.gz

clean:
	rm -f kanjidic*

deepclean: clean
