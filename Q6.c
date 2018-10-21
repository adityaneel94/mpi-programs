#include<stdio.h>
#include<mpi.h>
#define MAX 50

//The function checks if there is a cycle from curnode back to the node itself
void dfs(int graph[MAX][MAX],int vis[MAX],int start,int curnode,int cycle_len,int *cnt,int V){
    vis[curnode] = 1;
    if(cycle_len == 0){
        vis[curnode] = 0;
        if(graph[curnode][start]){
            *cnt = *cnt + 1;
            return;
        }
        else
            return;
    }
    for(int i = 0;i < V;++i){
        if(!vis[i] && graph[curnode][i]){
            dfs(graph,vis,start,i,cycle_len-1,cnt,V);
        }
    }
    vis[curnode] = 0;
}

int main(int argc, char** argv){

	int numprocs,rank;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    MPI_Status status;

    int graph[MAX][MAX] = {0};
	int vis[MAX] = {0};
	int tag1 = 5, tag2 = 10;
	int totalCycles = 0;

    if(rank == 0){
    	int V,e;
    	int n = 4;
	    scanf("%d %d",&V,&e);
	    
	    //populate the graph
	    int s,d;
	    for(int i = 0;i < e;i++){
	        scanf("%d %d",&s,&d);
	        graph[s][d] = 1;
	        graph[d][s] = 1;
	    }
	    
	    int nodesPerProcess = (V-(n-1))/(numprocs-1);
        int leftOverNodes = (V-(n-1)) % (numprocs - 1);

        int lower_bound = 0,upper_bound = nodesPerProcess - 1;
        
        if(leftOverNodes != 0){
            upper_bound = leftOverNodes - 1;

            //Offset nodes' cycles will be calculated by the master process
            int cnt = 0;
            for(int node = lower_bound; node <= upper_bound; ++node){
                dfs(graph,vis,node,node,n-1,&cnt,V);
                vis[node] = 1;
            }
            totalCycles += cnt;

            lower_bound = leftOverNodes;
            upper_bound += nodesPerProcess;
        }

        for(int dest = 1;dest < numprocs;dest++){
            MPI_Send(&lower_bound, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
            MPI_Send(&upper_bound, 1, MPI_INT, dest, tag1 + 1, MPI_COMM_WORLD);
            MPI_Send(&V, 1, MPI_INT, dest, tag1 + 2, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, dest, tag1 + 3, MPI_COMM_WORLD);
            lower_bound = upper_bound + 1;
            upper_bound +=  nodesPerProcess;
        }
    }

    //Broadcast the graph and visited array to all the nodes
    MPI_Bcast(&graph, MAX * MAX, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vis,MAX,MPI_INT,0,MPI_COMM_WORLD);

    //Slave process
    if(rank > 0){

        //Recieve respective parameters from the master process
        int lower_bound,upper_bound,V,n;
        MPI_Recv(&lower_bound, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, tag1 + 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&V, 1, MPI_INT, 0, tag1 + 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&n, 1, MPI_INT, 0, tag1 + 3, MPI_COMM_WORLD, &status);
        int localWalks = 0;

        //Calculate cycles starting from 'node' in the given range
        int cnt = 0;
        for(int node = lower_bound;node <= upper_bound;node++){
            dfs(graph,vis,node,node,n-1,&cnt,V);
            vis[node] = 1;
        }  
     
        //Send total cycles calculated by the slave to the master
        MPI_Send(&cnt, 1, MPI_INT, 0, tag2, MPI_COMM_WORLD);
    }
    
    //Master
    if(rank == 0){

        //Recieve cycle counts from respective slaves
        int x;
        for(int src = 1;src < numprocs;++src){
            MPI_Recv(&x, 1, MPI_INT, src, tag2, MPI_COMM_WORLD, &status);
            totalCycles += x;
        }
        printf("%d\n",totalCycles/2);
    }

    MPI_Finalize();
    return 0;
}