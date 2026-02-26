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

typedef struct{
    size_t id;
    size_t start_col;
    size_t end_col;
}thread_args;

pthread_barrier_t barrier;
pthread_mutex_t lock;


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

    new = (uint8_t**) malloc(sizeof(uint8_t*) * cols);
    for (size_t i = 0; i < cols; i++){
        new[i] = (uint8_t*) calloc(sizeof(uint8_t), rows);
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

    if (seed < 0){
        srand(time(NULL));
    }
    else{
        srand(seed);
    }

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
    if ( scanf("%lu", &arg_cols) != 1 || arg_cols == 0){
        fprintf(stderr, "Input was not an unsigned integer > 0.\n");
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


uint8_t nodeUpdate(size_t i, size_t j){

    uint8_t nalive = 0;
    uint8_t isalive = old[j][i];

    for (size_t testj = j-1; testj <= j+1; testj++){
        for (size_t testi = i-1; testi <= i+1; testi++){
            if (old[testj][testi] == 1){
                nalive++;
            }
        }
    }

    nalive -= isalive;

    if (isalive && (nalive == 2 || nalive == 3)){
        return 1;
    }
    else if (!isalive && (nalive == 3)){
        return 1;
    }
    return 0;
}


void* iter(void* varg){

    thread_args* arg = (thread_args*) varg;
    size_t id = arg->id;
    size_t start_col = arg->start_col;
    size_t end_col = arg->end_col;

    if (verbosity > 0){
        printf("Id: %d Start Column: %d End Column: %d\n", id, start_col, end_col);
    }

    for (size_t generation = 0; generation < generations; generation++){

        for (size_t j = start_col; j <= end_col; j++){
            for (size_t i = 1; i <= rows-2; i++){
                new[j][i] = nodeUpdate(i, j);
            }
        }

        // update the outer row layer
        for (size_t j = start_col; j <= end_col; j++){
            if (new[j][1] == 1){
                new[j][rows-1] = 1;
            }
            if (new[j][rows-2] == 1){
                new[j][0] = 1;
            }
        }

        // update corners and sides
        if (id == 0){
            memcpy(new[cols-1]+1, new[1]+1, rows-2);
            new[cols-1][rows-1] = new[1][1];
            new[cols-1][0]      = new[1][rows-2];
        }
        else if (id == n_threads-1){
            memcpy(new[0]+1, new[cols-2]+1, rows-2);
            new[0][rows-1] = new[cols-2][1];
            new[0][0]      = new[cols-2][rows-2];
        }

        pthread_barrier_wait(&barrier);

        // display and switch arrays grid
        if (id == 0){

            uint8_t** temp = new;
            new = old;
            old = temp;

            if (freq != 0 && generation % freq == 0){
                printf("Count: %d\n", generation+1);
                printf("------------\n");
                printArray(1);
                printf("------------\n");
            }
        }

        pthread_barrier_wait(&barrier);
    }

    return NULL;
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

    if (n_threads > (cols-2)){
        fprintf(stderr, "Error: threads exceed columns in matrix; reduce # of threads");
        exit(EXIT_FAILURE);
    }

    // distribute columns for the threads
    pthread_t threads[n_threads];
    thread_args args[n_threads];
    
    int cols_per_thread = (cols-2) / n_threads;
    int rem  = (cols-2) % n_threads;
    int start_col = 1;
    int end_col = 1;

    for (int i = 0; i < n_threads; i++){
        
        end_col = start_col + cols_per_thread-1 + (rem > 0 ? 1 : 0);

        args[i].id = i;
        args[i].start_col = start_col;
        args[i].end_col = end_col;

        start_col = end_col+1;
        rem -= 1;
    }

    // start thread process
    if (pthread_barrier_init(&barrier, NULL, n_threads) != 0){
        fprintf(stderr, "Error: pthread_barrier_init failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n_threads; i++){
        if (pthread_create(&threads[i], NULL, iter, (void*)&args[i]) != 0){
            fprintf(stderr, "Error: pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    if (freq != 0){
        printf("Final\n");
        printf("------------\n");
        printArray(1);
        printf("------------\n");
    }

    // join threads and clean
    for (int i = 0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);


    return 0;
}