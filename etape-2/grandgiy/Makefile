all: main.o parser.o
	gcc -o main main.o parser.o

main.o: main.c parser.h types.h
	gcc -c main.c parser.h types.h -Wall

parser.o: parser.c types.h
	gcc -c parser.c types.h -Wall

clean:
	rm -rf *.o