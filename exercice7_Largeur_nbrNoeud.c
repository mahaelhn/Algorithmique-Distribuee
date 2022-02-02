// Calcul du nombre de processus à l'aide d'un parcours en profendeur avec le minimum de messages

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>



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
        - 0: Traverse(list)
        - 1: Retour(list)
*/


int main(int argc, char *argv[])
{
    srand(time(NULL));
    int wsize, pi;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    // Création du graphe

    int index[5] = {1, 4, 7, 10, 12};
    int edges[] = {1, 0, 2, 3, 1, 3, 4, 1, 2, 4, 2, 3};

    MPI_Comm graph_comm;
    MPI_Graph_create(MPI_COMM_WORLD, wsize, index, edges, 1, &graph_comm);
    MPI_Comm_rank(graph_comm, &pi);

    // Variables locales

    int retour;
    int buffer;
    int msg1 = -1;
    int neighborsCount;
    MPI_Graph_neighbors_count(graph_comm, pi, &neighborsCount);
    int neighbors[neighborsCount]; // Liste des voisins
    MPI_Graph_neighbors(graph_comm, pi, neighborsCount, neighbors);
    int visite = 0;//n'est pas visité
    int i,j;
    int c = 0;
    int pred = -1; // Le prédecesseur de ce processus
    MPI_Status status;

    // A la récéption de INIT() par le processus 0

    if (pi == 0)
    { 
	visite = 1;
        printf("Node %i: INIT.\n", pi);
	for(i=0; i<neighborsCount; i++)
        {MPI_Send(&msg1,1,MPI_INT,neighbors[i],0,graph_comm);}
    }

    while (1)
    {
        MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, 0, graph_comm, &status);
        int pj = status.MPI_SOURCE;


        // A la reception de Traverse(list) par pi depuis pj

        if (buffer == -1){
           removeElementFromArray(neighbors, &neighborsCount,pj);
	    if(visite == 0){
		visite = 1;
                pred = pj;
            if (neighborsCount == 0)
            {
		retour = 1;
               // printf("Node %d: Finished, pred =%d c=%d: \n", pi, pred,retour);
                MPI_Send(&retour, 1, MPI_INT, pred, 0, graph_comm);
                break;
            }
            else
            { 	
		for(i=0; i<neighborsCount; i++)
                {MPI_Send(&msg1,1,MPI_INT,neighbors[i],0,graph_comm);}
            }
         }
	else{retour=0;
            MPI_Send(&retour,1,MPI_INT,pj,0,graph_comm);}       
}

        // A la reception de Retour(list) par pi depuis pj

        if (buffer != -1)
        {      removeElementFromArray(neighbors, &neighborsCount,pj);
		c = c+buffer;
            if (neighborsCount == 0)
            {
               // printf("Node %d: Finished, pred =%d c =%d: \n", pi,pred,c);
		printf("Node %d: Finished \n", pi);
                if (pred == -1)
                {
                     c = c+1;
                    printf("Node %d: END c = %d.\n", pi,c);
                break;
                }
                else
                {
	            c = c+1;
		    MPI_Send(&c, 1, MPI_INT, pred, 0, graph_comm);	
                }

                // Its safe to break here
                break;
            }

        }
    }

    MPI_Finalize();
    return 0;
}

