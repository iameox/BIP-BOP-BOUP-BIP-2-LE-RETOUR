httpparser: main.o test.o libparser.so 
	gcc $(CCFLAGS) -o $@ $< -L. -lparser 

main.o: main.c test.h
	gcc $(CCFLAGS) -c $< -o $@
	
test.o: test.c test.h
	gcc $(CCFLAGS) -c $< -o $@

.PRECIOUS: %.c %.h

all: httpparser

clean : 
	rm -rf httpparser
	rm -rf *.o 


