spanish: stage3
	echo Compiling spanish.
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o spanish

# Stage 1: 
vocab.out:
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning vocab
	$(PANINI)/tools/learn/vocab vocab.txt
	

stage1: data.txt vocab.out
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning Spanish
	$(PANINI)/tools/learn/pp spanish 1
	mv pp.out stage1
	cat stage1 | sort | uniq >> src/pp-out
	
stage2: stage1 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning Spanish
	$(PANINI)/tools/learn/pp spanish 2
	mv pp.out stage2
	cat stage2 | sort | uniq >> src/pp-out
	
stage3: stage2 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	echo -n Learning Spanish
	$(PANINI)/tools/learn/pp spanish 3
	mv pp.out stage3
	cat stage3 | sort | uniq >> src/pp-out
	echo

clean:
	rm -f spanish src/pp-out src/vocab pp-out core spanish-pp-out
	rm -f stage3 stage2 stage1 vocab.out 

install:
	cp spanish /usr/panini/languages
