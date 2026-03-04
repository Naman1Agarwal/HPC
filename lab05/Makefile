basicompgol: basicompgol.o
	gcc -Wall -g basicompgol.o -o basicompgol -fopenmp

basicompgol.o: basicompgol.c basicompgol.h
	gcc -Wall -g -c basicompgol.c -o basicompgol.o -fopenmp

ompgol: ompgol.o
	gcc -Wall -g ompgol.o -o ompgol -fopenmp

ompgol.o: ompgol.c ompgol.h
	gcc -Wall -g -c ompgol.c -o ompgol.o -fopenmp

clean:
	rm -f ompgol ompgol.o basicompgol basicompgol.o