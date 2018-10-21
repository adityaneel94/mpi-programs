#include<bits/stdc++.h>
#include"mpi.h"
#include <unistd.h>
using namespace std;

int main(int argc, char** argv){
    int myid, numprocs;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
    int n, e;
    if(myid == 0){
        //master
        int result = 0;
        cin >> n >> e;
        int graph[n][n];
        memset(graph, 0, sizeof(graph));
        
        for(int i = 0; i < e; i++){
            int u, v, w;
            cin >> u >> v >> w;
            graph[u][v] = w;
            graph[v][u] = w;
        }

        for(int  i = 1 ; i <numprocs; i++){
            MPI_Send(&n, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&e, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            MPI_Send(&graph, n*n, MPI_INT, i, 3, MPI_COMM_WORLD);
        }

        int share = (n/numprocs);
        if(share == 0)
            share = 1;
        
        for(int i = 1 ; i < numprocs; i++){
            int start = (i - 1) * share;
            int end = start + share;
            if(i == numprocs - 1)
                end = n - 1;
        }
        
        int count = 1;
        int checkVertex = 0;
        int minm = INT_MAX;
        int u,v;
        vector<vector<int> > ans;
        int cost = 0;

        while(count < n){
            for(int i = 1; i < numprocs ; i++){
                MPI_Send(&checkVertex, 1, MPI_INT, i, 4, MPI_COMM_WORLD);
            }

            int val1,val2,val3;
            for(int i = 1; i < numprocs; i++){
                MPI_Recv(&val1, 1, MPI_INT, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&val2, 1, MPI_INT, i, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&val3, 1, MPI_INT, i, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(minm > val1){
                    minm = val1;
                    u = val2;
                    v = val3;
                }
            }
                
            count++;
            if(minm == INT_MAX)
                continue;
            cost += minm;
            ans.push_back({u,v,minm});
            checkVertex = v;
            minm = INT_MAX;

        }
        cout<<"ans:"<<endl;
        cout<<cost<<endl;
        for(int i = 0; i < ans.size(); i++)
            cout<<ans[i][0]<<" "<<ans[i][1]<<" "<<ans[i][2]<<endl;
            
    }
    
    else{
        //slave
        MPI_Recv(&n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        MPI_Recv(&e, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
        int graph[n][n];
        MPI_Recv(&graph, n*n, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int share = (n/(numprocs-1));
        if(share == 0)
            share = 1;
        
        int start = (myid-1)*share;
        int end = start+share-1;
        if(myid == numprocs-1)
            end = n-1;
        
        vector <pair <int ,int > > distance(end-start+1, {INT_MAX, -1});
        vector<bool> visited(end-start+1, 0);
        int checkVertex;
        
        int mval = INT_MAX;
        int u,v;
        int count = 1;
        while(count < n){
            MPI_Recv(&checkVertex, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
            if(checkVertex >= start && checkVertex <= end){
                visited[checkVertex-start] = 1;
            }
            for(int i=0;i<(end-start+1);i++){
                if(visited[i]==0){
                    if(graph[checkVertex][start+i]!=0){
                        int edgeWeight = graph[checkVertex][start+i];
                        if(distance[i].first > edgeWeight){
                            distance[i].first = edgeWeight;
                            distance[i].second = checkVertex;
                        }
                    }
                    if(mval > distance[i].first){
                        mval = distance[i].first;
                        u = distance[i].second;
                        v = i+start;
                    }
                }
            }
            MPI_Send(&mval, 1, MPI_INT, 0, 5, MPI_COMM_WORLD);
            MPI_Send(&u, 1, MPI_INT, 0, 6, MPI_COMM_WORLD);
            MPI_Send(&v, 1, MPI_INT, 0, 7, MPI_COMM_WORLD);
            mval = INT_MAX;
            count++;
        }
    }
    MPI_Finalize();
}
