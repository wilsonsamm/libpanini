nobuild:
	@echo Do not invoke make directly.
	@echo Use the enclosed script build.sh.
	@echo -n You may also do "make docs" to generate documentation, 
	@echo which turns up in the
	@echo docs/ directory.

love:
	@echo Not war?

libpanini.a: list.o list-tokenise.o generate.o parse.o spawn.o learn.o monad.o bind.o intext.o outtext.o exec.o ns.o memory.o reduce.o phrase.o operations.o panini.o 
	ar rcs libpanini.a list.o list-tokenise.o generate.o parse.o spawn.o learn.o monad.o bind.o intext.o outtext.o exec.o ns.o memory.o reduce.o phrase.o operations.o panini.o 

list.o: list/list.c list/list.h
	gcc $(CCOPTS) list/list.c 

bind.o: panini/bind.c
	gcc $(CCOPTS) panini/bind.c 

list-tokenise.o: list/list-tokenise.c list/list.h
	gcc $(CCOPTS) list/list-tokenise.c 

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

ns.o: monad/monad.h monad/monad.c panini/ns.c
	gcc $(CCOPTS) panini/ns.c 

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

install: uninstall
	mkdir -p /usr/panini/targets
	cp libpanini.a /usr/lib/
	cp panini.h /usr/include
	+make -C targets/ainu install
	+make -C targets/algonquian install
	+make -C targets/chinese install
	+make -C targets/czech install
	+make -C targets/english install
	+make -C targets/japanese install
	+make -C targets/nahuatl install
	+make -C targets/pp install
	+make -C targets/quenya install
	+make -C targets/swahili install
	+make -C targets/world install

uninstall:
	rm -rf /usr/panini
	rm -rf /usr/lib/libpanini.a
	rm -rf /usr/include/panini.h

clean: 
	rm -f *.o *.gch *.a *.gz vlad core tc learn a.out gmon.out 
	rm -f imports-japanese
	rm -f monad/*.o monad/*.gch panini/*.o panini/*.gch list/*.o 
	rm -f list/*.gch panini/*.o panini/*.gch compiler/*.o panini/*.o

