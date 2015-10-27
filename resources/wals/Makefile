download: wals-language.csv.zip
language.csv: wals-language.csv.zip
	unzip -qq wals-language.csv.zip

wals-language.csv.zip:
	echo Downloading information from the World Atlas of Language Structures
	wget -q http://wals.info/static/download/wals-language.csv.zip

clean:
	rm -rf README.txt wals.csv wals-language.csv.zip
