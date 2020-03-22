all: utils.o parseur.o
	gcc -o parseur utils.o parseur.o
	rm *.o

parseur.o: parseur.c
	gcc -c -Wall parseur.c

utils.o: utils.c utils.h
	gcc -c -Wall utils.c
