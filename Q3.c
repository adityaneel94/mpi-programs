#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int calculateArea(int points[][2], int n){
	int ans = 0,i = 0;
    if(n <= 1)
        return 0;   
    
    while(i < n-1){
        ans = ans + (points[i][0] * points[i + 1][1] - points[i + 1][0] * points[i][1]);
        i++;
    }
    return ans;
}

int main (int argc, char** argv)
{   
    int myid, numprocs,n;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);

    if (!myid) {
        scanf("%d",&n);

        int arr[n + 1][2], i = 0;
        while(i < n){
            scanf("%d%d",&arr[i][0],&arr[i][1]);
            i++;
        }    
        
        arr[n][1] = arr[0][1];
        arr[n][0] = arr[0][0];

        //calculations
        i = 1;
        int totalPairs = n + 1;

        while(i < numprocs){
            int share = (totalPairs + numprocs - 3) / (numprocs - 1);
            int end = i * share;
            int start = (i - 1) * share;

            end = (i == numprocs - 1)? (totalPairs - 1) : end;

            int arrToSend[end - start + 1][2];
            int len = end - start + 1;

            MPI_Send(&len,1 , MPI_INT, i, 0, MPI_COMM_WORLD);

            int j = start, k = 0;
            while(j <= end){
            	arrToSend[k][1] = arr[j][1];
                arrToSend[k][0] = arr[j][0];
                ++j; ++k;
            }

            MPI_Send(&arrToSend,len*2 , MPI_INT, i, 1, MPI_COMM_WORLD);
            i++;
        }

        i = 1;
        int totalArea = 0;
        while(i < numprocs){
            int area = 0;
            MPI_Recv(&area, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalArea = totalArea + area;
            i++;
        }

        totalArea = abs(totalArea) / 2;
        printf("%d\n",totalArea);
    }

    else{
        int n;
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int arrRecv[n][2];
        MPI_Recv(&arrRecv, n*2, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int area = calculateArea(arrRecv,n);
        MPI_Send(&area, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

