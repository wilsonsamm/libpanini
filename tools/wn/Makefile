wn-bootstrap: bootstrap.o
	$(CCOMPILER) -o wn-bootstrap bootstrap.o -lwordnet
	
bootstrap.o: bootstrap.c
	$(CCOMPILER) $(CCOPTS) bootstrap.c

clean:
	rm bootstrap.o wn-bootstrap
