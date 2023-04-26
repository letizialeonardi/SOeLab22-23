/* file mycat1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{   char buffer[BUFSIZ]; 	/* buffer per la lettura/scrittura */
    int i;  			/* indice per scorrere i parametri/nomi file */
    int N;			/* numero di parametri, se presenti */
    int fd = 0;			/* fd per open, inizializzato a 0 in modo eventualmente di leggere da standard input */
    int finito=0;		/* variabile per il ciclo di apertura e lettura di ogni file, se presente, o dello standard input */
    int nread;			/* nread numero caratteri lettii */
    
	/* NOTA BENE: non possiamo fare alcun controllo sul numero di parametri perche' possono anche non essere passati parametri! */

	i = 0;	/* inizializziamo il contatore per scorrere i parametri */
	if (argc >= 2)	
		N = argc-1;	/* se abbiamo dei parametri calcoliamone il numero */

	while (!finito)
	{ 
		if (argc >= 2)
		/* se abbiamo dei parametri questi devono essere considerati nomi di un file */
        		if ((fd = open(argv[i+1], O_RDONLY)) < 0)
               		{	printf("Errore in apertura file %s\n", argv[i+1]);
                 		exit(1); 
			}
		i++;
		/* se non abbiamo un parametro, allora fd rimane uguale a 0 */
		while ((nread = read(fd, buffer, BUFSIZ)) > 0 )
		/* lettura dal file o dallo standard input fino a che ci sono caratteri */
                	write(1, buffer, nread); 	/* scrittura sullo standard output dei caratteri letti */
		if ((argc == 1) || (i == N))	/* se non sono stati passati parametri o se li abbiamo considerati gia' tutti */  
			finito=1;
	}

	exit(0);
}

