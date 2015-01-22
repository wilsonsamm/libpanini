nobuild:
	@echo Do not invoke make directly.
	@echo Use the enclosed script build.sh.
	@echo -n You may also do "make docs" to generate documentation, 
	@echo which turns up in the
	@echo docs/ directory.

love:
	@echo Not war?

libpanini.a: list.o list-tokenise.o generate.o parse.o spawn.o learn.o monad.o bind.o variables.o misc.o intext.o outtext.o exec.o binders.o memory.o reduce.o phrase.o operations.o panini.o 
	ar rcs libpanini.a list.o list-tokenise.o generate.o parse.o spawn.o learn.o monad.o bind.o variables.o misc.o intext.o outtext.o exec.o binders.o memory.o reduce.o phrase.o operations.o panini.o 

list.o: list/list.c list/list.h
	gcc $(CCOPTS) list/list.c 

bind.o: panini/bind.c
	gcc $(CCOPTS) panini/bind.c 

list-tokenise.o: list/list-tokenise.c list/list.h
	gcc $(CCOPTS) list/list-tokenise.c 
	
variables.o: monad/monad.h panini/variables.c
	gcc $(CCOPTS) panini/variables.c 

monad.o: monad/monad.h monad/monad.c
	gcc $(CCOPTS) monad/monad.c 

operations.o: monad/monad.h monad/operations.c
	gcc $(CCOPTS) monad/operations.c

panini.o: panini/panini.c
	gcc $(CCOPTS) panini/panini.c

parse.o: monad/monad.h monad/monad.c panini/parse.c
	gcc $(CCOPTS) panini/parse.c 

exec.o: monad/monad.h monad/monad.c panini/exec.c
	gcc $(CCOPTS) panini/exec.c 

generate.o: monad/monad.h monad/monad.c panini/generate.c
	gcc $(CCOPTS) panini/generate.c 

misc.o: monad/monad.h monad/monad.c panini/misc.c
	gcc $(CCOPTS) panini/misc.c 

binders.o: monad/monad.h monad/monad.c panini/binders.c
	gcc $(CCOPTS) panini/binders.c 

intext.o: monad/monad.h panini/intext.c
	gcc $(CCOPTS) panini/intext.c 

outtext.o: monad/monad.h panini/outtext.c
	gcc $(CCOPTS) panini/outtext.c 

phrase.o: monad/monad.h panini/phrase.c
	gcc $(CCOPTS) panini/phrase.c

reduce.o: monad/monad.h panini/reduce.c
	gcc $(CCOPTS) panini/reduce.c

memory.o: monad/monad.h panini/memory.c
	gcc $(CCOPTS) panini/memory.c

learn.o: monad/monad.h monad/monad.c panini/learn.c
	gcc $(CCOPTS) panini/learn.c 

spawn.o: monad/monad.h monad/monad.c monad/spawn.c
	gcc $(CCOPTS) monad/spawn.c 

docs:
	make -C doc

ainu:
	+make -C languages/ainu ainu
algonquian:
	+make -C languages/algonquian algonquian
czech:
	+make -C languages/czech czech
english:
	+make -C languages/english english
japanese:
	+make -C languages/japanese japanese
nahuatl:
	+make -C languages/nahuatl nahuatl
pp:
	+make -C languages/pp pp
quenya:
	+make -C languages/quenya quenya
spanish:
	+make -C languages/spanish spanish
swahili:
	+make -C languages/swahili swahili

install: uninstall
	mkdir -p /usr/panini/languages
	cp libpanini.a /usr/lib/
	cp panini.h /usr/include
	make -C languages/ainu install
	make -C languages/algonquian install
	make -C languages/czech install
	make -C languages/english install
	make -C languages/japanese install
	make -C languages/nahuatl install
	make -C languages/pp install
	make -C languages/quenya install
	make -C languages/spanish install
	make -C languages/swahili install

uninstall:
	rm -rf /usr/panini
	rm -rf /usr/lib/libpanini.a
	rm -rf /usr/include/panini.h

clean: 
	rm -f *.o *.gch *.a *.txt *.gz vlad core tc learn a.out gmon.out 
	rm -f imports-japanese
	rm -f monad/*.o monad/*.gch panini/*.o panini/*.gch list/*.o 
	rm -f list/*.gch panini/*.o panini/*.gch compiler/*.o panini/*.o
	rm -f ainu algonquian czech english japanese nahuatl pp quenya 
	rm -f spanish swahili
	make -C demos/tranny clean
	make -C demos/kanjify clean
	make -C doc clean
	make -C tools/pcomp clean
	make -C tools/pp clean
	make -C tools/ekan clean
	make -C languages/ainu clean
	make -C languages/algonquian clean
	make -C languages/czech clean
	make -C languages/english clean
	make -C languages/japanese clean
	make -C languages/nahuatl clean
	make -C languages/pp clean
	make -C languages/quenya clean
	make -C languages/spanish clean
	make -C languages/swahili clean

# Git commands might be useful to have in a Makefile.
pull:
	git pull

push: clean
	git commit -a
	git push

# I keep making these typoes.
celan: clean
isntall: install
