#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if ( size < 2 || size % 2 != 0){
        fprintf(stderr, "The # of processes must be an even number >= 2.\n");
        return 1;
    }

    int num_steps = atoi(argv[1]);
    if (num_steps <= 0){        
        fprintf(stderr, "%s must be a number greater than 0.\n", num_steps);
        return 1;
    }

    // variables to determine whether we are sending or recv
    int window_size  = size/2;
    int start_window = 0;
    int end_window   = window_size - 1;
    int is_sender    = 0;

    // value exchange
    int partner = (rank + window_size) % size;
    long val = rank;

    for (int i = 0; i < num_steps; i++) {
        
        is_sender = 0;

        // determines if we are sending or recv
        if (rank >= start_window && rank <= end_window) {
            is_sender = 1;
        }
        // example window 8,9,0,1
        if (end_window < start_window) {
            if (rank <= end_window && start_window > rank) {
                is_sender = 1;
            }
            if (rank >= start_window && rank > end_window ) {
                is_sender = 1;
            }
        }

        long temp;
        if (is_sender) {
            temp = (rank+1)*i;
            MPI_Send(&temp, 1, MPI_LONG, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(&temp, 1, MPI_LONG, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            val += temp;
        }
        else {
            MPI_Recv(&temp, 1, MPI_LONG, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            val += temp;
            temp = (rank+1)*i;
            MPI_Send(&temp, 1, MPI_LONG, partner, 0, MPI_COMM_WORLD);
        }

        start_window = (start_window+1) % size;
        end_window = (start_window+window_size-1) % size;
    }

    if (rank > 0) {
        MPI_Send(&val, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
    }
    else {
        printf("Rank: %d Value: %d\n", rank, val);
        
        long other_vals;
        int n_recv = 1;
        MPI_Status status;

        while (n_recv < size) {
            MPI_Recv(&other_vals, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Rank: %d Value: %d\n", status.MPI_SOURCE, other_vals);

            n_recv += 1;
        }
    }

    MPI_Finalize();
    return 0;
}