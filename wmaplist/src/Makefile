all: wmaplist

wmaplist: wmaplist.o simz80.o mem_mmu.o
	gcc -o $@ $+

wmaplist.o: wmaplist.c ../../common/xltables.h ../../common/zx81rom.h
	gcc -O3 -I../../common -c $< -o $@

simz80.o: simz80.c
	gcc -O3 -I../../common -c $< -o $@

mem_mmu.o: mem_mmu.c
	gcc -O3 -I../../common -c $< -o $@

clean:
	rm -f ageplist wmaplist.o simz80.o mem_mmu.o

.PHONY: clean FORCE
