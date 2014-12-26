
pp:	main.o
	gcc -o pp main.o -lpanini

main.o: main.c
	gcc -ggdb -pg -g -c -Wall main.c

langclean:
	rm -rf spanish-pp-out french-pp-out norwegian-pp-out
	rm -rf spanish/pp-out french/pp-out norwegian/pp-out

clean: langclean
	rm -rf pp *.o

langs: pp langclean
	@./run.sh french
	@./run.sh spanish
	@./run.sh norwegian
	
