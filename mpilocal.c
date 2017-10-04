#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define NUM_MAX 100000

int main(int argc, char** argv) {
  /*** Initialize the MPI environment ***/
  MPI_Status status;
  MPI_Init(NULL, NULL);
  MPI_Comm newcomm;
  /*** Getting world size and rank ***/
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  /*** Creating new virtual topology ***/ 
  int dims = world_size;
  int qperiodic= 1;
  int reorder = 1;
  MPI_Dims_create(world_size, 1, &dims);
  MPI_Cart_create(MPI_COMM_WORLD, 1, &dims, &qperiodic, reorder, &newcomm);
  /*** Getting new world rank ***/
  MPI_Comm_rank(newcomm, &world_rank);

  /*** Variables and memory pointers***/
  int n,i;
  float sum = 0.0;
  float total = 0.0;
  float *numbers;
  float *buf;

  printf("World size is %d and world rank is %d\n\n", world_size, world_rank);
  srand(time(0));
  /*** Broadcast and receive ***/
  numbers = (float *)malloc(sizeof(float) * NUM_MAX);
  buf = (float *)malloc(sizeof(float) * NUM_MAX);
  if(world_rank == 0) {
    // If we are rank 0, set the number to -1 and send it to process 1

    for(i=0; i<NUM_MAX; i++)
    {
	numbers[i] = (rand() / (float)RAND_MAX)*10;
	//printf("  %d,", numbers[i]);
    }
	MPI_Bcast(numbers, NUM_MAX, MPI_INT, 0, newcomm);
  }
  else {

	MPI_Bcast(buf, NUM_MAX, MPI_INT ,0 , newcomm);
  }
 // printf("Broadcast successful\n\r");

  /*** Summing in individal processes ***/
  if(world_rank != 0)
  {
	int from = (world_rank - 1)*(NUM_MAX/(world_size - 1));
	int to = world_rank * (NUM_MAX/(world_size - 1));
	for(i = from; i < to; i++)
	{
		sum = sum + buf[i];
	}
	printf("Sum from %d is %f\n", world_rank, sum);
  }
 	MPI_Reduce(&sum, &total, 1, MPI_FLOAT, MPI_SUM, 0, newcomm);

  /*** Collecting all sums at root node and finding the total sum ***/
  if(world_rank == 0)
  {
  	printf("The total sum is %f\n", total);
  }
  
  free(numbers);
  free(buf);
  MPI_Finalize();
}

