#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

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
    matrix* mat;
}simulation;

void printArray(matrix* m);
int parseArgs(simulation* sim, int offset, char* argv[]);
matrix* initMat(int rows, int cols);
matrix* readFile(FILE* fd);
short update(matrix* mat, int min, int max);
void iterate(simulation* sim);
void clean(simulation* sim);