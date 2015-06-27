all: query

query: query.cc
	$(CXXCOMPILER) -I/usr/include/sword/ -o query query.cc -lsword -lstdc++

clean:
	rm -f query query.o
