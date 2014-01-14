all: vlad languages libtranny.a

learner: learner.c libtranny.a
	gcc -ggdb -c -Wall learn.c
	gcc -o learn learn.o list.o list-tokenise.o monad.o monad-parse.o monad-generate.o monad-gowild.o bind.o monad-learn.o monad-spawn.o variables.o

vlad: validate.o list.o list-tokenise.o monad.o parse.o generate.o gowild.o learn.o bind.o spawn.o variables.o
	gcc -o vlad validate.o list.o list-tokenise.o monad.o parse.o generate.o gowild.o bind.o learn.o spawn.o variables.o

libtranny.a: list.o list-tokenise.o generate.o gowild.o parse.o spawn.o learn.o monad.o bind.o variables.o 
	ar rcs libtranny.a list.o list-tokenise.o generate.o gowild.o parse.o spawn.o learn.o monad.o bind.o variables.o

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

gowild.o: monad/monad.h monad/monad.c tranny/gowild.c
	gcc -ggdb -c -Wall tranny/gowild.c

spawn.o: monad/monad.h monad/monad.c monad/spawn.c
	gcc -ggdb -c -Wall monad/spawn.c

learn.o: monad/monad.h monad/monad.c tranny/learn.c
	gcc -ggdb -c -Wall tranny/learn.c
	
compiler: compiler-main.o compiler-definition.o compiler-check.o compiler-sandhi-init.o compiler/compiler.h list.o list-tokenise.o compiler-sandhi-fin.o
	gcc -o tc compiler-main.o list.o list-tokenise.o compiler-check.o compiler-sandhi-init.o compiler-definition.o compiler-sandhi-fin.o

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

learn-ainu: learn 
	@./learner ainu
	@cat learn.out | uniq >> languages/ainu/learned 
	@rm -rf learn.out
	
install: 
	mkdir -p /usr/tranny/languages
	mv nahuatl swahili ainu czech english quenya /usr/tranny/languages
	cp libtranny.a /usr/lib/
	cp tranny.h /usr/include

uninstall:
	rm -rf /usr/tranny
	rm -rf /usr/lib/libtranny.a

clean: lang-clean
	rm -f *.o *.gch *.a vlad core tc learn a.out 
	rm -f monad/*.o monad/*.gch tranny/*.o tranny/*.gch list/*.o list/*.gch vars/*.o vars/*.gch
	rm -f demos/dictionary/*.txt demos/dictionary/*.o demos/dictionary/*.gch demos/dictionary/wordlist demos/dictionary/dictionary
	rm -f demos/tranny/*.o demos/tranny/*.gch demos/tranny/tranny
	
	
	
	

# I keep making this typo.
celan: clean
