/* Soluzione della parte C del compito del 16 Giugno 2021 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#define PERM 0644

typedef int pipe_t[2];
typedef char L[250]; /* tipo array statico: per ogni linea letta dai figli dal proprio file come indicato dal testo basta 250 caratteri per linea e terminatore di linea */

int main (int argc, char **argv)
{
int N; 			/* numero di file/processi */
/* ATTENZIONE NOME N imposto dal testo! */
int pid;                /* pid per fork */
pipe_t *pipes;          /* array di pipe usate a ring da primo figlio, a secondo figlio .... ultimo figlio, padre e poi a primo figlio: ogni processo legge dalla pipe n
 e scrive sulla pipe (n+1) */
int n,j,i,k; 		/* indici */
/* ATTENZIONE NOME n imposto dal testo! */
int fd, fdw; 		/* file descriptor */
int pidFiglio, status, ritorno;	/* per valore di ritorno figli */
L linea;        	/* linea corrente */
/* ATTENZIONE NOME linea imposto dal testo! */
L *tutteLinee;        	/* array dinamico di linee */
/* ATTENZIONE NOME tutteLinee imposto dal testo! */
int nr,nw;              /* variabili per salvare valori di ritorno di read/write da/su pipe */

/* controllo sul numero di parametri almeno 2 file */
if (argc < 3)
{
	printf("Errore numero di parametri\n");
	exit(1);
}

N = argc-1;
printf("Numero di processi da creare %d\n", N);

/* allocazione pipe */
if ((pipes=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
{
	printf("Errore allocazione pipe\n");
	exit(2); 
}

/* creazione pipe */
for (n=0;n<N;n++)
	if(pipe(pipes[n])<0)
	{
		printf("Errore creazione pipe\n");
		exit(3);
	}

/* allocazione array di linee */
if ((tutteLinee=(L *)malloc(N*sizeof(L))) == NULL)
{
	printf("Errore allocazione array di linee\n");
	exit(4); 
}

/* creiamo il file nella directory corrente avente come nome il mio cognome */
if ((fdw=creat("Leonardi", PERM)) < 0)
{
        printf("Errore nella creat del file %s\n", "Leonardi");
        exit(5);
}

/* creazione figli */
for (n=0;n<N;n++)
{
	if ((pid=fork())<0)
	{
		printf("Errore creazione figlio\n");
		exit(6);
	}
	if (pid == 0)
	{ /* codice figlio */
		printf("Sono il figlio %d e sono associato al file %s\n", getpid(), argv[n+1]);
		/* nel caso di errore in un figlio decidiamo di ritornare il valore -1 che sara' interpretato dal padre come 255 (valore NON ammissibile) */

		/* chiusura pipes inutilizzate */
		for (j=0;j<N;j++)
		{	/* si veda commento nella definizione dell'array pipes per comprendere le chiusure */
			if (j!=n)
				close (pipes[j][0]);
			if (j != (n+1)%N)
				close (pipes[j][1]);
		}	
	
		/* apertura file */
		if ((fd=open(argv[n+1],O_RDONLY))<0)
		{
			printf("Impossibile aprire il file %s\n", argv[n+1]);
			exit(-1);
		}
	
		/* inizializziamo l'indice dei caratteri letti per ogni singola linea */
		j = 0;
		/* con un ciclo leggiamo tutte le linee, come richiede la specifica */
        	while(read(fd,&(linea[j]),1) != 0)
        	{
          		if (linea[j] == '\n') /* siamo a fine linea */
                	{
				/* dobbiamo aspettare l'array dal figlio precedente o dal padre per mandare avanti */
                		nr=read(pipes[n][0],tutteLinee,N*sizeof(L));
        			/* per sicurezza controlliamo il risultato della lettura da pipe */
                		if (nr != N*sizeof(L))
                		{
                       			printf("Figlio %d ha letto un numero di byte sbagliati %d\n", n, nr);
                       			exit(-1);
                		}
				/* a questo punto si deve inserire la linea letta nel posto giusto */
				for (k=0; k <=j ; k++)
                        	{
					/* ricordarsi che non si possono copiare direttamente gli array */
					/* fare attenzione ad includere nella copia anche lo '\n' e quindi il controllo su k e' k <= j */
					tutteLinee[n][k]=linea[k];
                        	}

				/* ora si deve mandare l'array in avanti */
        			nw=write(pipes[(n+1)%N][1],tutteLinee,N*sizeof(L));
        			/* anche in questo caso controlliamo il risultato della scrittura */
        			if (nw != N*sizeof(L))
        			{
               				printf("Figlio %d ha scritto un numero di byte sbagliati %d\n", n, nw);
               				exit(-1);
        			}
				/* NOTA BENE: nell'ultima iterazione l'ultimo figlio mandera' un OK al primo figlio che pero' non verra' ricevuto, ma non creera' alcun problema a patto che il padre mantenga aperto il lato di lettura di pipes[0]: in questo modo, l'ultimo figlio non incorrera' nel problema di scrivere su una pipe che non ha lettori */
				/* l'ultimo figlio deve scrivere le linee sul file creato dal padre */
				if (n == N-1)
                        	{
					/* printf("ULTIMO FIGLIO indice %d e pid= %d\n", n, getpid()); */
					for (i=0;i<N;i++)
					{	
						for (k=0; k<250; k++)
	        				{
							/* fino a che non incontriamo il fine linea scriviamo sul file creato */
							write(fdw, &(tutteLinee[i][k]), 1);
							if (tutteLinee[i][k] == '\n')
                        				{
								/* quando troviamo il terminatore di linea, ... */
								break; /* usciamo dal ciclo for piu' interno */
							}	
						}
					}		
				}	

				/* si deve azzerare l'indice della linea e il conteggio dei caratteri numerici, quest'ultimo dopo averlo salvato e incrementato per poterlo tornare correttamente */
					ritorno = j+1;
					j = 0;
        		}	
			else
                	{
				j++; /* incrementiamo sempre l'indice della linea */
        		}		
        	}		
		/* ogni figlio deve tornare il numero di caratteri dell'ultima linea */
		exit(ritorno);
	}
} /* fine for */

/* codice del padre */
/* chiusura di tutte le pipe che non usa, a parte la prima perche' il padre deve dare il primo OK al primo figlio. N.B. Si lascia aperto sia il lato di scrittura che viene usato (e poi in effetti chiuso) che il lato di lettura (che non verra' usato ma serve perche' non venga inviato il segnale SIGPIPE all'ultimo figlio che terminerebbe in modo anomalo)  */
for (n=1;n<N;n++) 
/* l'indice lo facciamo partire nuindi da 1! */
{
	close (pipes[n][0]);
	close (pipes[n][1]); 
}

/* ora si deve mandare l'array tutteLinee per innescare il ring: notare che non serve azzerarlo */
nw=write(pipes[0][1],tutteLinee,N*sizeof(L));
/* anche in questo caso controlliamo il risultato della scrittura */
if (nw != N*sizeof(L))
{
       printf("Padre ha scritto un numero di byte sbagliati %d\n", nw);
       exit(7);
}

/* ora possiamo chiudere anche il lato di scrittura, ma ATTENZIONE NON QUELLO DI LETTURA! */
close(pipes[0][1]);
/* OSSERVAZIONE SU NON CHIUSURA DI pipes[0][0]: se si vuole procedere con la chiusura di tale lato nel padre, bisognerebbe introdurre del codice ulteriore solo nel prim
o figlio che vada a fare la lettura dell'ultimo OK prima di terminare! */

/* Il padre aspetta i figli */
for (n=0; n < N; n++)
{
        pidFiglio = wait(&status);
        if (pidFiglio < 0)
        {
                printf("Errore in wait\n");
                exit(8);
        }
        if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else
        { 
		ritorno=(int)((status >> 8) & 0xFF);
        	printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
        } 
}
exit(0);
}
