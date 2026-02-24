#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>

size_t rows;
size_t cols;
uint8_t** old;
uint8_t** new;

size_t generations;
size_t freq;
int seed;
size_t verbosity;
size_t n_threads;

#define TRUE 1
#define FALSE 0

void printArray(size_t offset){
    for (size_t  i = offset; i < rows-offset; i++){
        for (size_t j = offset; j < cols-offset; j++){
            printf("%u ", (unsigned int) old[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}


void initMat(size_t arg_rows, size_t arg_cols){
    cols = arg_cols;
    rows = arg_rows;

    // init by cols
    old = (uint8_t**) malloc(sizeof(uint8_t*) * cols);
    for (size_t i = 0; i < cols; i++){
        old[i] = (uint8_t*) calloc(sizeof(uint8_t), rows);
    }
}


void readFile(FILE* fd){

    size_t arg_cols, arg_rows;

    char buf[1024];
    char *p_rows, *p_cols, *p_n;
    size_t n;
    
    // reads first line
    fgets(buf, sizeof buf, fd);
    p_rows = strtok(buf, " ");
    p_cols = strtok(NULL, " ");
    p_n = strtok(NULL, " ");

    if (sscanf(p_rows, "%lu", &arg_rows) != 1){
        fprintf(stderr, "Error: %s is invalid input.\n", p_rows);
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_cols, "%lu", &arg_cols) != 1){
        fprintf(stderr, "Error: %s is invalid input.\n", p_cols);
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_n, "%lu", &n) != 1){
        fprintf(stderr, "Error: %s is invalid input.\n", p_n);
        exit(EXIT_FAILURE);
    }

    initMat(arg_rows+2, arg_cols+2);

    // read each line and changes the value in arr
    char *p_x, *p_y;
    size_t x, y;

    for (size_t i = 0; i < n; i++){
        fgets(buf, sizeof buf, fd);
        p_x = strtok(buf, " ");
        p_y = strtok(NULL, " ");

        if (sscanf(p_x, "%lu", &x) != 1){
            fprintf(stderr, "Error: %s is not an unsigned integer.\n", p_x);
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_y, "%lu", &y) != 1){
            fprintf(stderr, "Error: %s is not an unsigned integer.\n", p_y);
            exit(EXIT_FAILURE);
        }

        if (x < arg_rows && y < arg_cols){
            old[y+1][x+1] = 1;
        }
        else{
            fprintf(stderr, "Error: File input outside bounds of matrix.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void randMatrix(size_t offset){
    for (size_t j = offset; j < cols-offset; j++){
        for (size_t i = offset; i < rows-offset; i++){
            old[j][i] = (uint8_t) ( rand()%2 );
        }
    }
}

void getMatFromUser(){

    size_t arg_rows, arg_cols;

    printf("num rows: ");
    if (scanf("%lu", &arg_rows) != 1){
        fprintf(stderr, "Input was not an unsigned integer.\n");
        exit(EXIT_FAILURE);
    }
    if (arg_rows != 0) {
        goto read_columns;
    }
    else if (arg_rows == 0){
        goto read_file;
    }

read_columns:
    printf("num cols: ");
    if ( scanf("%lu", &arg_cols) != 1 ){
        fprintf(stderr, "Input was not an unsigned integer.\n");
        exit(EXIT_FAILURE);
    }
    printf("seed: ");
    if ( scanf("%d", &seed) != 1 ){
        fprintf(stderr, "Input was not an integer.\n");
        exit(EXIT_FAILURE);
    }

    initMat(arg_rows + 2, arg_cols + 2);

    randMatrix(1);

    return;

read_file:
    char filename[1024]; 
    printf("Matrix file path: ");
    scanf("%1024s", filename);
        
    FILE* fd = fopen(filename, "r");
    if (fd == NULL){
        fprintf(stderr, "Error opening file %s: %s.\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    readFile(fd);

    fclose(fd);

    return;
}

int main(int argc, char* argv[]){

    if (argc != 5){
        fprintf(stderr, "Usage: %s <generations> <frequency> <verbosity> <threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if ( sscanf(argv[1], "%lu", &generations) != 1 ){
        fprintf(stderr, "Error: %s could not be parsed as unsigned number.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if ( sscanf(argv[2], "%lu", &freq) != 1 ){
        fprintf(stderr, "Error: %s could not be parsed as unsigned number.\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    if ( sscanf(argv[3], "%lu", &verbosity) != 1 ){
        fprintf(stderr, "Error: %s could not be parsed as unsigned number.\n", argv[3]);
        exit(EXIT_FAILURE);
    }
    if ( sscanf(argv[4], "%lu", &n_threads) != 1 ){
        fprintf(stderr, "Error: %s could not be parsed as unsigned number.\n", argv[4]);
        exit(EXIT_FAILURE);
    }

    getMatFromUser();

    printArray(0);

    //printf("Cmd line args %lu %lu %lu %lu\n", generations, freq, verbosity, n_threads);
    //printf("Cols and rows: %lu %lu\n", cols, rows);

    return 0;
}