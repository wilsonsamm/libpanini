all: kMandarin kTraditionalVariant kSimplifiedVariant
download: Unihan.zip
Unihan.zip:
	echo Downloading the Unihan data
	wget -q http://www.unicode.org/Public/UCD/latest/ucd/Unihan.zip

Unihan_Readings.txt: Unihan.zip
	unzip -qq -u Unihan.zip Unihan_Readings.txt

Unihan_Variants.txt: Unihan.zip
	unzip -qq -u Unihan.zip Unihan_Variants.txt

kMandarin: Unihan_Readings.txt
	grep kMandarin Unihan_Readings.txt > kMandarin

kTraditionalVariant: Unihan_Variants.txt
	grep kTraditional Unihan_Variants.txt > kTraditionalVariant

kSimplifiedVariant: Unihan_Variants.txt
	grep kSimplified Unihan_Variants.txt > kSimplifiedVariant

clean:
	rm -f Unihan_Readings.txt Unihan_Variants.txt Unihan.zip
	rm -f kMandarin kSimplifiedVariant kTraditionalVariant
