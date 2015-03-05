czech: stage1
	echo
	echo Compiling czech.
	$(PANINI)/tools/pcomp/pcomp -d src -i src/main -o czech

# Read the bibles and learn some Czech.
$(PP_SWORD_CZ):
	$(PANINI)/tools/sword/query english $(PP_SWORD_EN) czech $(PP_SWORD_CZ) > pp.txt

pp.txt: $(PP_SWORD_CZ) data.txt
	cat $(PP_SWORD_CZ) data.txt > pp.txt

stage1: pp.txt vocab.out
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning Czech
	$(PANINI)/tools/learn/pp pp.txt 3
	mv pp.out stage1
	cat stage1 | sort | uniq > src/pp-out

# Something that imports some vocabulary. 
vocab.out: 
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o czech
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt
	cat vocab.out | sort | uniq > src/vocab

clean:
	rm -f czech core vocab.out pp.txt src/vocab

install:
	cp czech /usr/panini/languages
