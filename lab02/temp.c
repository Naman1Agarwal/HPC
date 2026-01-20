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

/*
prints array
*/ 
void printArray(matrix* m){
    for (index_t  i = 0; i < m->rows; i++){
        for (index_t j = 0; j < m->cols; j++){
            printf("%hhu ", m->arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*
uses cmd line arguments to setup variables for simulations
*/ 
int parseArgs(simulation* sim, index_t offset, char* argv[]){
    if  ( sscanf(argv[offset], "%d", &sim->generations) != 1 ){
        return 1;
    }
    if ( sscanf(argv[offset+1], "%d", &sim->freq) != 1 ){
        return 1;
    }
    return 0;
}


/*
allocates memory to hold 2d grid of numbers
*/ 
matrix* initMat(index_t rows, index_t cols){
    matrix* mat = (matrix*) malloc(sizeof(matrix));
    mat->rows = rows;
    mat->cols = cols;
    value_t** arr = (value_t**) malloc(sizeof(value_t*) * rows);
    for (index_t i = 0; i < rows; i++){
        arr[i] = (value_t*) calloc(sizeof(value_t), cols);
    }
    mat->arr = arr;
    return mat;
}



/*
allocates memory to hold 2d grid of numbers
*/ 
matrix* readFile(FILE* fd){
    char buf[1024];
    char *p_rows, *p_cols, *p_n;
    int rows, cols, n;
    
    // reads first line
    fgets(buf, sizeof buf, fd);
    p_rows = strtok(buf, " ");
    p_cols = strtok(NULL, " ");
    p_n = strtok(NULL, " ");

    if (sscanf(p_rows, "%d", &rows) != 1 || rows <= 0){
        fprintf(stderr, "Error: File contains invalid input");
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_cols, "%d", &cols) != 1 || cols <= 0){
        fprintf(stderr, "Error: File contains invalid input");
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_n, "%d", &n) != 1 || n <= 0){
        fprintf(stderr, "Error: File contains invalid input");
        exit(EXIT_FAILURE);
    }

    matrix* mat = initMat(rows+2, cols+2);
    value_t** arr = mat->arr;

    // read each line and changes the value in arr
    char *p_x, *p_y, *p_val;
    value_t x, y, val;
    for (index_t i = 0; i < n; i++){
        fgets(buf, sizeof buf, fd);
        p_x = strtok(buf, " ");
        p_y = strtok(NULL, " ");
        p_val = strtok(NULL, " ");
        
        if (sscanf(p_x, "%hhu", &x) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_y, "%hhu", &y) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_val, "%hhu", &val) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }

        if (x < rows && y < cols && x >= 0 && y >= 0){
            arr[x+1][y+1] = val;
        }
        else{
            fprintf(stderr, "Error: File input outside bounds of matrix");
            exit(EXIT_FAILURE);
        }
    }

    return mat;
}



int main(int argc, char* argv[]){

    // sim is an object that will store the state of our simulation
    simulation* sim = (simulation*) malloc(sizeof(simulation));

    if (argc != 3 && argc != 6){
        fprintf(stderr, "Usage: %s generations frequency [seeed] [rows] [column] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // parse initial arguments
    if ( parseArgs(sim, 1, argv) == 1){
        fprintf(stderr, "Error: Input is not numbers");
        exit(EXIT_FAILURE);
    }

    // calls readFile if there were only 5 cmd line arguments
    if (argc == 3){
        char filename[512]; 
        printf("Matrix file path: ");
        scanf("%512s", filename);
        FILE* fd = fopen(filename, "r");
        
        if (fd == NULL){
            fprintf(stderr, "Error opening file %s: %s \n", filename, strerror(errno));
            exit(EXIT_FAILURE);
        }

        sim->old = readFile(fd);
        fclose(fd);
    }

    printArray(sim->old);
    
    // initializes grid of numbers to all 0
    /*if (argc == 6){
        int rows, cols;
        if (sscanf(argv[5], "%d", &rows) != 1){
            fprintf(stderr, "Error: Input is not numbers");
            exit(EXIT_FAILURE);
        }
        if (sscanf(argv[6], "%d", &cols) != 1){
            fprintf(stderr, "Error: Input is not numbers");
            exit(EXIT_FAILURE);
        }
        sim->mat = initMat(rows, cols);
    }*/
    return 0;
}
