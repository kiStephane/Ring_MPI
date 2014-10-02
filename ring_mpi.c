/*
  Compile with
    mpicc -O3 -std=c99 ring_mpi.c -o ring
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define K 1024            /* One Kilobyte */
#define M 1000*K          /* One Megabyte */
#define MAXSIZE 1000*M    /* 1000 Megabytes = 1 GB */

int main(int argc, char* argv[]) {
  int np, id, err;
  int messageSize = -1;    
  char* buffer;
    int tag = 777;
  MPI_Status status;
  double startTime, endTime;

  /* Initialize the messageSize with program argument*/
  if (argc > 1){
    messageSize= atoi(argv[1]);
  }

  /* Initialize MPI */
  err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS) {
        printf("MPI_init failed!\n");
        exit(1);
    }
        
  err = MPI_Comm_size(MPI_COMM_WORLD, &np);
    if (err != MPI_SUCCESS) {
        printf("MPI_Comm_size failed!\n");
        exit(1);
    }
    
  err = MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if (err != MPI_SUCCESS) {
        printf("MPI_Comm_rank failed!\n");
        exit(1);
    }

  /* Allocate a sufficiently large message buffer */
  buffer = (char*) malloc(MAXSIZE*sizeof(char));
  memset(buffer, 1, MAXSIZE);   // Initialize the buffer to all ones

    /* Check that we run on at least two processors */
    if (np < 2) {
        printf("You have to use at least 2 processes to run this program\n");
        MPI_Finalize();	       /* Quit if there is only one processor */
        exit(0);
    }
    
  if (id == 0) {
    printf("Number of processors is %d\n", np);
    
    /* Read the messageSize from user input  if not specified 
    as an argument of the program*/
    if(messageSize==-1){
      printf("Please give an input size in bytes: \n");
      fflush(NULL);
      scanf("%d", &messageSize);  
    }
    
    startTime = MPI_Wtime();      // Start measuring time
  
    if (messageSize > MAXSIZE) {
      printf("Sorry, that's too much memory!\n\n");
      messageSize = 0;
    }
    printf("Message size is %d\n", messageSize);
      
    printf("Process %d sending to process all\n", id);
	  MPI_Bcast(&messageSize,1, MPI_INT, 0,MPI_COMM_WORLD );

    char * message;
    message = (char*) malloc(messageSize*sizeof(char));
    memset(message,7,messageSize);

    err=MPI_Send(message, messageSize, MPI_CHAR, 1, tag,
               MPI_COMM_WORLD);    
    if(err!=MPI_SUCCESS){
    printf("Error is MPI_Send in process 0 ");
    }

    err = MPI_Recv(buffer,messageSize, MPI_CHAR,np-1,tag, MPI_COMM_WORLD,&status);
    if (err!=MPI_SUCCESS){
    printf("Erro in MPI_Recv in process %d\n", id);
    }

    printf("Process 0 received message from %d\n", np-1);
    endTime = MPI_Wtime();      // Stop measuring time
    printf("Time: %f s\n", endTime-startTime);
    free(message);

  } else{
    int next=(id+1)%np;
    err= MPI_Bcast(&messageSize, 1, MPI_INT, 0, MPI_COMM_WORLD);    
    if(err!= MPI_SUCCESS){
    printf("Error in MPI_Recv messageSize in process %d\n", id);
    exit(1);
    }     

    err=MPI_Recv (buffer, messageSize, MPI_CHAR, id-1, tag, MPI_COMM_WORLD, &status);  
    if(err!=MPI_SUCCESS){
    printf("Error in MPI_Recv buffer in process %d\n", id);
    }
    //printf("Process %d received %d from %d\n",id,buffer[0],id-1);

    MPI_Send(buffer, messageSize, MPI_CHAR, next,tag,MPI_COMM_WORLD);
    }
  
    free(buffer);
    MPI_Finalize();
    exit(0); 
}

