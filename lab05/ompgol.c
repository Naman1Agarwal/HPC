#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>
#include <omp.h>

size_t g_rows;
size_t g_cols;
uint8_t** g_new;
uint8_t** g_old;

size_t generations;
size_t freq;
size_t verbosity;
size_t n_threads;


void printArray(size_t offset) {
    for (size_t  i = offset; i < g_rows-offset; i++) {
        for (size_t j = offset; j < g_cols-offset; j++) {
            printf("%u ", (unsigned int) g_old[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}


void randMatrix(size_t offset) {

    for (size_t j = offset; j < g_cols-offset; j++) {
        for (size_t i = offset; i < g_rows-offset; i++) {
            g_old[j][i] = (uint8_t) ( rand()%2 );
        }
    }
}


void initMat() {

    g_old = (uint8_t**) malloc(sizeof(uint8_t*) * g_cols);
    g_new = (uint8_t**) malloc(sizeof(uint8_t*) * g_cols);
    
    for (size_t i = 0; i < g_cols; i++) {
        g_old[i] = (uint8_t*) calloc(sizeof(uint8_t), g_rows);
        g_new[i] = (uint8_t*) calloc(sizeof(uint8_t), g_rows);
    }
}



void readFile(FILE* fd) {

    size_t rows, cols, n;

    if (fscanf(fd, "%lu %lu %lu", &rows, &cols, &n) != 3) {
        fprintf(stderr, "Unable to read three numbers from the file\n");
        exit(EXIT_FAILURE);
    }

    g_rows = rows+2;
    g_cols = cols+2;
    initMat();

    size_t x, y;
    for (size_t i = 0; i < n; i++){
        if (fscanf(fd, "%lu %lu ", &x, &y) != 2) {
            fprintf(stderr, "Unable to read two numbers from the file\n");
            exit(EXIT_FAILURE);
        }

        if (x >= rows || y >= cols){
            fprintf(stderr, "Indexes %lu and %lu not within matrix bounds.\n", x, y);
            exit(EXIT_FAILURE);
        }

        g_old[y+1][x+1] = 1;
    }
}


int main(int argc, char* argv[]){

    

    return 0;
}