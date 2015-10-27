download: glottolog-languoid.csv.zip

languoid.csv: glottolog-languoid.csv.zip
	unzip -qq glottolog-languoid.csv.zip
	
glottolog-languoid.csv.zip:
	echo Downloading information from Glottolog
	wget -q http://glottolog.org/static/download/glottolog-languoid.csv.zip

clean:
	rm -rf README.txt wals.csv wals-language.csv.zip
