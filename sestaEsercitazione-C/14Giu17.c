/* Soluzione della Prova d'esame del 14 Giugno 2017 - Parte C */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char **argv) 
{
	/* -------- Variabili locali ---------- */
	int pid;			/* process identifier per le fork() */
	int N; 				/* numero di file passati sulla riga di comando */
	char Cx;			/* carattere passato come ultimo parametro */
	int fd; 			/* file descriptor per apertura file */
	long int pos; 			/* valore per comunicare la posizione al padre e quindi all'utente */
	int status;			/* variabile di stato per la wait */
	pipe_t *pipedFP;		/* array dinamico di pipe descriptors per comunicazioni figli-padre */
	pipe_t *pipedPF;		/* array dinamico di pipe descriptors per comunicazioni padre-figli */
	int i, j;			/* indici per i cicli */
	char cx; 			/* variabile che viene usata dal padre per avere dall'utente l'informazione del carattere da sostituire e  dai figli per recuperare il carattere comunicato dal padre */
	char scarto; 			/* variabile che viene usata dal padre per eliminare lo '\n' letto dallo standard input */
	char ch; 			/* variabile che viene usata dai figli per leggere dal file */
	int ritorno=0; 			/* variabile che viene ritornata da ogni figlio al padre e che contiene il numero di caratteri sostituiti nel file (supposto minore di 255 */
  	int nr;     			/* variabile che serve al padre per sapere se non ha letto nulla */
  	int finito;     		/* variabile che serve al padre per sapere se non ci sono piu' posizioni da leggere */
      
	/* ------------------------------------ */
	
	/* Controllo sul numero di parametri */
	if (argc < 3) /* Meno di due parametri: ci vuole almeno un file e il carattere da cercare */  
	{
		printf("Errore nel numero dei parametri dato che argc=%d (ce ne vogliono almeno due, un file e un carattere)\n", argc);
		exit(1);
	}
	/* Calcoliamo il numero di parametri passati */
	N = argc - 2;
	/* Isoliamo il carattere, dopo aver controllato che sia un singolo carattere */
	if (argv[argc-1][1] != '\0')
	{
		printf("Errore nell'ultimo parametro %s: non singolo carattere\n", argv[argc-1]);
		exit(2);
	}
	Cx = argv[argc-1][0];

	printf("DEBUG-Sono il padre con pid %d e creo %d figli che dovranno cercare il carattere %c\n", getpid(), N, Cx);

	/* Allocazione dei due array di N pipe descriptors*/
	pipedFP = (pipe_t *) malloc (N*sizeof(pipe_t));
	pipedPF = (pipe_t *) malloc (N*sizeof(pipe_t));
	if ((pipedFP == NULL) || (pipedPF == NULL))
	{
		printf("Errore nella allocazione della memoria per le pipe\n");
		exit(3);
	}
	
	/* Creazione delle N pipe figli-padre e delle N pipe padre-figli */
	for (i=0; i < N; i++)
	{
		if(pipe(pipedFP[i]) < 0)
		{
			printf("Errore nella creazione della pipe figli-padre\n");
			exit(4);
		}
		if(pipe(pipedPF[i]) < 0)
		{
			printf("Errore nella creazione della pipe padre-figli\n");
			exit(5);
		}

	}
	
/* Ciclo di generazione dei figli */
	for (i=0; i < N; i++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork\n");
			exit(6);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio: in caso di errore torniamo -1 che verra' considerato come 255 dal padre */
			printf("DEBUG-Sono il processo figlio di indice %d e pid %d e sono associato al file %s\n", i, getpid(), argv[i+1]);
			/* Chiusura delle pipe non usate nella comunicazione con il padre */
			for (j=0; j < N; j++)
			{
				close(pipedFP[j][0]);
				close(pipedPF[j][1]);
				if (i != j)  {
						close(pipedFP[j][1]);
						close(pipedPF[j][0]);
						}
			}
			/* apertura del file associato sia in lettura che in scrittura! */
			if ((fd=open(argv[i+1], O_RDWR)) < 0)
			{
                                printf("Errore nella open del file %s\n", argv[i+1]);
                                exit(-1);
                       	}
			while (read(fd, &ch, 1))
			{
				if (ch == Cx)
				{ /* il figlio ha trovato il carattere cercato e quindi bisogna comunicare al padre la posizione: si puo' fare, ad esempio, usando la funzione lseek; consideriamo le posizioni dei caratteri a partire dalla posizione 1 (dato che abbiamo gia' letto il carattere il valore restituito dalla lseek sara' gia' quello giusto) */
		 			pos = lseek(fd, 0L, SEEK_CUR);
					write(pipedFP[i][1], &pos, sizeof(pos));
					/* il figlio ora deve aspettare il carattere da sostituire, se '\n' non deve fare nulla */
					read(pipedPF[i][0], &cx, 1);
                        		/* printf("FIGLIO RICEVUTO %c\n", cx); */
					/* adesso il figlio deve tornare indietro con il file pointer per sovrascrivere, nel caso, il carattere trovato con quello comunicato dal padre */
					if (cx != '\n')
					{
				/* per poter sovrascrivere il carattere, come richiesto dal padre, bisogna tornare indietro di una posizione */
						lseek(fd, -1L, SEEK_CUR);
						write(fd, &cx, 1);
						/* il figlio incrementa il valore di ritorno, dato che e' stata fatta una sostituzione */
						ritorno++;
                        		}	
                        	}	
		        }
			/* il figlio Pi deve ritornare al padre il valore corrispondente al numero di sostituzioni effettuate (che si puo' supporre strettamente minore di 255, come indicato nel testo) */
			exit(ritorno);
  		}	
	}
	
/* Codice del padre */
/* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
 	{
		close(pipedFP[i][1]);
		close(pipedPF[i][0]);
        }

/* Il padre recupera le informazioni dai figli: prima in ordine di posizioni e quindi in ordine di indice */
 	finito = 0; /* all'inizio supponiamo che non sia finito nessun figlio */
        while (!finito)
        {
		finito = 1; /* mettiamo finito a 1 perche' potrebbero essere finiti tutti i figli */
                for (i=0; i<N; i++)
                {
                 /* si legge la posizione inviata  dal figlio i-esimo */
                	nr = read(pipedFP[i][0], &pos, sizeof(pos));
			if (nr != 0)
                        { 
				finito = 0; /* almeno un processo figlio non e' finito */
                		printf("Il figlio di indice %d ha letto dal file %s nella posizione %ld il carattere %c. Se vuoi sostituirlo, fornisci il carattere altrimenti una linea vuota?\n", i,  argv[i+1], pos, Cx);
        			read(0, &cx, 1);
				if (cx != '\n') read(0, &scarto, 1); /* se e' stato letto un carattere, bisogna fare una lettura a vuoto per eliminare il carattere corrispondente all'invio */
				write(pipedPF[i][1], &cx, 1);  /* inviamo comunque al figlio */

                        }
                }
         }

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
			printf("Errore in wait\n");
			exit(7);
		}
		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 	ritorno=(int)((status >> 8) &	0xFF); 
		  	printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi nel figlio)\n", pid, ritorno);
		}
	}

	exit(0);
}
