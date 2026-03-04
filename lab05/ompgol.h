#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>
#include <omp.h>

extern size_t g_rows;
extern size_t g_cols;
extern uint8_t** g_new;
extern uint8_t** g_old;

extern size_t generations;
extern size_t freq;

typedef struct {
    int     secs;
    int     usecs;
} TIME_DIFF;


void printArray(size_t offset);

void randMatrix(size_t offset);

void initMat();

void readFile(FILE* fd);

void getMatFromUser();

void parseCmdLine(int argc, char* argv[]);

uint8_t nodeUpdate(size_t i, size_t j);

void iter();

TIME_DIFF * my_difftime (struct timeval * start, struct timeval * end);

void clean();