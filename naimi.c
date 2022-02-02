#include <stdlib.h> 
#include <stdio.h> 
#include <mpi.h> 
#include <pthread.h> 
#include <string.h> 
#include <unistd.h>

//openMP mem centrale partage 
//openMPI mem distribue
// MPI_THREAD_MULTIPLE : possibilite de recv et send msg qlq soit le thread
// MPI : only le processus principale

   pthread_t T[2];
   int ERt1, ERt2, size, rank, message[2], Privilege = 0, demandeur = 0, dernier, suivant;
   void* sending(void *arg) {
   printf("\n Thread 1 from rank : %d started\n",rank);
   demandeur = 1; int tag = rank;
   if(dernier != -1) {
   MPI_Send(0, 2, MPI_INT, dernier, tag , MPI_COMM_WORLD); // envoie requete
   dernier = -1; 
   }
    while(Privilege != 1); //attendre privilege 1
    printf("Thread 1 from rank : %d : je suis dans la section critique \n", rank);
    sleep(10); //SC
    demandeur = 0;

    if(suivant != -1) 
     {
      Privilege = 0;
      MPI_Send(1, 2, MPI_INT, suivant, tag, MPI_COMM_WORLD); //envoie jeton
      suivant = -1; 
     }  
     printf("\n Thread 1 from rank : %d finished \n",rank);
     return NULL; 
    } 

     void* receiving(void *arg) 
     {
     printf("\n Thread 2 from rank : %d started\n",rank);
     int tag = rank;
     MPI_Recv(&message, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG , MPI_COMM_WORLD, MPI_STATUS_IGNORE); // envoie requete
    if(message == 0) { // requete
    if(dernier == -1) 
    {
    if(demandeur == 1)
    { suivant = tag; }
    else {
       MPI_Send(1, 2, MPI_INT, suivant, tag , MPI_COMM_WORLD);
       Privilege = 0; }
   } 

   else { 
   MPI_Send(0, 2, MPI_INT, dernier, suivant , MPI_COMM_WORLD); }
   dernier = message[1];
        } 
else //jeton { Privilege = 1; }ENDELSE
printf("\n Thread 2 from rank : %d finished\n",rank);
return NULL;
        }
 
int main(int argc, char * argv[]) 
{
       int provided;
       MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
       if (provided != MPI_THREAD_MULTIPLE)
       { 
      printf("\n Warning MPI did not provide MPI_THREAD_MULTIPLE \n"); }
      MPI_Comm_rank(MPI_COMM_WORLD,&rank);
      MPI_Comm_size(MPI_COMM_WORLD,&size);
      ERt1 = pthread_create(&(T[0]), NULL, &sending, NULL);
      ERt2 = pthread_create(&(T[1]), NULL, &receiving, NULL);

      if (ERt1 != 0) printf("\nError creating the thread 1: [%s]", strerror(ERt1));
      if(ERt2 != 0) printf("\nError creating the thread 2: [%s]", strerror(ERt2));
      
//MISSING SECTION
pthread_join(T[0], NULL); pthread_join(T[1], NULL);
       
MPI_Finalize(); 

return 0; 
}
