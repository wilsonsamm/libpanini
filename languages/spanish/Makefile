spanish-pp-out: data.txt
	$(PANINI)/tools/pp/pp spanish

spanish: spanish-pp-out
	echo Compiling spanish.
	cat spanish-pp-out | sort | uniq > src/pp-out
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main > spanish

clean:
	rm -f spanish spanish-pp-out src/pp-out

install:
	cp spanish /usr/panini/languages
