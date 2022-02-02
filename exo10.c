// Coloration de processus dans un graphe

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Function for finding  
// elements which are there  
// in a[]  but not in b[]. 
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
        - 0: maCouleur(couleur)
        - 1: coloration(pk)
        - 2: retour(succes/echec) 
      
*/
void sendMessage(int type_mess,int v1, int dest, MPI_Comm comm)
{
    int buffer[2], i;
    buffer[0] = type_mess;
    buffer[1] = v1;
   

    MPI_Send(buffer, 2, MPI_INT, dest, 0, comm);
}



int main(int argc, char *argv[])
{
    srand(time(NULL));
    int wsize, pi,deja=0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    // Création du graphe

    int index[8] = {2, 4, 6, 8,10 ,12,14,16};
    int edges[] = {4,7,3,6,5,7,1,5,0,6,2,3,1,4,0,2};

    MPI_Comm graph_comm;
    MPI_Graph_create(MPI_COMM_WORLD, wsize, index, edges, 1, &graph_comm);
    MPI_Comm_rank(graph_comm, &pi);

    // Variables locales

    int buffer[2];
    int neighborsCount;
    MPI_Graph_neighbors_count(graph_comm, pi, &neighborsCount);
    int neighbors[neighborsCount]; // Liste des voisins
    MPI_Graph_neighbors(graph_comm, pi, neighborsCount, neighbors);
    MPI_Status status;

    enum etats {NONCOLORE,COLORE}etat; 

    int i,nbC,maC,pere=-1;
    int cLibres[] = {1,3,5,6,4,9};
    int cCount = 6;
    int vColore[neighborsCount];
    int vColoreCount = 0;
    etat =NONCOLORE;
    int fils[0];
    int filsCount = 0;
  
   
    

    
    // A la récéption de INIT() par le processus 0
    if (pi == 0)
    {
        printf("Node %i: INIT.\n \n", pi);
        
       
        if(neighborsCount==0){
            etat = COLORE;
        }
        else
        {
             maC = cLibres[0];
             printf(" proc %d color = %d\n",pi,maC);

             removeElementFromArray(cLibres,&cCount,maC);

             for(i=0;i<neighborsCount;i++){
                 sendMessage(0,maC,neighbors[i],graph_comm);
             }
             sendMessage(1,0,neighbors[0],graph_comm);

        }
        
    }

    while (1)
    {

        MPI_Recv(buffer, 2, MPI_INT, MPI_ANY_SOURCE, 0, graph_comm, &status);
        int pj = status.MPI_SOURCE;

        // A la reception de maCouleur() par pi depuis pj
        if (buffer[0] == 0)
        {
            
            removeElementFromArray(cLibres,&cCount,buffer[1]);
            vColore[vColoreCount]=pj;
            vColoreCount++;

          
          
                      
        }
        // A la reception de coloration() par pi depuis pj
        if (buffer[0] == 1)
        {
         
            if(etat == NONCOLORE){
                pere=pj;

                //choix de couleur 
                maC = cLibres[0];
                etat = COLORE;
                printf(" proc %d color = %d\n",pi,maC);
  
                removeElementFromArray(cLibres,&cCount,maC);

                for(i=0;i<neighborsCount;i++){
                
                    sendMessage(0,maC,neighbors[i],graph_comm);
                    
                }
               
                if(vColoreCount==neighborsCount){
                    //envoie de success au père
                    sendMessage(2,1,pere,graph_comm);
                    break;
     }
               
                else
                {
                    
              
                    int v = findMissing(neighbors,vColore,neighborsCount,vColoreCount);
                    if(v!=-1){
                        sendMessage(1,0,v,graph_comm);
                    }
                    
                   
                    if(cCount==0){
                        //echec au père 
                        sendMessage(2,0,pere,graph_comm);
                        break;
                    }
                }
                
           
            }
            else
            {
                if(pj!=pere){
                    //envoie de success a pj
                    sendMessage(2,1,pj,graph_comm);
                    
                }

            }
            

          
          
          
                      
        }
        

    

          
        

        // A la reception de Retour(success/echec) par pi depuis pj

        if (buffer[0] == 2)
        {
            
            if(buffer[1] == 1){
               
                vColore[vColoreCount]=pj;
                if(vColoreCount<neighborsCount) vColoreCount++;
               
                if(neighborsCount==vColoreCount){

                    //envoie de success au pere si il ya un père
                    if(pere!=-1)
                    sendMessage(2,1,pere,graph_comm);
                    break;
                }
                else
                {
                    int v = findMissing(neighbors,vColore,neighborsCount,vColoreCount);
                    if(v!=-1){
                        sendMessage(1,0,v,graph_comm);
                    }
                  
                   
                }
                
            }
            else {
                
                for (i = 0; i < vColoreCount; i++)
                {
                    if(vColore[i] !=pere)
                    {
                       removeElementFromArray(vColore,&vColoreCount,vColore[i]);
                       i--; 
                    }
                    
                }
                if(cCount==0) {
  
                    //echec
                    if(pere!=-1) {
                        sendMessage(2,0,pere,graph_comm);
                        break;
                    }
                    else {

                        printf("coloration echouée\n");
                        break;
                    }
                }
                else
                {
                    //choix de couleur 

                    maC = cLibres[0];
                    etat = COLORE;
                    printf(" proc %d color = %d\n",pi,maC);

                    removeElementFromArray(cLibres,&cCount,maC);

                    for(i=0;i<neighborsCount;i++){
                        sendMessage(0,maC,neighbors[i],graph_comm);
                    }
                    sendMessage(1,0,neighbors[0],graph_comm);  
                }
                
                             
                    
            }
          
        }


    }
       
    MPI_Finalize();
    return 0;
}