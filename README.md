# Algorithmique-Distribuée
TP/Project

Le langage choisi est C, utilisé, conjointement aux primitives offertes par la bibliothèque de 
communication par échange de message MPI. L'acronyme MPI signifie Message Passing 
Interface, spécification d'environnements de programmation pour systèmes à 
mémoire distribuée. Dans le cadre des TPs, nous utiliserons l'implémentation 
OpenMPI (http://www.open-mpi.org). 

Exécuter un programme openMPI nécessite l’inclusion des fichiers d’entête dans lesquels se 
trouvent les définitions des différentes primitives de MPI. Il existe 6 commandes de base :
 MPI _Init qui initialise un bloc de calcul utilisant MPI.
 MPI_Finalize qui termine un bloc de calcul MPI.
 MPI_COMM size qui détermine le nombre de processus qui seront engagé dans un 
calcul.
 MPI_COMM rank qui détermine l’identificateur d’un processus.
 MPI_Send primitive d’envoi de messages.
 MPI_Recv primitive de reception de messages.
