#include <stdio.h>
#include<stdlib.h>
#include <mpi.h>
#include <unistd.h>

int main(int argc, char** argv) 
{
	int myrank, nprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Request request[nprocs + 1];
	MPI_Status status;
	int number;
	int dataReceived[3];
	int dataToSend[nprocs + 1][3];
	int colored[1000];
	int weights[1000];
	int graph[1000][1000];
	int maximum_color_number = 0;
	int vertices, edges;

	int l = 0;
	for(l = 0; l < 1000; l++)
		colored[l] = -1;


	if (myrank == 0) 
	{
		scanf("%d",&vertices);
		scanf("%d",&edges);
		int i, j;

		for(i = 0; i < vertices; i++)
		{
			weights[i] = rand() % 13 + rand() % 17 + rand() % 23 + rand() % 29;
			colored[i] = -1;
		}

		int from, to;
		for(i = 0; i < edges; i++)
		{	
			scanf("%d %d",&from, &to);
			graph[from][to] = 1;
			graph[to][from] = 1;
		}
		
		int max = -1;

		for(i = 0; i < vertices; i++)
		{	
			max = 0;
			for(j = 0; j < vertices; j++)
			{
				if(graph[i][j] != 0)
					max++;
			}
			if(max > maximum_color_number)
				maximum_color_number = max;
		}

		maximum_color_number += 1;

		int each_process_get_vertices = vertices / (nprocs - 1);
		int last_process_get_vertices = vertices / (nprocs - 1) + (vertices % (nprocs - 1));
		int fromVertex = 0, toVertex;

		for(i = 1; i < nprocs - 1; i++)
		{
			MPI_Send(&vertices, 1, MPI_INT, i, i, MPI_COMM_WORLD);
			int k = 0;
			for(k = 0; k < vertices; k++)
			{
				MPI_Send(&graph[k], vertices, MPI_INT, i, i, MPI_COMM_WORLD);
			}
			
			toVertex = fromVertex + each_process_get_vertices - 1;
			dataToSend[i][0] = fromVertex;
			dataToSend[i][1] = toVertex;
			dataToSend[i][2] = maximum_color_number;
			MPI_Send(dataToSend[i], 3, MPI_INT, i, i, MPI_COMM_WORLD);
			MPI_Send(colored, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
			MPI_Send(weights, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
			fromVertex = toVertex + 1;
		}
		MPI_Send(&vertices, 1, MPI_INT, i, i, MPI_COMM_WORLD);

		int k = 0;
		for(k = 0; k < vertices; k++)
			MPI_Send(&graph[k], vertices, MPI_INT, i, i, MPI_COMM_WORLD);

		toVertex = fromVertex + last_process_get_vertices - 1;
		dataToSend[i][0] = fromVertex;
		dataToSend[i][1] = toVertex;
		dataToSend[i][2] = maximum_color_number;
		MPI_Send(dataToSend[i], 3, MPI_INT, i, i, MPI_COMM_WORLD);
		MPI_Send(colored, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
		MPI_Send(weights, vertices, MPI_INT, i, i, MPI_COMM_WORLD);
		
		for(i = 1; i <= nprocs - 1; i++)
			MPI_Recv(colored, vertices, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		max = -1;
		for(i = 0; i < vertices; i++)
		{
			if(colored[i] > max)
				max = colored[i];
		}
		printf("%d\n",max + 1);
		for(i = 0; i < vertices; i++)
			printf("%d\n",colored[i]);	
		
	} 
	else
	{
		int vertices;
		MPI_Recv(&vertices, 3, MPI_INT, 0, myrank, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		int graph[vertices][vertices];

		int i, j;
		for(i = 0; i < vertices; i++)
			MPI_Recv(&graph[i], vertices, MPI_INT, 0, myrank, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		MPI_Request receiver_request;
		MPI_Status receiver_status;

		MPI_Recv(dataReceived, 3, MPI_INT, 0, myrank, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		int colored[vertices], weights[vertices];
		MPI_Recv(colored, vertices, MPI_INT, 0, myrank, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(weights, vertices, MPI_INT, 0, myrank, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		int current_vertex;
		int round = 0;
		while(round < vertices)
		{
			int all_colored = 1;
			for(current_vertex = dataReceived[0]; current_vertex <= dataReceived[1]; current_vertex++)
			{
				if(colored[current_vertex] == -1)
				{
					all_colored = 0;

					int my_weight = weights[current_vertex]; 

					int is_my_weight_highest = 1;

					int neighbours;

					for(neighbours = 0; neighbours < vertices; neighbours++)
					{
						if(neighbours != current_vertex)
						{
							if(graph[current_vertex][neighbours] == 1 && colored[neighbours] == -1 && weights[neighbours] > weights[current_vertex])
							{
								is_my_weight_highest = 0;
								break;
							}
						}
					}
					if(is_my_weight_highest)
					{
						int assigned_color = -1;
					
						for(assigned_color = 0; assigned_color < dataReceived[2]; assigned_color++)
						{
							int canBeGiven = 1;
							for(neighbours = 0; neighbours < vertices; neighbours++)
							{
								if(graph[current_vertex][neighbours] == 1 && colored[neighbours] != -1 && assigned_color == colored[neighbours])
								{
									canBeGiven = 0;
									break;
								}
							}
							if(canBeGiven == 1)
							{
								colored[current_vertex] = assigned_color;
								break;
							}
						}
					}
				}
			}
			int m;
			for(m = 1; m <= nprocs - 1;m++)
			{
				if(m != myrank)
					MPI_Send(colored, vertices, MPI_INT, m, m, MPI_COMM_WORLD);
			}
			
			for(m = 1; m <= nprocs - 1;m++)
			{
				int local_colors[vertices], local_weights[vertices];
				if(m != myrank)
					MPI_Recv(local_colors, vertices, MPI_INT, m, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				int cnt = 0;
				for(cnt = 0; cnt < vertices; cnt++)
				{
					if(colored[cnt] == -1 && local_colors[cnt] != -1 && local_colors[cnt] < dataReceived[2] && local_colors[cnt] >= 0)
						colored[cnt] = local_colors[cnt];
				}
			}
			int cnt;	
			round++;
			sleep(3);
		}
		MPI_Send(colored, vertices, MPI_INT, 0, myrank, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}