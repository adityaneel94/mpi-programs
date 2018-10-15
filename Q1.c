#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int prime_test (int N, int rank, int size)
{
    int h = N / 2;
    int k = h / size + 1;
    int upper = (rank + 1) * k + 1;
    int lower = rank * k + 2;

    upper  = (upper > h) ? h : upper; 

    lower = (rank == 0) ? 2 : lower;

    int i = lower;
    while(i <= upper){
        if (N % i == 0)
            return 1;
        i++;
    }
    return 0;
}

int main (int argc, char** argv)
{
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int myid, numprocs, processor_name_length;
    
    MPI_Init (0, 0);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Get_processor_name (processor_name, &processor_name_length);

    int N, i, result = 0;

    // Input N on master
    if (!myid) 
        scanf("%d", &N);

    if (!myid) {
        // Send number to all slaves
        i = 1;
        while(i < numprocs){
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            i++; 
        }

        result = prime_test (N, myid, numprocs);

        // Get results from slaves
        i = 1;
        int result_slave = 0;
        while(i < numprocs){
            MPI_Recv(&result_slave, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            result = result + result_slave;
            i++;
        }

        // Print message
        (result == 0)? printf("%d is prime.\n",N) : printf("%d is not prime.\n",N);             
    
    } 
    else{
        // Recieve number from master
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // test for prime
        result = prime_test (N, myid, numprocs);

        // send result to master
        MPI_Send(&result, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}