httpparser: main.o libparser.so 
	gcc -c test.c
	gcc $(CCFLAGS) -o $@ $< -L. -lparser 

main.o: main.c test.h
	gcc $(CCFLAGS) -c $< -o $@

.PRECIOUS: %.c %.h

all: httpparser

clean : 
	rm -rf httpparser
	rm -rf *.o 


