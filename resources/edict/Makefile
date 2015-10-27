
download: edict.gz

edict.txt: edict.euc-jp
	iconv -f EUC-JP edict.euc-jp > edict.txt

edict.euc-jp: edict.gz
	zcat edict.gz > edict.euc-jp

edict.gz:
	echo Downloading EDICT
	wget -q http://ftp.monash.edu.au/pub/nihongo/edict.gz

clean:
	rm -f edict*
