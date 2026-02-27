#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct{
    size_t id;
    size_t start_col;
    size_t end_col;
}thread_args;

typedef struct {
    int     secs;
    int     usecs;
} TIME_DIFF;

extern size_t rows;
extern size_t cols;
extern uint8_t** old;
extern uint8_t** new;

extern size_t generations;
extern size_t freq;
extern int seed;
extern size_t verbosity;
extern size_t n_threads;

extern pthread_barrier_t barrier;


TIME_DIFF * my_difftime (struct timeval * start, struct timeval * end);

void printArray(size_t offset);

void initMat(size_t arg_rows, size_t arg_cols);

void readFile(FILE* fd);

void randMatrix(size_t offset);

void getMatFromUser();

uint8_t nodeUpdate(size_t i, size_t j);

void* iter(void* varg);

void clean();