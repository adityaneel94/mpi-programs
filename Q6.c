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

	if (myrank == 0) 
	{
		int pairs;
		scanf("%d",&pairs);

		int dataToSend[1 + pairs * 2][pairs];
		int i, j;

		for(i = 1; i <= 2 * pairs;i++)
		{
			for(j = 0; j < pairs; j++)
			{
				scanf("%d",&dataToSend[i][j]);
			}
		}

		// 1 : MALE
		// 2 : FEMALE

		for(i = 1; i <= 2 * pairs; i++)
		{
			// send number of pairs
			MPI_Send(&pairs, 1, MPI_INT, i, i, MPI_COMM_WORLD);
			// prefrence list
			MPI_Send(dataToSend[i], pairs, MPI_INT, i, i, MPI_COMM_WORLD);
		}

		int marriages[2 * pairs + 1];

		for(i = 1; i <= 2 * pairs; i++)
			marriages[i] = 0;

		while(1)
		{
			int all_marriages_done = 1;
			for(i = 1; i  <= 2 * pairs; i++)
			{
				if(marriages[i] == 0)
				{
					all_marriages_done = 0;
					break;
				}
			}
			if(all_marriages_done == 1)
			{
				printf("\n ALL MARRIAGE DONE \n");
				for(i = 1; i <= pairs;i++)
				{
					printf("\n%d %d",i-1, marriages[i] - pairs - 1);
				}
				exit(0);
				break;
			}
			else
			{
				int data[2];
				MPI_Recv(data, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				marriages[data[0]] = data[1];
				marriages[data[1]] = data[0];
			}
		}
	} 
	else
	{
		int pairs;
		MPI_Recv(&pairs, 1, MPI_INT, 0, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int order[pairs];
		MPI_Recv(order, pairs, MPI_INT, 0, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int i;
		int currently_assigned = 0;
		int rejected_from[2 * pairs + 1];
		for(i = 1; i <= 2 * pairs; i++)
			rejected_from[i] = 0;

		// male
		if(myrank <= pairs)
		{
			while(1)
			{
				if(currently_assigned == 0)
				{
					for(i = 0; i < pairs; i++)
					{		
						// pick female not rejected from
						if(rejected_from[pairs + 1 + order[i]] == 0)
						{		
							printf("\n process %d sending proposal to %d",myrank, pairs + 1 + order[i]);

							// send proposal
							MPI_Send(&myrank, 1, MPI_INT, pairs + 1+ order[i], pairs + 1+ order[i], MPI_COMM_WORLD);

							int status;
							MPI_Recv(&status, 1, MPI_INT, pairs + 1 + order[i], pairs + 1 + order[i], MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							if(status == 1)
							{
								printf("\n %d process got staus = %d from process %d",myrank, status, pairs + 1 + order[i]);
								currently_assigned = pairs + 1 + order[i];
								break;
							}
							else
							{
								rejected_from[pairs + 1 + order[i]] = 1;
								printf("\n %d process got staus = %d from process %d",myrank, status, pairs + 1 + order[i]);
							}
						}
					}
				}
				else
				{
					// listen for breakup
					int status;
					MPI_Recv(&status, 1, MPI_INT, currently_assigned, currently_assigned, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

					printf("\n %d process got breakup from %d",myrank, currently_assigned);
					currently_assigned = 0;
					rejected_from[currently_assigned] = 1;
				}
			}
		}
		else // female
		{
			while(1)
			{
				// listen for proposal
				int male;
				MPI_Recv(&male, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				printf("\n %d process got proposal from %d process",myrank, male);

				printf("\n currently assigned for female process %d = %d",myrank, currently_assigned);
			
				if(currently_assigned == 0)
				{
					currently_assigned = male;
					int status = 1;
					
					// accept proposal
					MPI_Send(&status, 1, MPI_INT, male, myrank, MPI_COMM_WORLD);
					printf("\n female %d assigning to %d ",myrank, male);
					int data[2];
					data[0] = male;
					data[1] = myrank;
					MPI_Send(data, 2, MPI_INT, 0, myrank, MPI_COMM_WORLD);
				}
				else
				{
					printf("\n for female %d, there is conflict",myrank);
					int pos1, pos2;
					for(i = 0; i < pairs; i++)
						if(order[i] == currently_assigned - 1)
							pos1 = i;
				
					for(i = 0; i < pairs; i++)
						if(order[i] == male - 1)
							pos2 = i;

					printf("\n for process %d , currently assigned pos1 = %d, new req pos2 = %d",myrank, pos1, pos2);


					if(pos1 < pos2)
					{
						// send reject to male
						int status = 0;
						MPI_Send(&status, 1, MPI_INT, male, myrank, MPI_COMM_WORLD);
						printf("\n female %d rejecting %d",myrank, male);
					}	
					else
					{
						// accept this and reject currently assigned
						int status = 1;
						MPI_Send(&status, 1, MPI_INT, male, myrank, MPI_COMM_WORLD);
						printf("\n female %d accepting %d",myrank, male);
						int data[2];
						data[0] = male;
						data[1] = myrank;
						MPI_Send(data, 2, MPI_INT, 0, myrank, MPI_COMM_WORLD);
						status = 0;
						MPI_Send(&status, 1, MPI_INT, currently_assigned, myrank, MPI_COMM_WORLD);
						printf("\n female %d rejecting %d",myrank, currently_assigned);
						currently_assigned = male;
					}
				}
			} // while true
		} // else female
	} // else

	MPI_Finalize();
	return 0;
}
