#include "parray.h"

simulation sim;
pthread_barrier_t barrier;
pthread_mutex_t lock;
int global_cont = 0;

// from notes
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

// get all cmd line args
int parseArgs(simulation* sim, int offset, char* argv[]){
    if  ( sscanf(argv[offset], "%d", &sim->iterations) != 1 ){
        return FALSE;
    }
    if ( sscanf(argv[offset+1], "%d", &sim->threshold) != 1 ){
        return FALSE;
    }
    if ( sscanf(argv[offset+2], "%d", &sim->freq) != 1 ){
        return FALSE;
    }
    if ( sscanf(argv[offset+3], "%d", &sim->seed) != 1 ){
        return FALSE;
    }
    if ( sscanf(argv[offset+4], "%d", &sim->threads) != 1 ){
        return FALSE;
    }
    if ( sscanf(argv[offset+5], "%d", &sim->verbose) != 1 ){
        return FALSE;
    }
    return TRUE;
}

// malloc memory for 2d grid
void initMat(simulation* sim, int rows, int cols){
    sim->rows = rows;
    sim->cols = cols;
    // does cols first because threads access memory by column
    int** arr = (int**) malloc(sizeof(int*) * cols);
    for (int i = 0; i < cols; i++){
        arr[i] = (int*) calloc(sizeof(int), rows);
    }
    sim->arr = arr;
}

// print 2d grid
void printArray(simulation* sim){
    for (int i = 0; i < sim->rows; i++){
        for (int j = 0; j < sim->cols; j++){
            printf("%d ", sim->arr[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}

// read 3 numbers from each line in a file
void readFile(FILE* fd, simulation* sim){
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
    
    initMat(sim, rows, cols);
    int** arr = sim->arr;

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
}

// init 2d grid with file or just zeros
void getMatFromUser(simulation* sim){
    int rows, cols;

    printf("num rows: ");
    if (scanf("%u", &rows) != 1){
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

        readFile(fd, sim);

        fclose(fd);
    }
    else{
        printf("num cols: ");
        if ( scanf("%u", &cols) != 1 ){
            fprintf(stderr, "Input was not an unsigned integer\n");
            exit(EXIT_FAILURE);
        }

        initMat(sim, rows, cols);
    }
}

// pthread_create expects function of type void* (*) (void*)
void* iter(void* varg){

	// get columns to update
	thread_args* arg = (thread_args*) varg;
	int id = arg->id;
	int start_col = arg->start_col;
	int end_col = arg->end_col;

	// other attributes needed for update
	int max = sim.threshold;
	int min = sim.threshold*-1;
	int rows = sim.rows;
	int** arr = sim.arr;
	int temp;
	int cont = 0;

    if (sim.verbose > 0){
        printf(
            "tid %d    columns: %d:%d    (%d)\n", 
            id, start_col, end_col, end_col-start_col+1
        );
    }

	for (int loop = 0; loop < sim.iterations; loop++){

		cont = 0;

		// update with random values
		for (int j = start_col; j <= end_col; j++){
            for (int i = 0; i < rows; i++){
                temp = arr[j][i];
                if (temp <= max && temp >= min){
                    temp += (int) (21.0 * ((double) rand()/(RAND_MAX+1.0)) - 10);
                    //temp += id; //debugging purposes
                    arr[j][i] = temp;
                    cont = 1;
                }
            }
        }

        // update global continue if we still have data within threshold
        /*if (cont == 1){
            pthread_mutex_lock(&lock);
            global_cont = 1;
            pthread_mutex_unlock(&lock);
        }

        pthread_barrier_wait(&barrier);
        if (global_cont != 1){
        	break;
        }

        // display grid if thread 0
        if (id == 0){
        	if (sim.freq != 0 && loop % sim.freq == 0){
        		printf("Count %d\n", loop);
        		printf("------------\n");
        		printArray(&sim);
        		printf("------------\n");
        	}
        }
        pthread_barrier_wait(&barrier);

        // thread 0 will reset the global cont to 0
        // should avoid race conditions because of the
        // barrier wait in line 219
        if (id == 0){
        	global_cont = 0;
        }*/
	}
	return NULL;
}

void clean(simulation* sim){
    int** arr = sim->arr;
    for (int i = 0; i < sim->cols; i++){
        free(arr[i]);
    }
    free(arr);
}

int main(int argc, char* argv[]){

	if (argc != 7){
        fprintf(stderr, "Usage: %s iterations threshold frequency seeed threads verbosity\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    simulation* pSim = &sim;

    if (parseArgs(pSim, 1, argv) == FALSE){
        fprintf(stderr, "Error: Input is not numbers");
        exit(EXIT_FAILURE);
    }

    getMatFromUser(pSim);

    // seed for our random number generation
    if (sim.seed < 0){
        srand(time(NULL));
    }
    else {
        srand(sim.seed);
    }

    pthread_t threads[sim.threads];
    thread_args args[sim.threads];

    if (pthread_barrier_init(&barrier, NULL, sim.threads) != 0){
    	fprintf(stderr, "Error: pthread_barrier_init failed");
        exit(EXIT_FAILURE);
    }

    int start_col = 0;
    int end_col = 0;
    int cols_per_thread = sim.cols / sim.threads;

    if (cols_per_thread <= 0){
    	fprintf(stderr, "Error: threads exceed columns in matrix; reduce # of threads");
        exit(EXIT_FAILURE);
    }

    int rem = sim.cols % sim.threads;

    for (int i = 0; i < sim.threads; i++){
    	// distributes the remainder among the threads
    	end_col = start_col + cols_per_thread-1 + (rem > 0 ? 1 : 0);

    	args[i].id = i;
    	args[i].start_col = start_col;
    	args[i].end_col = end_col;

    	start_col = end_col+1;
    	rem -= 1;
    }

    // get start time
    struct timeval myTVstart, myTVend;
    gettimeofday(&myTVstart, NULL);

    // run threads
    for (int i = 0; i < sim.threads; i++){
    	if (pthread_create(&threads[i], NULL, iter, (void*)&args[i]) != 0){
    		fprintf(stderr, "Error: pthread_create failed");
    		exit(EXIT_FAILURE);
    	}
    }

    for (int i = 0; i < sim.threads; i++){
    	pthread_join(threads[i], NULL);
    }

    gettimeofday(&myTVend, NULL);
    TIME_DIFF* difference = my_difftime(&myTVstart, &myTVend);

    //printf("final\n");
    //printf("------------\n");
    //printArray(pSim);
    //printf("------------\n");

    if (sim.verbose == 2){
        printf("\n");
        printf("time: sec %3d  microsec %6d \n", difference->secs, difference->usecs);
    }

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&lock);
    clean(pSim);
    free(difference);

	return 0;
}
