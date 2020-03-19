CC=gcc -std=c99
CFLAGS=-Wall -Wextra -Wshadow -Wno-unused -pedantic -O3
FOPENMP=-fopenmp

all: horn_schunck_classic

horn_schunck_classic: horn_schunck_classic.c iio.o
	$(CC) $(CFLAGS) -o horn_schunck_classic horn_schunck_classic.c iio.o -lpng -ljpeg -ltiff -lm

iio.o: iio.c
	$(CC) $(CFLAGS) -DNDEBUG -D_GNU_SOURCE -c iio.c

clean:
	rm -f horn_schunck_classic

zip: clean
	cd .. ; tar zcfv phs_3.tar.gz phs_3

