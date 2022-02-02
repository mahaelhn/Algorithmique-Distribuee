#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Function for finding elements which are there in table a[]  but not in b[]. 
int findMissing(int a[], int b[],  
                 int n, int m) 
{ 
    for (int i = 0; i < n; i++) 
    { 
        int j; 
        for (j = 0; j < m; j++) 
            if (a[i] == b[j]) 
                break; 
  
        if (j == m) 
            return a[i]; 
    } 
    return -1;
} 

//Find if an array contains an element
int findElementInArray(int el,int arr[],int arrSize){

    for (int i = 0; i < arrSize; i++)
    {
        if(arr[i]==el){
            return 1;
        }
    }
    return 0;
}
//Count occurence of a value in an array
int countElInArray(int el,int arr[],int arrSize){
    int nb=0;
    for (int i = 0; i < arrSize; i++)
    {
        if(arr[i]==el){
            nb++;
        }
    }
    return nb;

}
// Fonction pour enlever un élèment d'une liste
void removeElementFromArray(int array[], int *length, int element)
{
    int i, pos = -1;
    for (i = 0; i < *length; i++)
    {
        if (array[i] == element)
        {
            pos = i;
            break;
        }
    }

    // If the element was found
    if (pos != -1)
    {
        for (i = pos; i < *length - 1; i++)
        {
            array[i] = array[i + 1];
        }

        (*length)--;
    }
}

/*
    Messages utilisés (chacun son id):
        - 0: diffuser(t)
        - 1: retout(t)
        - 2: end() 
        Diffuser() : message envoyé par Pi à l’un de ses processus voisins Pj pour lui visiter.
        INIT() : message re¸cu par un processus quelconque lui demandant de
d´eclencher l’algorithme. Le processus l’ayant re¸cu devient l’initiateur.
 T= tableau
      
*/

// la fonction pour envoyer un message de type buffer 
void sendMessage(int type_mess,int t[],int sizeT, int dest, MPI_Comm comm)
{
    int buffer[sizeT+1];
    buffer[0] = type_mess;

   
    for (int i = 0; i <sizeT; i++)
    {
        buffer[i+1]=t[i];
    }
    
   

    MPI_Send(buffer, sizeT+1, MPI_INT, dest, 0, comm);
}



int main(int argc, char *argv[])
{
    srand(time(NULL));
    int wsize, pi,deja=0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    // Création du graphe

    int index[5] = {2,6,10,13,16};
    int edges[] = {1,2,0,2,3,4,0,1,3,4,1,2,4,1,2,3};

    MPI_Comm graph_comm;
    MPI_Graph_create(MPI_COMM_WORLD, wsize, index, edges, 1, &graph_comm);
    MPI_Comm_rank(graph_comm, &pi);

    // Variables locales

    int neighborsCount;
    MPI_Graph_neighbors_count(graph_comm, pi, &neighborsCount);
    int neighbors[neighborsCount]; 
    MPI_Graph_neighbors(graph_comm, pi, neighborsCount, neighbors);
    MPI_Status status;

    int buffer[wsize+1];

    int i;
    //number of lists to which pi belongs
    int nbList=0; 
    int t[wsize];
    int sizeT = wsize; 
    
   
    
    // A la récéption de INIT() par le processus 0
    if (pi == 0)
    {
        
        printf("Node %i: INIT.\n \n", pi);
        
        //diffusion a tous les voisins,Diffuser() : message envoyé par Pi à l’un de ses processus voisins Pj pour lui visiter.

        for ( int c = 0; c < neighborsCount; c++)
        {   
           
            for ( i = 0; i <sizeT; i++)
            {
                
                t[i]=0;
            }
            t[pi]=neighbors[c];
            
            sendMessage(0,t,sizeT,neighbors[c],graph_comm);
            nbList++;
            

        }
        
    }

    while (1)
    {

        MPI_Recv(buffer, wsize+1, MPI_INT, MPI_ANY_SOURCE, 0, graph_comm, &status);
        int pj = status.MPI_SOURCE;

       


        // A la reception de diffuser(Tableau) par pi depuis pj
        if (buffer[0] == 0)
        {
           
            //Copie des valeurs de T dans une variable locale
            for (int i = 0; i <sizeT; i++)
            {
                
                t[i]=buffer[i+1];
            }
             
            
            printf("%d =>  %d  ",pj,pi);
            for (int w = 0; w < sizeT; w++)
            {
                
                printf("%d",t[w]);
            }
            printf("\n");

           //si pi est l'initiateur (fin du chemin)
           if(pi==0){
                printf("anneau :");
                for (int w = 0; w < sizeT; w++)
                {
                    
                    printf("%d",t[w]);
                }
                printf("\n");
                
                
                
           }
               
            //Si pi est une feuille    
            if(countElInArray(0,t,sizeT) == 1  ){

                //si pi est voisin de l'initiateur
                if(findElementInArray(0,neighbors,neighborsCount) == 1){

                    //Diffuser T a pi (c'est un anneau)
                    //t[pi]=0;
                    sendMessage(0,t,sizeT,0,graph_comm);
                    
                    //envoie retour au predecesseur dans t qui est pj
                    sendMessage(1,t,sizeT,pj,graph_comm);

                    
                
    
                 
                }
            
            }
            
                
            //Si p n'est pas une feuille
            else if(countElInArray(0,t,sizeT) >1  ) {
                
                //nombre de message envoiés
                int sent=0;

                //diffusion a tous les voisins
                for ( int c = 0; c < neighborsCount; c++)
                {   
                    if(neighbors[c] != pj && findElementInArray(neighbors[c],t,sizeT) == 0){

                        t[pi]=neighbors[c];
                        sendMessage(0,t,sizeT,neighbors[c],graph_comm);
                        nbList++;

                        sent++;
                        t[pi]=0;
                    }
                   
                }
                //Si pi n'arrive a envoyer aucune message de diffusion
                if(sent ==0){
                     //envoie retour au predecesseur dans T
                    for ( i = 0; i < sizeT; i++)
                    {
                        if(t[i]==pi){

                            sendMessage(1,t,sizeT,i,graph_comm);
                        }
                    }
                }
            
            }

        
          
                      
        } 
        // A la reception de retour(Tableau) par pi depuis pj
        if (buffer[0] == 1)
        {   
            
             if(pi==0){
exit(0);
             }   
            //Decrementation du nombre de retours a attendre
            nbList--;
            //Si tous les retours sont recu
            if(nbList==0) {
                //Si pi est l'initiateur
                if(pi==0){

                         
                    //diffusion de fin a tous les voisins et break
                    for ( int c = 0; c < neighborsCount; c++)
                    {  
                        
                        sendMessage(2,t,sizeT,neighbors[c],graph_comm);

                    }
                    break;
                }
                
                    
            }
            if(pi!=0){
                 //envoie retour au predecesseur dans T
                for ( i = 0; i < sizeT; i++)
                {
                    if(t[i]==pi){

                        sendMessage(1,t,sizeT,i,graph_comm);
                        
                    }
                }
            }
            
    
          
            
           

        }   

        //A la reception de fin par pi depuis pj
        if(buffer[0]==2){
            
            //diffusion de fin a tous les voisins !=pj et break
            for ( int c = 0; c < neighborsCount; c++)
            {   
               
                
                if(neighbors[c]!=pj) sendMessage(2,t,sizeT,neighbors[c],graph_comm);
                

            }
            break;
        }

    }
       
    MPI_Finalize();
    return 0;
}