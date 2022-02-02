#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


printf("Hello I am the Process with rank %d pair and size %d \n",rank, size);

	  //MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0,
		  // MPI_COMM_WORLD);
	
	  //  MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD,
		   //  MPI_STATUS_IGNORE);
	  //  printf("Process %d received token %d from process %d\n", world_rank, token,
		//   world_size - 1);
  	  //}
//}


  MPI_Finalize();
}