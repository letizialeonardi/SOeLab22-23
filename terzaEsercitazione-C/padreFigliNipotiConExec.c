#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define PERM 0644

int main (int argc, char **argv)
{
   int N; 				/* numero di figli */
   int pid;				/* pid per fork */
   int i; 				/* indice */
   char *FOut;				/* array dinamico di char per il nome del file da creare da parte dei figli */
   int fdw; 				/* per la creat */
   int pidFiglio, status, ritorno;	/* per valore di ritorno figli/nipoli */

	/* controllo sul numero di parametri: almeno tre nomi file */
	if (argc < 4)
	{
		printf("Errore numero di parametri: i parametri passati a %s sono solo %d\n", argv[0], argc);
		exit(1);
	}

	/* individuiamo il numero di file/processi */
	N=argc-1; 	/* in questo caso basta eliminare il nome del programma eseguibile */

	printf("Sono il processo padre con pid %d e creero' %d processi figli che generanno ognuno un nipote\n", getpid(), N);

	/* creazione figli */
	for (i=0;i<N;i++)
	{
		if ((pid=fork())<0)
		{
			printf("Errore creazione figlio %d-esimo\n", i);
			exit(3);
		}
		if (pid==0)
		{ 	/* codice figlio */
			printf("Sono il figlio %d di indice %d\n", getpid(), i);
		/* in caso di errore (sei nei figli che nei nipoti) decidiamo di ritornare -1 che sara' interpretato dal padre come 255 e quindi un valore non valido! */
	 	/* i figli devono creare il file specificato */
                	FOut=(char *)malloc(strlen(argv[i+1]) + 6); /* bisogna allocare una stringa lunga come il nome del file + il carattere '.' + i caratteri della parola sort (4) + il terminatore di stringa */
                	if (FOut == NULL)
                	{
                		printf("Errore nelle malloc\n");
                        	exit(-1);
                	}
                	/* copiamo il nome del file associato al figlio */
                	strcpy(FOut, argv[i+1]);
                	/* concateniamo la stringa specificata dal testo */
                	strcat(FOut,".sort");
                	if ((fdw=creat(FOut, PERM)) < 0)
                	{
                		printf("Impossibile creare il file %s\n", FOut);
                        	exit(-1);
                	}
			/* chiudiamo il file creato che il figlio non usa */
			close(fdw);

			if ( (pid = fork()) < 0) /* ogni figlio crea un nipote */
			/* OSSERVAZIONE: abbiamo riutilizzato la stessa variabile pid */
                	{
                        	printf("Errore nella fork di creazione del nipote\n");
                        	exit(-1); 
                	}
                	if (pid == 0)
                	{
                 		/* codice del nipote */
                        	printf("Sono il processo nipote del figlio di indice %d e pid %d sto per eseguire il comando sort per il file %s\n", i, getpid(), argv[i+1]); 
				/* chiudiamo lo standard input dato che la specifica dice che dobbiamo usare il comando filtro sort */
				close(0);
				/* apriamo il file associato in sola lettura */
				if (open(argv[i+1], O_RDONLY) < 0)
				{	
					printf("Errore: FILE %s NON ESISTE\n", argv[i+1]); 
					exit(-1);
				}
				/* chiudiamo lo standard output sempre per la stessa ragione */
                        	close(1);
				/* apriamo il file creato in sola scrittura */
				if (open(FOut, O_WRONLY) < 0)
				{	
					printf("Errore: FILE %s NON si riesce ad aprire in scrittura\n", FOut); 
					exit(-1);
				}
                        	/* Il nipote diventa il comando sort: bisogna usare le versioni dell'exec con la p in fondo in modo da usare la variabile di ambiente PATH: NON serve alcun parametro */
                        	execlp("sort", "sort", (char *)0);

                        	/* Non si dovrebbe mai tornare qui!!*/		
		       		/* usiamo perror che scrive su standard error, dato che lo standard output e' collegato ad un file */
                       		perror("Problemi di esecuzione del sort da parte del nipote");
                       		exit(-1); 
			}
 			/* il figlio deve aspettare il nipote per ritornare al padre il valore tornato dal nipote  */
                	if ((pidFiglio=wait(&status)) < 0)
                	{
                		printf("Errore in wait\n");
                        	exit(-1); 
                	}
                	if ((status & 0xFF) != 0)
                	{
                		printf("Nipote con pid %d terminato in modo anomalo\n", pidFiglio);
                        	exit(-1); 
                	}
                	else
				ritorno=(int)((status >> 8) & 0xFF);
               		/* il figlio ritorna il valore ricevuto dal nipote  */
                	exit(ritorno);
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
