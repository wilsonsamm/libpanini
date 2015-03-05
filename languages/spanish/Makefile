spanish: stage3
	echo
	echo Compiling spanish.
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o spanish

# Stage 1: 
vocab.out: vocab.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt
	cat vocab.out | sort | uniq > src/vocab-out

pp.txt: data.txt $(PP_SWORD_SP)
	cat data.txt $(PP_SWORD_SP) > pp.txt

$(PP_SWORD_SP):
	$(PANINI)/tools/sword/query english $(PP_SWORD_EN) spanish $(PP_SWORD_SP) >> $(PP_SWORD_SP) 

stage1: pp.txt vocab.out
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning Spanish
	$(PANINI)/tools/learn/pp pp.txt 1
	mv pp.out stage1
	cat stage1 | sort | uniq > src/pp-out
	
stage2: stage1 pp.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning Spanish
	$(PANINI)/tools/learn/pp pp.txt 2
	mv pp.out stage2
	cat stage2 stage1 | sort | uniq > src/pp-out
	
stage3: stage2 pp.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning Spanish
	$(PANINI)/tools/learn/pp pp.txt 3
	mv pp.out stage3
	cat stage3 stage2 stage1 | sort | uniq > src/pp-out

clean:
	rm -f spanish src/pp-out src/vocab-out pp-out core spanish-pp-out SpaRV
	rm -f stage3 stage2 stage1 vocab.out pp.out pp.txt

install:
	cp spanish /usr/panini/languages
