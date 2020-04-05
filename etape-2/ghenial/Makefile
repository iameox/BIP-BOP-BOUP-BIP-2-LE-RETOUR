all: main.o api.o parser.o abnf.o structures.o
	gcc -o main main.o api.o parser.o abnf.o structures.o

main.o: main.c api.h
	gcc -c main.c -Wall

api.o: api.c *.h
	gcc -c api.c -Wall

parser.o: parser.c *.h
	gcc -c parser.c -Wall

abnf.o: abnf.c abnf.h
	gcc -c abnf.c -Wall

structures.o: structures.c structures.h
	gcc -c structures.c -Wall

parseur: parser.o abnf.o structures.o
	gcc -o agaga parser.o abnf.o structures.o

clean:
	rm -rf *.o