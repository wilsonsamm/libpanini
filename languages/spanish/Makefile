spanish: stage3
	echo Compiling spanish.
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o spanish

# Stage 1: 
stage1: data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	$(PANINI)/tools/pp/pp spanish 1
	mv spanish-pp-out stage1
	#echo \(close conj-aff\) >> stage1
	cat stage1 | sort | uniq >> src/pp-out
	
stage2: stage1 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	$(PANINI)/tools/pp/pp spanish 2
	mv spanish-pp-out stage2
	cat stage2 | sort | uniq >> src/pp-out
	
stage3: stage2 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o spanish
	$(PANINI)/tools/pp/pp spanish 3
	mv spanish-pp-out stage3
	cat stage3 | sort | uniq >> src/pp-out

clean:
	rm -f spanish src/pp-out pp-out core spanish-pp-out
	rm -f stage3 stage2 stage1

install:
	cp spanish /usr/panini/languages
