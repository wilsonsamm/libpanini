pp-out: data.txt
	$(PANINI)/tools/pp/pp spanish > pp-out

spanish: pp-out
	cp pp-out src/
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > spanish

clean:
	rm -f spanish pp-out src/pp-out

install:
	cp spanish /usr/panini/languages
