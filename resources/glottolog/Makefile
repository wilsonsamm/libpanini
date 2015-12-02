download: glottolog-languoid.csv.zip

languoid.csv: glottolog-languoid.csv.zip
	unzip -qq glottolog-languoid.csv.zip
	
glottolog-languoid.csv.zip:
	wget -q http://glottolog.org/static/download/glottolog-languoid.csv.zip

clean:
	rm -rf languoid.csv glottolog-languoid.csv.zip

deepclean: clean
