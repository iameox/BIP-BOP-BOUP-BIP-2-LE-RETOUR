all: main.o api.o parser.o abnf.o structure.o
	gcc -o main main.o api.o parser.o abnf.o structure.o

main.o: main.c api.h
	gcc -c main.c -Wall

api.o: api.c *.h
	gcc -c api.c -Wall

parser.o: parser.c *.h
	gcc -c parser.c -Wall

abnf.o: abnf.c abnf.h
	gcc -c abnf.c -Wall

structure.o: structure.c structure.h
	gcc -c structure.c -Wall

test: parser.o abnf.o structure.o
	gcc -o parseur_test parser.o abnf.o structure.o

clean:
	rm -rf *.o