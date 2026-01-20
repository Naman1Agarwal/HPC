#include "array.h"

/*
print 2d grid of numbers
*/ 
void printArray(matrix* m){
    for (int i = 0; i < m->rows; i++){
        for (int j = 0; j < m->cols; j++){
            printf("%3d ", m->arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


/*
uses cmd line arguments to setup variables for simulations
*/ 
int parseArgs(simulation* sim, int offset, char* argv[]){
    if  ( sscanf(argv[offset], "%d", &sim->iterations) != 1 ){
        return 1;
    }
    if ( sscanf(argv[offset+1], "%d", &sim->threshold) != 1 ){
        return 1;
    }
    if ( sscanf(argv[offset+2], "%d", &sim->freq) != 1 ){
        return 1;
    }
    if ( sscanf(argv[offset+3], "%d", &sim->seed) != 1 ){
        return 1;
    }
    return 0;
}


/*
allocates memory to hold 2d grid of numbers
*/ 
matrix* initMat(int rows, int cols){
    matrix* mat = (matrix*) malloc(sizeof(matrix));
    mat->rows = rows;
    mat->cols = cols;
    int** arr = (int**) malloc(sizeof(int*) * rows);
    for (int i = 0; i < rows; i++){
        arr[i] = (int*) calloc(sizeof(int), cols);
    }
    mat->arr = arr;
    return mat;
}


/*
reads file to set up grid
exits if file has non-integer input
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

    if (sscanf(p_rows, "%d", &rows) != 1){
        fprintf(stderr, "Error: File contains non-integer input");
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_cols, "%d", &cols) != 1){
        fprintf(stderr, "Error: File contains non-integer input");
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_n, "%d", &n) != 1){
        fprintf(stderr, "Error: File contains non-integer input");
        exit(EXIT_FAILURE);
    }

    matrix* mat = initMat(rows, cols);
    int** arr = mat->arr;

    // read each line and changes the value in arr
    char *p_x, *p_y, *p_val;
    int x, y, val;
    for (int i = 0; i < n; i++){
        fgets(buf, sizeof buf, fd);
        p_x = strtok(buf, " ");
        p_y = strtok(NULL, " ");
        p_val = strtok(NULL, " ");
        
        if (sscanf(p_x, "%d", &x) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_y, "%d", &y) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_val, "%d", &val) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }

        if (x < rows && y < cols && x >= 0 && y >= 0){
            arr[x][y] = val;
        }
        else{
            fprintf(stderr, "Error: File input outside bounds of matrix");
            exit(EXIT_FAILURE);
        }
    }

    return mat;
}

/*
updates each value in grid by adding a random value
does not update the value if it falls outside min and max
*/ 
short update(matrix* mat, int min, int max){

    int r = mat->rows;
    int c = mat->cols;
    int temp, rand_val;
    short valid = 0;

    for (int i = 0; i < r; i++){
        for (int j = 0; j < c; j++){

            temp = mat->arr[i][j];

            if (temp <= max && temp >= min){
                valid = 1;
                rand_val = (int) (21.0 * ((double) rand()/(RAND_MAX+1.0))) - 10;
                mat->arr[i][j] += rand_val;
            }
        }
    }

    return valid;
}

/*
calls update and prints the array as needed
*/ 
void iterate(simulation* sim){

    int min = sim->threshold*-1;
    int max = sim->threshold;
    matrix* mat = sim->mat;
    short cont;


    for (int i = 0; i < sim->iterations; i++){

        if (sim->freq != 0 && i % (sim->freq) == 0 ){
            printf("Count: %d\n", i);
            printf("------------\n");
            printArray(mat);
            printf("------------\n");
        }
        cont = update(mat, min, max);

        if (! cont ){
            break;
        }
    }

    printf("final\n");
    printf("------------\n");
    printArray(mat);
    printf("------------\n");
}

/*
frees allocated memory
*/ 
void clean(simulation* sim){
    matrix* mat = sim->mat;
    for (int i = 0; i < mat->rows; i++){
        free(mat->arr[i]);
    }
    free(mat->arr);
    free(mat);
    free(sim);
}


int main(int argc, char* argv[]){

    // sim is an object that will store the state of our simulation
    simulation* sim = (simulation*) malloc(sizeof(simulation));

    if (argc != 5 && argc != 7){
        fprintf(stderr, "Usage: %s iterations threshold frequency seeed [rows] [column] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // parse initial arguments
    if ( parseArgs(sim, 1, argv) == 1){
        fprintf(stderr, "Error: Input is not numbers");
        exit(EXIT_FAILURE);
    }

    // calls readFile if there were only 5 cmd line arguments
    if (argc == 5){
        char filename[1024]; 
        printf("Matrix file path: ");
        scanf("%1024s", filename);
        FILE* fd = fopen(filename, "r");
        
        if (fd == NULL){
            fprintf(stderr, "Error opening file %s: %s \n", filename, strerror(errno));
            exit(EXIT_FAILURE);
        }

        sim->mat = readFile(fd);
        fclose(fd);
    }
    // initializes grid of numbers to all 0
    if (argc == 7){
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
    }

    // seed for our random number generation
    if (sim->seed < 0){
        srand(time(NULL));
    }
    else {
        srand(sim->seed);
    }

    // let the simulation run
    iterate(sim);

    // clean memory
    clean(sim);

    return 0;
}