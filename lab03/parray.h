
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct simulation{
    int iterations;
    int threshold;
    int freq;
    int seed;
    int threads;
    int verbose;
    
    int rows; // 2d grid attributes
    int cols;
    int** arr;
}simulation;

typedef struct{
    int id;
    int start_col;
    int end_col;
}thread_args;

// time struct from notes
typedef struct {
    int     secs;
    int     usecs;
} TIME_DIFF;


#define FALSE 0
#define TRUE 1


TIME_DIFF * my_difftime (struct timeval * start, struct timeval * end);


int parseArgs(simulation* sim, int offset, char* argv[]);


void initMat(simulation* sim, int rows, int cols);


void printArray(simulation* sim);


void readFile(FILE* fd, simulation* sim);


void getMatFromUser(simulation* sim);


void* iter(void* varg);


void clean(simulation* sim);