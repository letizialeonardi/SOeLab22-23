#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main (int argc, char **argv)
{
   char Cx;				/* carattere che i figli devono cercare nel file a loro associato */
   int N; 				/* numero di figli */
   int pid;				/* pid per fork */
   int i; 				/* indice per i figli */
   int totale=0; 			/* serve per calcolare il numero di occorrenze: in questo caso abbiamo usato un semplice int perche' la specifica dice che si puo' supporre minore di 255 */
   int fd; 				/* per la open */
   char c; 				/* per leggere i caratteri da ogni file */
   int pidFiglio, status, ritorno;	/* per valore di ritorno figli */

	/* controllo sul numero di parametri: almeno due nomi file e un carattere */
	if (argc < 4)
	{	
		printf("Errore numero di parametri: i parametri passati a %s sono solo %d\n", argv[0], argc);
		exit(1);
	}

	/* controlliamo che l'ultimo parametro sia un singolo carattere */
	if (strlen( argv[argc-1]) != 1)
	{
        	printf("Errore ultimo parametro non singolo carattere dato che e' %s\n", argv[argc-1]);
        	exit(2);
	}

	/* individuiamo il carattere da cercare */
	Cx = argv[argc-1][0];

	/* individuiamo il numero di file/processi */
	N=argc-2; 	/* ATTENZIONE dobbiamo togliere sia il nome dell'eseguibile che il carattere! */

	printf("DEBUG-Sono il processo padre con pid %d e creero' %d processi figli che cercheranno il carattere %c nei file passati come parametri\n", getpid(), N, Cx); 
	/* creazione figli */
	for (i=0; i < N; i++)
	{
		if ((pid=fork())<0)
		{
			printf("Errore creazione figlio\n");
			exit(3);
		}
		if (pid==0)
		{ 	/* codice figlio */
			printf("DEBUG-Sono il figlio %d di indice %d\n", getpid(), i);
			/* apriamo il file (deleghiamo ad ogni processo figlio, il controllo che i singoli parametri (a parte l'ultimo) siano nomi di file */
			/* notare che l'indice che dobbiamo usare e' i+1 */
			/* in caso di errore decidiamo di ritornare -1 che sara' interpretato dal padre come 255 e quindi un valore non valido! */
			if ((fd = open(argv[i+1], O_RDONLY)) < 0)
			{	
				printf("Errore: FILE %s NON ESISTE o NON LEGGIBILE\n", argv[i+1]); 
				exit(-1);
			}	
			/* leggiamo il file */
			while (read (fd, &c, 1) != 0)
		         	if (c == Cx) totale++;     /* se troviamo il carattere incrementiamo il conteggio */
		
		 	/* ogni figlio deve tornare il numero di occorrenze e quindi totale */
			exit(totale);	
		}	
	} /* fine for */

	/* codice del padre */
	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
        	if ((pidFiglio=wait(&status)) < 0)
        	{
                	printf("Errore in wait\n");
                	exit(4);
        	}
        	if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{ 
			ritorno=(int)((status >> 8) & 0xFF);
        		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
        	} 
	}

	exit(0);
}
