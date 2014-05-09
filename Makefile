all: vlad languages libtranny.a

vlad: validate.o list.o list-tokenise.o monad.o parse.o generate.o learn.o bind.o spawn.o variables.o
	gcc -o vlad validate.o list.o list-tokenise.o monad.o parse.o generate.o bind.o learn.o spawn.o variables.o

libtranny.a: list.o list-tokenise.o generate.o parse.o spawn.o learn.o attest.o monad.o bind.o variables.o 
	ar rcs libtranny.a list.o list-tokenise.o generate.o parse.o spawn.o learn.o attest.o monad.o bind.o variables.o

validate.o: validate.c
	gcc -ggdb -c -Wall validate.c

list.o: list/list.c list/list.h
	gcc -ggdb -c -Wall list/list.c

bind.o: vars/bind.c
	gcc -ggdb -c -Wall vars/bind.c

list-tokenise.o: list/list-tokenise.c list/list.h
	gcc -ggdb -c -Wall list/list-tokenise.c
	
variables.o: monad/monad.h vars/variables.c
	gcc -ggdb -c -Wall vars/variables.c

monad.o: monad/monad.h monad/monad.c
	gcc -ggdb -c -Wall monad/monad.c

parse.o: monad/monad.h monad/monad.c tranny/parse.c
	gcc -ggdb -c -Wall tranny/parse.c

generate.o: monad/monad.h monad/monad.c tranny/generate.c
	gcc -ggdb -c -Wall tranny/generate.c

attest.o: monad/monad.h monad/monad.c tranny/attest.c
	gcc -ggdb -c -Wall tranny/attest.c
	
spawn.o: monad/monad.h monad/monad.c monad/spawn.c
	gcc -ggdb -c -Wall monad/spawn.c

learn.o: monad/monad.h monad/monad.c tranny/learn.c
	gcc -ggdb -c -Wall tranny/learn.c
	
compiler: compiler-main.o compiler-definition.o compiler-check.o compiler-sandhi-init.o compiler/compiler.h list.o list-tokenise.o compiler-sandhi-fin.o compiler-for.o compiler-ontology.o
	gcc -o tc compiler-main.o list.o list-tokenise.o compiler-check.o compiler-sandhi-init.o compiler-definition.o compiler-sandhi-fin.o compiler-for.o compiler-ontology.o

compiler-main.o: compiler/main.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-main.o compiler/main.c

compiler-check.o: compiler/check.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-check.o compiler/check.c

compiler-definition.o: compiler/definition.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-definition.o compiler/definition.c

compiler-sandhi-init.o: compiler/sandhi-init.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-sandhi-init.o compiler/sandhi-init.c

compiler-sandhi-fin.o: compiler/sandhi-fin.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-sandhi-fin.o compiler/sandhi-fin.c

compiler-for.o: compiler/for.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-for.o compiler/for.c
	
compiler-ontology.o: compiler/ontology.c compiler/compiler.h
	gcc -ggdb -c -Wall -o compiler-ontology.o compiler/ontology.c
	
languages: lang-clean nahuatl swahili ainu english czech quenya

lang-clean:
	rm -rf nahuatl swahili ainu english czech quenya

quenya: compiler
	@echo Compiling Quenya.
	@./tc quenya > quenya
	
czech: compiler
	@echo Compiling Czech.
	@./tc czech > czech
	
english: compiler
	@echo Compiling English.
	@./tc english > english

nahuatl: compiler
	@echo Compiling Nahuatl.
	@./tc nahuatl > nahuatl

swahili: compiler
	@echo Compiling Swahili.
	@./tc swahili > swahili

ainu: compiler
	@echo Compiling Ainu.
	@./tc ainu > ainu
	
install: 
	mkdir -p /usr/tranny/languages
	mkdir -p /usr/tranny/learned
	mkdir -p /usr/tranny/attested
	mv nahuatl swahili ainu czech english quenya /usr/tranny/languages
	cp libtranny.a /usr/lib/
	cp tranny.h /usr/include

uninstall:
	rm -rf /usr/tranny
	rm -rf /usr/lib/libtranny.a

clean: lang-clean
	rm -f *.o *.gch *.a vlad core tc learn a.out 
	rm -f monad/*.o monad/*.gch tranny/*.o tranny/*.gch list/*.o list/*.gch vars/*.o vars/*.gch compiler/*.o
	make -C demos/dictionary clean
	make -C demos/tranny clean
	make -C utils clean
	
# Git commands might be useful to have in a Makefile.
pull:
	git pull

push: clean
	git commit -a
	git push

# I keep making this typo.
celan: clean
