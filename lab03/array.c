#include "array.h"


void printArray(matrix* m){
    for (int i = 0; i < m->rows; i++){
        for (int j = 0; j < m->cols; j++){
            printf("%3d ", m->arr[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}


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
     if ( sscanf(argv[offset+4], "%d", &sim->verbose) != 1 ){
        return 1;
    }
    return 0;
}


matrix* initMat(int rows, int cols){
    matrix* mat = (matrix*) malloc(sizeof(matrix));
    mat->rows = rows;
    mat->cols = cols;
    // does cols first because threads access memory by column
    int** arr = (int**) malloc(sizeof(int*) * cols);
    for (int i = 0; i < cols; i++){
        arr[i] = (int*) calloc(sizeof(int), rows);
    }
    mat->arr = arr;
    return mat;
}


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
    int row_i, col_j, val;
    for (int i = 0; i < n; i++){
        fgets(buf, sizeof buf, fd);
        p_x = strtok(buf, " ");
        p_y = strtok(NULL, " ");
        p_val = strtok(NULL, " ");
        
        if (sscanf(p_x, "%d", &row_i) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_y, "%d", &col_j) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_val, "%d", &val) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }

        if ( row_i < rows && col_j < cols && row_i >= 0 && col_j >= 0){
            arr[col_j][row_i] = val;
        }
        else{
            fprintf(stderr, "Error: File input outside bounds of matrix");
            exit(EXIT_FAILURE);
        }
    }

    return mat;
}


void clean(simulation* sim){
    matrix* mat = sim->mat;
    for (int i = 0; i < mat->cols; i++){
        free(mat->arr[i]);
    }
    free(mat->arr);
    free(mat);
    free(sim);
}


void getMatFromUser(simulation* sim){
    int rows, cols;

    printf("num rows: ");
    if ( scanf("%u", &rows) != 1 ){
        fprintf(stderr, "Input was not an unsigned integer\n");
        exit(EXIT_FAILURE);
    }

    if (rows == 0){
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
    else{
        printf("num cols: ");
        if ( scanf("%u", &cols) != 1 ){
            fprintf(stderr, "Input was not an unsigned integer\n");
            exit(EXIT_FAILURE);
        }

        sim->mat = initMat(rows, cols);
    }
}


int main(int argc, char* argv[]){

    // sim is an object that will store the state of our simulation
    simulation* sim = (simulation*) malloc(sizeof(simulation));

    if (argc != 6){
        fprintf(stderr, "Usage: %s iterations threshold frequency seeed verbosity\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // parse initial arguments
    if ( parseArgs(sim, 1, argv) == 1){
        fprintf(stderr, "Error: Input is not numbers");
        exit(EXIT_FAILURE);
    }

    // gets grid specifications from user
    getMatFromUser(sim);

    // seed for our random number generation
    if (sim->seed < 0){
        srand(time(NULL));
    }
    else {
        srand(sim->seed);
    }

    // clean memory
    clean(sim);

    return 0;
}