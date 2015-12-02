ainu:
	touch $(PANINI)/tools/wiktionary/ainu.panini
	cat $(PANINI)/tools/wiktionary/ainu.panini src/wt | sort | uniq > ./wt
	mv ./wt src/wt
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main > ainu

clean:
	rm -f ainu core

install:
	cp ainu /usr/panini/targets

deepclean: clean
