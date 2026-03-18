#include <stdio.h>
#include <string.h>
#include <mpi.h>


int main(){

    MPI_Init(NULL, NULL);


    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char str[1024] = { 0 };

    if (rank == 0) {

        printf("Hello from process %d of %d\n", rank, size);

        int total = 0;

        for (int i = 1; i < size; i++){
            MPI_Recv(str, 1024, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", str);

            int temp = 0;
            MPI_Recv(&temp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total += temp;
        }

        printf("Combined values sent: %d\n", total);
    }
    else {
        snprintf(str, 1023, "Hello from process %d of %d", rank, size);

        MPI_Send(str, strlen(str)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        int value_to_send = rank*2;
        MPI_Send(&value_to_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }



    MPI_Finalize();
    return 0;
}