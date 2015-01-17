spanish: stage2
	echo Compiling spanish.
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -o spanish

# Stage 0: Basic conjugation.
stage0: data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o stage0
	$(PANINI)/tools/pp/pp stage0
	cat stage0-pp-out >> pp-out
	cat pp-out | sort | uniq > src/pp-out

# Stage 1: Learn some vocabulary (mainly verbstems)
stage1: stage0 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o stage1
	$(PANINI)/tools/pp/pp stage1
	cat stage1-pp-out >> pp-out
	echo \(close conj-aff\) >> pp-out
	cat pp-out | sort | uniq > src/pp-out
	
stage2: stage1 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o stage2
	$(PANINI)/tools/pp/pp stage2
	cat stage2-pp-out >> pp-out
	cat pp-out | sort | uniq > src/pp-out
	
stage3: stage2 data.txt
	$(PANINI)/tools/pcomp/pcomp -d ./src/ -i src/main -l $(PANINI)/languages/pp/pp -o stage3
	$(PANINI)/tools/pp/pp stage3
	cat stage3-pp-out >> pp-out
	cat pp-out | sort | uniq > src/pp-out

clean:
	rm -f spanish src/pp-out pp-out core
	rm -f stage3 stage3-pp-out 
	rm -f stage2 stage2-pp-out 
	rm -f stage1 stage1-pp-out 
	rm -f stage0 stage0-pp-out 

install:
	cp spanish /usr/panini/languages
