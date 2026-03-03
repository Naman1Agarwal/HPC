#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>

void printArray(uint8_t** arr, size_t offset){
    for (size_t  i = offset; i < rows-offset; i++){
        for (size_t j = offset; j < cols-offset; j++){
            printf("%u ", (unsigned int) arr[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}


uint8_t** initMat(size_t rows, size_t cols){

    uint8_t** arr = (uint8_t**) malloc(sizeof(uint8_t*) * cols);
    for (size_t i = 0; i < cols; i++){
        arr[i] = (uint8_t*) calloc(sizeof(uint8_t), rows);
    }

    return arr;
}

void randMatrix(uint8_t** arr, size_t offset){

    for (size_t j = offset; j < cols-offset; j++){
        for (size_t i = offset; i < rows-offset; i++){
            arr[j][i] = (uint8_t) ( rand()%2 );
        }
    }
}

uint8_t** readFile(FILE* fd){

    size_t rows, cols, n;

    if (fscanf(fd, "%lu %lu %lu", &rows, &cols, &n) != 3){
        fprintf(stderr, "Unable to read three numbers from the file\n");
        exit(EXIT_FAILURE);
    }

    uint8_t* arr = initMat(rows+2, cols+2);

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

        arr[y+1][x+1] = 1;
    }

    return arr;
}

void getMatFromUser() {
    
    size_t rows, cols;

    printf("num rows: ");
    if (scanf("%lu", &rows) != 1){
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
    if (scanf("%lu", &cols) != 1 || cols == 0){
        fprintf(stderr, "Input was not an unsigned integer > 0.\n");
        exit(EXIT_FAILURE);
    }
    
    printf("seed: ");
    if ( scanf("%d", &seed) != 1 ){
        fprintf(stderr, "Input was not an integer.\n");
        exit(EXIT_FAILURE);
    }

    uint8_t** arr = initMat(rows, cols);

    if (seed < 0){
        srand(time(NULL));
    }
    else{
        srand(seed);
    }
    randMatrix(arr, 1);

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

    uint8_t** arr = readFile(fd);
    fclose(fd);
    return;
}

int main(int argc, char* argv[]){

    return 1;
}