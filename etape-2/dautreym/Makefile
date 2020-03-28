all: utils.o parseur.o compare.o
	gcc -o parseur utils.o parseur.o compare.o
	rm *.o

parseur.o: parseur.c utils.h compare.h
	gcc -c -Wall parseur.c

utils.o: utils.c
	gcc -c -Wall utils.c

compare.o: compare.c compare.h
	gcc -c -Wall compare.c