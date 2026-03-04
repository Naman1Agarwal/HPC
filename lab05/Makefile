basiccompgol: basiccompgol.o
	gcc -Wall -g basiccompgol.o -o basiccompgol -fopenmp

basiccompgol.o: basiccompgol.c basiccompgol.h
	gcc -Wall -g -c basiccompgol.c -o basiccompgol.o -fopenmp

ompgol: ompgol.o
	gcc -Wall -g ompgol.o -o ompgol -fopenmp

ompgol.o: ompgol.c ompgol.h
	gcc -Wall -g -c ompgol.c -o ompgol.o -fopenmp

clean:
	rm -f ompgol ompgol.o basiccompgol basiccompgol.o