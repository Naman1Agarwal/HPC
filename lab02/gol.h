#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>

typedef uint8_t value_t;
typedef uint32_t index_t;
typedef int32_t  init_t;

typedef struct matrix{
    index_t rows;
    index_t cols;
    value_t** arr;
}matrix;

typedef struct simulation{
    init_t generations;
    init_t freq;
    init_t seed;
    matrix* old;
    matrix* new;
}simulation;


void printArray(matrix* m, index_t offset);

int parseArgs(simulation* sim, index_t offset, char* argv[]);

matrix* initMat(index_t rows, index_t cols);

matrix* readFile(FILE* fd);

void genRandVals(matrix* mat, index_t offset);

value_t nodeUpdate(value_t** m, index_t i, index_t j);

void updateEdges(matrix* m, index_t offset);

void update(matrix* old, matrix* new);

void iterate(simulation* sim);


