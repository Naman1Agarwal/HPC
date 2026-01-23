#include "gol.h"

void printArray(matrix* m, index_t offset){
    for (index_t  i = offset; i < m->rows-offset; i++){
        for (index_t j = offset; j < m->cols-offset; j++){
            printf("%u ", (unsigned int) m->arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


int parseArgs(simulation* sim, index_t offset, char* argv[]){
    if  ( sscanf(argv[offset], "%d", &sim->generations) != 1 ){
        return 1;
    }
    if ( sscanf(argv[offset+1], "%d", &sim->freq) != 1 ){
        return 1;
    }
    return 0;
}


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


matrix* readFile(FILE* fd){
    char buf[1024];
    char *p_rows, *p_cols, *p_n;
    index_t rows, cols, n;
    
    // reads first line
    fgets(buf, sizeof buf, fd);
    p_rows = strtok(buf, " ");
    p_cols = strtok(NULL, " ");
    p_n = strtok(NULL, " ");

    if (sscanf(p_rows, "%u", &rows) != 1){
        fprintf(stderr, "Error: File contains invalid input");
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_cols, "%u", &cols) != 1){
        fprintf(stderr, "Error: File contains invalid input");
        exit(EXIT_FAILURE);
    }
    if (sscanf(p_n, "%u", &n) != 1){
        fprintf(stderr, "Error: File contains invalid input");
        exit(EXIT_FAILURE);
    }

    matrix* mat = initMat(rows+2, cols+2);
    value_t** arr = mat->arr;

    // read each line and changes the value in arr
    char *p_x, *p_y;
    index_t x, y;

    for (index_t i = 0; i < n; i++){
        fgets(buf, sizeof buf, fd);
        p_x = strtok(buf, " ");
        p_y = strtok(NULL, " ");

        if (sscanf(p_x, "%u", &x) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }
        if (sscanf(p_y, "%u", &y) != 1){
            fprintf(stderr, "Error: File contains non-integer input");
            exit(EXIT_FAILURE);
        }

        if (x < rows && y < cols && x >= 0 && y >= 0){
            arr[x+1][y+1] = 1;
        }
        else{
            fprintf(stderr, "Error: File input outside bounds of matrix");
            exit(EXIT_FAILURE);
        }
    }

    return mat;
}


void genRandVals(matrix* mat, index_t offset){

    for (index_t i = offset; i < mat->rows-offset; i++){
        for (index_t j = offset; j < mat->cols-offset; j++){
            mat->arr[i][j] = (value_t) ( rand()%2 );
        }
    }
}


value_t nodeUpdate(value_t** m, index_t i, index_t j){
    
    value_t nalive = 0;
    value_t isalive = m[i][j];

    for (index_t testi = i-1; testi <= i+1; testi++){
        for (index_t testj = j-1; testj <= j+1; testj++){

            if (testi == i && testj == j){
                continue;
            }

            if (m[testi][testj] == 1){
                nalive++;
            }
        }
    }

    if (isalive && ( nalive == 2 || nalive == 3) ){
        return 1;
    }
    else if (!isalive && (nalive == 3)){
        return 1;
    }

    return 0;
}


void updateEdges(matrix* m, index_t offset){

    index_t rows = m->rows;
    index_t cols = m->cols;
    index_t rl_bound = offset;
    index_t ru_bound = rows-offset-1;
    index_t cl_bound = offset;
    index_t cu_bound = cols-offset-1;

    // sides
    for (index_t j = cl_bound; j <= cu_bound; j++){
        if (m->arr[rl_bound][j] == 1) {
            m->arr[ru_bound+1][j] = 1;
        }
    }
    for (index_t j = cl_bound; j <= cu_bound; j++){
        if (m->arr[ru_bound][j] == 1) {
            m->arr[rl_bound-1][j] = 1;
        }
    }

    for (index_t i = rl_bound; i <= ru_bound; i++){
        if (m->arr[i][cl_bound] == 1) {
            m->arr[i][cu_bound+1] = 1;
        }
    }
    for (index_t i = rl_bound; i <= ru_bound; i++){
        if (m->arr[i][cu_bound] == 1) {
            m->arr[i][cl_bound-1] = 1;
        }
    }

    // corners
    if (m->arr[rl_bound][cl_bound] == 1){
        m->arr[ru_bound+1][cu_bound+1] = 1;
    }
    if (m->arr[rl_bound][cu_bound] == 1){
        m->arr[ru_bound+1][cl_bound-1] = 1;
    }
    if (m->arr[ru_bound][cl_bound] == 1){
        m->arr[rl_bound-1][cu_bound+1] = 1;
    }
    if (m->arr[ru_bound][cu_bound] == 1){
        m->arr[rl_bound-1][cl_bound-1] = 1;
    }
}


void update(matrix* old, matrix* new){

    index_t r = old->rows;
    index_t c = old->cols;
    value_t** old_arr = old->arr;

    for (index_t i = 1; i < r-1; i++){
        for (index_t j = 1; j < c-1; j++){
            new->arr[i][j] = nodeUpdate(old_arr, i, j);
        }
    }

    updateEdges(new, 1);
}



void iterate(simulation* sim){

    for (index_t i = 0; i < sim->generations; i++){

        if (sim->freq != 0 && i % (sim->freq) == 0 ){
            printf("Count: %d\n", i);
            printf("------------\n");
            printArray(sim->old, 1);
            printf("------------\n");
        }

        // we update the next generation of lives based on the old board
        update(sim->old, sim->new);

        // switch for next update
        matrix* temp = sim->old;
        sim->old = sim->new;
        sim->new = temp;
    }

    printf("final\n");
    printf("------------\n");
    printArray(sim->old, 1);
    printf("------------\n");
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

    // reads file
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

    // random init of lives
    if (argc == 6){
        if ( sscanf(argv[3], "%d", &sim->seed) != 1 ){
            fprintf(stderr, "Error: Input is not numbers");
            exit(EXIT_FAILURE);
        }
        
        if (sim->seed < 0){
            srand(time(NULL));
        }
        else {
            srand(sim->seed);
        }

        index_t rows, cols;
        if (sscanf(argv[4], "%u", &rows) != 1){
            fprintf(stderr, "Error: Input is not numbers");
            exit(EXIT_FAILURE);
        }
        if (sscanf(argv[5], "%u", &cols) != 1){
            fprintf(stderr, "Error: Input is not numbers");
            exit(EXIT_FAILURE);
        }


        sim->old = initMat(rows+2, cols+2);
        genRandVals(sim->old, 1);
    }

    sim->new = initMat(sim->old->rows, sim->old->cols);

    iterate(sim);

    return 0;
}