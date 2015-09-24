parse-wt: parse-wt.o
	$(CCOMPILER) -o parse-wt parse-wt.o -lpanini
parse-wt.o : parse-wt.c
	$(CCOMPILER) $(CCOPTS) parse-wt.c
clean:
	rm -rf english kfile* *.o *.panini parse-wt
	
deepclean: clean
	rm -rf wt-*
