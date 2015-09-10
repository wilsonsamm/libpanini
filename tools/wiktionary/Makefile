all: parse-wt
	./script.sh

parse-wt.o : parse-wt.c
	$(CCOMPILER) $(CCOPTS) parse-wt.c

parse-wt: parse-wt.o
	$(CCOMPILER) -o parse-wt parse-wt.o -lpanini

clean:
	rm -rf english kfile* *.o parse-wt
	
deepclean: clean
	rm -rf wt-*
