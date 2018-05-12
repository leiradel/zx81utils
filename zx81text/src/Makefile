all: zx81text

zx81text: zx81text.o
	gcc -o $@ $+

zx81text.o: zx81text.c ../../common/zx81rom.h
	gcc -O3 -I../../common -c $< -o $@

clean:
	rm -f zx81text zx81text.o

.PHONY: clean FORCE
