all: ageplist

ageplist: ageplist.o
	gcc -o $@ $+

ageplist.o: ageplist.c ../../common/xltables.h
	gcc -O3 -I../../common -c $< -o $@

clean:
	rm -f ageplist ageplist.o

.PHONY: clean FORCE
