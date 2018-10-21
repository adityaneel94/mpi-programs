#include<stdio.h>
#include<mpi.h>
#define MAX 50

int dfs(int start,int n,int gph[MAX][MAX],int k){
    if(k == 0)
        return 1;
    
    int tc = 0,i = 1;
    
    for(;i <= n;++i){
        if(gph[start][i] == 1)
            tc += dfs(i,n,gph,k-1);
    }

    return tc;
}

int main(int argc, char** argv){

    int numprocs,rank;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Status status;

    int tag1 = 5, tag2 = 10;
    int totalWalks = 0;
    int gph[MAX][MAX] = {0};

    //master
    if(rank == 0){
        int n,e,k;
        scanf("%d %d %d",&n,&e,&k);
        
        //Populate the graph
        int s,d;
        for(int i = 0;i<e;i++){
            scanf("%d %d",&s,&d);
            gph[s][d] = 1;
        }

        //Calculate nodes per process
        int nodesPerProcess = n/(numprocs-1);
        int leftOverNodes = n % (numprocs - 1);

        //mapping
        int lower_bound = 1,upper_bound = nodesPerProcess;
        
        if(leftOverNodes != 0){
            upper_bound = leftOverNodes;

            //Offset nodes' walks will be calculated by the master process
            for(int node = lower_bound; node <= upper_bound; ++node)
                totalWalks += dfs(node,n,gph,k);

            lower_bound = upper_bound + 1;
            upper_bound = lower_bound + nodesPerProcess - 1;
        }

        //Send paramenters to respective slave processes
        for(int dest = 1;dest < numprocs;dest++){
            MPI_Send(&lower_bound, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            MPI_Send(&upper_bound, 1, MPI_INT, dest, tag1 + 1, MPI_COMM_WORLD);
            MPI_Send(&k, 1, MPI_INT, dest, tag1 + 2, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, dest, tag1 + 3, MPI_COMM_WORLD);
            lower_bound = upper_bound + 1;
            upper_bound +=  nodesPerProcess;
        }
    }   


    //Broadcast the graph to all the nodes
    MPI_Bcast(&gph, MAX * MAX, MPI_INT, 0, MPI_COMM_WORLD);
    
    //Slave process
    if(rank > 0){

        //Recieve respective parameters from the master process
        int lower_bound,upper_bound,k,n;
        MPI_Recv(&lower_bound, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, tag1 + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&k, 1, MPI_INT, 0, tag1 + 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&n, 1, MPI_INT, 0, tag1 + 3, MPI_COMM_WORLD, &status);
        int localWalks = 0;

        //Calculate walks for each node in the given range
        for(int node = lower_bound;node <= upper_bound;node++){
            int x = dfs(node,n,gph,k);
            localWalks += x;
        }
        
        //Send total walks calculated by the slave to the master
        MPI_Send(&localWalks, 1, MPI_INT, 0, tag2, MPI_COMM_WORLD);
    }

    //Master
    if(rank == 0){

        //Recieve walks from respective slaves
        int x;
        for(int src = 1;src < numprocs;++src){
            MPI_Recv(&x, 1, MPI_INT, src, tag2, MPI_COMM_WORLD, &status);
            totalWalks += x;
        }
        printf("%d\n",totalWalks);
    }

    MPI_Finalize();
    return 0;  
}