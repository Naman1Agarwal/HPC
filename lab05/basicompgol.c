#include "basicompgol.h"

size_t g_rows;
size_t g_cols;
uint8_t** g_new;
uint8_t** g_old;

size_t generations;
size_t freq;
size_t verbosity;
size_t n_threads;

void initMat(){

    g_old = (uint8_t**) malloc(sizeof(uint8_t*) * g_cols);
    g_new = (uint8_t**) malloc(sizeof(uint8_t*) * g_cols);
    
    for (size_t i = 0; i < g_cols; i++) {
        g_old[i] = (uint8_t*) calloc(g_rows, sizeof(uint8_t));
        g_new[i] = (uint8_t*) calloc(g_rows, sizeof(uint8_t));
    }
}


void randMatrix(size_t offset){

    for (size_t j = offset; j < g_cols-offset; j++) {
        for (size_t i = offset; i < g_rows-offset; i++) {
            g_old[j][i] = (uint8_t) ( rand()%2 );
        }
    }
}


void printArray(size_t offset){
    for (size_t  i = offset; i < g_rows-offset; i++) {
        for (size_t j = offset; j < g_cols-offset; j++) {
            printf("%u ", (unsigned int) g_old[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}


void readFile(FILE* fd){

    size_t rows, cols, n;

    if (fscanf(fd, "%lu %lu %lu", &rows, &cols, &n) != 3){
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


void getMatFromUser() {

    size_t rows, cols;

    printf("num rows: ");
    if (scanf("%lu", &rows) != 1){
        fprintf(stderr, "Input was not an unsigned integer.\n");
        exit(EXIT_FAILURE);
    }
    
    if (rows != 0) {
        goto read_columns;
    }
    else if (rows == 0){
        goto read_file;
    }


read_columns:

    printf("num cols: ");
    if (scanf("%lu", &cols) != 1 || cols == 0){
        fprintf(stderr, "Input was not an unsigned integer > 0.\n");
        exit(EXIT_FAILURE);
    }
    
    int seed;
    printf("seed: ");
    if ( scanf("%d", &seed) != 1 ){
        fprintf(stderr, "Input was not an integer.\n");
        exit(EXIT_FAILURE);
    }

    g_rows = rows+2;
    g_cols = cols+2;
    initMat();

    if (seed < 0){
        srand(time(NULL));
    }
    else{
        srand(seed);
    }
    randMatrix(1);

    return;

read_file:

    char filename[1024] = { 0 }; 
    printf("Matrix file path: ");
    scanf("%1023s", filename);

    FILE* fd = fopen(filename, "r");
    if (fd == NULL){
        fprintf(stderr, "Error opening file %s: %s.\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    readFile(fd);
    fclose(fd);

    return;
}


void parseCmdLine(int argc, char* argv[]){
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
    if ( sscanf(argv[4], "%lu", &n_threads) != 1 || n_threads == 0){
        fprintf(stderr, "Error: %s could not be parsed as unsigned number > 0.\n", argv[4]);
        exit(EXIT_FAILURE);
    }
}


// check if a single node will stay alive or die
uint8_t nodeUpdate(size_t i, size_t j){

    uint8_t nalive = 0;
    uint8_t isalive = g_old[j][i];

    // loop through all neighbors
    for (size_t testj = j-1; testj <= j+1; testj++){
        for (size_t testi = i-1; testi <= i+1; testi++){
            if (g_old[testj][testi] == 1){
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


void iter(){

    size_t start_col, end_col;

    // calculate each thread's area
    int id = omp_get_thread_num();
    int cols_per_thread = (g_cols-2) / n_threads;
    start_col = cols_per_thread * id + 1;
    end_col = start_col + cols_per_thread - 1;
    if (id == n_threads-1) {
        end_col += (g_cols-2) % n_threads;
    }

    if (verbosity > 0) {
        printf("Id: %u Start Column: %lu End Column: %lu\n", id, start_col, end_col);
    }

    for (size_t generation = 0; generation < generations; generation++) {

        for (size_t j = start_col; j <= end_col; j++) {
            for (size_t i = 1; i <= g_rows-2; i++) {
                g_new[j][i] = nodeUpdate(i, j);
            }
        }

        // update the outer row layer
        for (size_t j = start_col; j <= end_col; j++) {
            g_new[j][g_rows-1] = g_new[j][1];
            g_new[j][0] = g_new[j][g_rows-2];
        }

        // update corners and sides
        if (id == 0) {
            memcpy(g_new[g_cols-1]+1, g_new[1]+1, g_rows-2);
            g_new[g_cols-1][g_rows-1] = g_new[1][1];
            g_new[g_cols-1][0]      = g_new[1][g_rows-2];
        }
        else if (id == n_threads-1) {
            memcpy(g_new[0]+1, g_new[g_cols-2]+1, g_rows-2);
            g_new[0][g_rows-1] = g_new[g_cols-2][1];
            g_new[0][0]      = g_new[g_cols-2][g_rows-2];
        }

        #pragma omp barrier
        
        // display and switch arrays grid
        if (id == 0) {

            if (freq != 0 && generation % freq == 0){
                printf("------------\n");
                printf("Count: %ld\n", generation);
                printf("------------\n");
                printArray(1);
            }

            uint8_t** temp = g_new;
            g_new = g_old;
            g_old = temp;
        }
        
        #pragma omp barrier
    }
}

// from notes used to measure execution time
TIME_DIFF * my_difftime (struct timeval * start, struct timeval * end){
    TIME_DIFF * diff = (TIME_DIFF *) malloc ( sizeof (TIME_DIFF) );

    if (start->tv_sec == end->tv_sec) {
        diff->secs = 0;
        diff->usecs = end->tv_usec - start->tv_usec;
    }
    else {
        diff->usecs = 1000000 - start->tv_usec;
        diff->secs = end->tv_sec - (start->tv_sec + 1);
        diff->usecs += end->tv_usec;
        if (diff->usecs >= 1000000) {
            diff->usecs -= 1000000;
            diff->secs += 1;
        }
    }

    return diff;
}

void clean(){
    for (size_t i = 0; i < g_cols; i++){
        free(g_old[i]);
        free(g_new[i]);
    }
    free(g_old);
    free(g_new);
}

int main(int argc, char* argv[]){

    parseCmdLine(argc, argv);

    getMatFromUser();

    if (n_threads > (g_cols-2)){
        fprintf(stderr, "Error: threads exceed columns in matrix; reduce # of threads\n");
        exit(EXIT_FAILURE);
    }

    omp_set_num_threads(n_threads);

    struct timeval myTVstart, myTVend;
    gettimeofday(&myTVstart, NULL);

#pragma omp parallel
    iter();

    gettimeofday(&myTVend, NULL);
    TIME_DIFF* difference = my_difftime(&myTVstart, &myTVend);

    if (freq != 0){
        printf("------------\n");
        printf("Final\n");
        printf("------------\n");
        printArray(1);
        printf("------------\n");
    }

     if (verbosity == 2){
        printf("\n");
        printf("time: sec %3d  microsec %6d \n", difference->secs, difference->usecs);
    }

    clean();
    free(difference);

    return 0;
}