all: utils.o parseur.o compare.o api.o tree.o main_test.o
	gcc -o parseur utils.o parseur.o compare.o api.o tree.o main_test.o
	rm *.o

parseur.o: parseur.c utils.h compare.h parseur.h api.h tree.h
	gcc -c -Wall parseur.c

utils.o: utils.c utils.h compare.h parseur.h api.h tree.h
	gcc -c -Wall utils.c

compare.o: compare.c utils.h compare.h parseur.h api.h tree.h
	gcc -c -Wall compare.c

api.o: api.c utils.h compare.h parseur.h api.h tree.h
	gcc -c -Wall api.c

tree.o: tree.c utils.h compare.h parseur.h api.h tree.h
	gcc -c -Wall tree.c

main_test.o: main_test.c utils.h compare.h parseur.h api.h tree.h
	gcc -c -Wall main_test.c
