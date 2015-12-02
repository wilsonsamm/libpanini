download: language.csv
language.csv: wals-language.csv.zip
	unzip -qq wals-language.csv.zip

wals-language.csv.zip:
	wget -q http://wals.info/static/download/wals-language.csv.zip

clean:
	rm -rf README.txt language.csv wals-language.csv.zip

deepclean: clean
