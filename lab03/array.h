#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

// this struct will store our matrix of numbers 
typedef struct matrix{
    int rows;
    int cols;
    int** arr;
}matrix;

// this struct will store the state of our simulation
typedef struct simulation{
    int iterations;
    int threshold;
    int freq;
    int seed;
    int threads;
    int verbose;
    matrix* mat;
}simulation;

/*
print 2d grid of numbers
*/ 
void printArray(matrix* m);


/*
uses cmd line arguments to setup variables for simulations
*/ 
int parseArgs(simulation* sim, int offset, char* argv[]);


/*
allocates memory to hold 2d grid of numbers
*/ 
matrix* initMat(int rows, int cols);


/*
reads file to set up grid
exits if file has non-integer input
*/ 
matrix* readFile(FILE* fd);


/*
frees allocated memory
*/ 
void clean(simulation* sim);