pan: pan.o
	$(CCOMPILER) -o pan pan.o -lpanini
	
pan.o: pan.c
	$(CCOMPILER) $(CCOPTS) pan.c

clean:
	rm -f *.o pan

deepclean: clean
