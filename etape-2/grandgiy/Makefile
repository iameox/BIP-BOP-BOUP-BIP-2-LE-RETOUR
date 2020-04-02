all: main.o parser.o
	gcc -o main main.o parser.o

main.o: main.c parser.h types.h
	gcc -c main.c -Wall

parser.o: parser.c types.h
	gcc -c parser.c -Wall

clean:
	rm -rf *.o