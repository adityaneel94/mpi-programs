#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int totalNumbers;
int eachProcessArraySize;
int lastProcessGet;

/* merge sort function referred online */
void merge(int *a, int *b, int l, int m, int r) {

	int h = l, i = l, j = m+1, k;

	while((h <= m) && (j <= r)){
		if(a[h] <= a[j]) 
			b[i++] = a[h++];
		else
			b[i++] = a[j++];
	}
	if(m < h) {
		k = j;
		while(k <= r)
			b[i++] = a[k++];
	}
	else {
		k = h;
		while(k <= m)
			b[i++] = a[k++];
	}

	k = l;
	while(k <= r){
		a[k] = b[k];
		k++;
	}		
}

void mergeSort(int *a, int *b, int l, int r) {

	int m;

	if(l < r) {
		m = (l + r) / 2;
		mergeSort(a, b, l, m);
		mergeSort(a, b, (m + 1), r);
		merge(a, b, l, m, r);
	}
}

void mergeFinal(int *a, int m, int *b, int n, int *sorted) {
	int i = 0, j = 0, k = 0;

	while(i < m + n){
		if (j < m && k < n) {
			if (a[j] < b[k]) 
				sorted[i++] = a[j++];
			else
				sorted[i++] = b[k++];
		}
		else if (j == m) {
			for (; i < m + n;) 
				sorted[i++] = b[k++];
		}
		else {
			for(; i < m + n;)
				sorted[i++] = a[j++];
		}
	}
}

int main(int argc, char **argv) 
{
	int myrank, nprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	if(!myrank){

		int i=0, j=0;
		int arr[10000];
		char temp;

		while(1){
			scanf("%d%c", &arr[i], &temp);
			i++;
			if(temp == '\n')
				break;
		}

		totalNumbers = i;

		lastProcessGet = (totalNumbers / (nprocs-1)) + (totalNumbers % (nprocs-1));

		eachProcessArraySize = totalNumbers / (nprocs-1);

		int inner,cnt = 1;
		int dataToSend[nprocs + 1][lastProcessGet];

		while(cnt < nprocs - 1){
			
			inner  = 0;
			while(inner < eachProcessArraySize){
				dataToSend[cnt][inner] = arr[(cnt-1)*eachProcessArraySize + inner];
				inner++;				
			}

			MPI_Send(&eachProcessArraySize,1, MPI_INT, cnt, cnt, MPI_COMM_WORLD);
			MPI_Send(dataToSend[cnt], eachProcessArraySize, MPI_INT, cnt, cnt, MPI_COMM_WORLD);
			cnt++;
		}

		inner  = 0;
		while(inner < lastProcessGet){
			dataToSend[cnt][inner] = arr[(cnt - 1) * eachProcessArraySize + inner];
			inner++;				
		}

		MPI_Send(&lastProcessGet,1, MPI_INT, cnt, cnt, MPI_COMM_WORLD);
		MPI_Send(dataToSend[cnt], lastProcessGet, MPI_INT, cnt, cnt, MPI_COMM_WORLD);

		int tempAnswerSize = 0;
		int finalAnswer[totalNumbers], tempAnswer[totalNumbers];

		i = 1;
		int h = 0; 

		while(i < nprocs){
			int sizeReceived;
			int sortedResult[sizeReceived];
			MPI_Recv(&sizeReceived,1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			MPI_Recv(sortedResult,sizeReceived, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			mergeFinal(tempAnswer, tempAnswerSize, sortedResult, sizeReceived, finalAnswer);

			tempAnswerSize =  tempAnswerSize + sizeReceived;

			h = 0;
			while(h < tempAnswerSize){
				tempAnswer[h] = finalAnswer[h];
				h++;
			}

			i++;
		}

		h = 0;
		while(h < totalNumbers){
			printf("%d ",finalAnswer[h]);
			h++;
		}			
	}
	else
	{
		int sizeReceived;
		int dataReceived[sizeReceived];

		MPI_Recv(&sizeReceived,1, MPI_INT, 0, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		MPI_Recv(dataReceived,sizeReceived, MPI_INT, 0, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int *tmp_array = malloc(sizeReceived * sizeof(int));
		mergeSort(dataReceived, tmp_array, 0, (sizeReceived - 1));

		MPI_Send(&sizeReceived,1, MPI_INT, 0, myrank, MPI_COMM_WORLD);
		MPI_Send(dataReceived, sizeReceived, MPI_INT, 0, myrank, MPI_COMM_WORLD);

		for(;;){}
	}
	return 0;
}