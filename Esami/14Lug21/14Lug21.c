/* Soluzione della parte C del compito del 14 Luglio 2021 */
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
typedef char L[250]; /* tipo array statico: per ogni linea letta dai figli dal proprio file come indicato dal testo bastano 250 caratteri per linea e terminatore di linea */

int main (int argc, char **argv)
{
int N; 			/* numero di file/processi */
/* ATTENZIONE NOME N imposto dal testo! */
int pid;                /* pid per fork */
pipe_t *pipes;  /* array di pipe usate a pipeline da primo figlio, a secondo
 figlio .... ultimo figlio e poi a padre: ogni processo (a parte il primo) legge dal
la pipe i-1 e scrive sulla pipe i */
int n,j,i,k; 		/* indici */
/* ATTENZIONE NOME n imposto dal testo! */
int fd, fdw; 		/* file descriptor */
int pidFiglio, status, ritorno;	/* per valore di ritorno figli */
int nroLinee;		/* variabile per salvare la lunghezza in linee dei file */
/* ATTENZIONE NOME nroLinee imposto dal testo! */
L linea;        	/* linea corrente */
/* ATTENZIONE NOME linea imposto dal testo! */
L *tutteLinee;        	/* array dinamico di linee */
/* ATTENZIONE NOME tutteLinee imposto dal testo! */
int nr,nw;              /* variabili per salvare valori di ritorno di read/write da/su pipe */

/* controllo sul numero di parametri: la specifica diceva minimo un solo file e la lunghezza in linee, ma in realta' era una imprecisione del testo e quindi nella soluzione si e' considerato almeno 2 file e la lunghezza in linee (nella correzione delle consegne si e' considerato quello c'era scritto nei testi)*/
if (argc < 4)
{
	printf("Errore numero di parametri\n");
	exit(1);
}

nroLinee=atoi(argv[argc-1]);
if (nroLinee <= 0)
{
	printf("Errore nel numero di linee %d\n", nroLinee);
	exit(2);
}

N = argc-2;
printf("Numero di processi da creare %d\n", N);

/* allocazione pipe */
if ((pipes=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
{
	printf("Errore allocazione pipe\n");
	exit(3); 
}

/* creazione pipe */
for (n=0;n<N;n++)
	if(pipe(pipes[n])<0)
	{
		printf("Errore creazione pipe\n");
		exit(4);
	}

/* allocazione array di linee */
if ((tutteLinee=(L *)malloc(N*sizeof(L))) == NULL)
{
	printf("Errore allocazione array di linee\n");
	exit(5); 
}

/* creiamo il file nella directory corrente avente come nome il mio cognome (TUTTO IN MAIUSCOLO, come specificato nel testo) */
if ((fdw=creat("LEONARDI", PERM)) < 0)
{
        printf("Errore nella creat del file %s\n", "LEONARDI");
        exit(6);
}

/* creazione figli */
for (n=0;n<N;n++)
{
	if ((pid=fork())<0)
	{
		printf("Errore creazione figlio\n");
		exit(7);
	}
	if (pid == 0)
	{ /* codice figlio */
		printf("Sono il figlio %d e sono associato al file %s\n", getpid(), argv[n+1]);
		/* nel caso di errore in un figlio decidiamo di ritornare il valore -1 che sara' interpretato dal padre come 255 (valore NON ammissibile) */

		/* chiusura pipes inutilizzate */
		for (j=0;j<N;j++)
		{	/* si veda commento nella definizione dell'array pipes per comprendere le chiusure */
                	if (j!=n)
                		close (pipes[j][1]);
                	if ((n == 0) || (j != n-1))
               			close (pipes[j][0]);
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
				if (n != 0)
	                	{
					/* se non siamo il primo figlio, dobbiamo aspettare l'array dal figlio precedente per mandare avanti */
                			nr=read(pipes[n-1][0],tutteLinee,N*sizeof(L));
        				/* per sicurezza controlliamo il risultato della lettura da pipe */
                			if (nr != N*sizeof(L))
                			{
                       				printf("Figlio %d ha letto un numero di byte sbagliati %d\n", n, nr);
                       				exit(-1);
                			}
                		}	
				/* a questo punto si deve inserire la linea letta nel posto giusto */
				for (k=0; k <= j; k++)
                       		{ /* ricordarsi che non si puo' fare una copia diretta di un array! */
					tutteLinee[n][k]=linea[k];
                       		}

				/* ora si deve mandare l'array in avanti */
        			nw=write(pipes[n][1],tutteLinee,N*sizeof(L));
        			/* anche in questo caso controlliamo il risultato della scrittura */
        			if (nw != N*sizeof(L))
        			{
               				printf("Figlio %d ha scritto un numero di byte sbagliati %d\n", n, nw);
               				exit(-1);
        			}
				/* si deve azzerare l'indice della linea, dopo averlo salvato (incrementato) per poterlo tornare correttamente */
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
/* chiusura di tutte le pipe che non usa */
for (n=0;n<N;n++) 
{
	close (pipes[n][1]);
	if (n != N-1) close (pipes[n][0]); 
}

/* il padre deve leggere tutti gli array di linee inviati dall'ultimo figlio */
for (j=1; j<=nroLinee; j++)
{
        nr=read(pipes[n-1][0],tutteLinee,N*sizeof(L));
       	/* per sicurezza controlliamo il risultato della lettura da pipe */
        if (nr != N*sizeof(L))
      	{
       		printf("Padre ha letto un numero di byte sbagliati %d\n", nr);
                exit(8);
        }

	/* il padre deve scrivere le linee sul file creato */
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

/* Il padre aspetta i figli */
for (n=0; n < N; n++)
{
        pidFiglio = wait(&status);
        if (pidFiglio < 0)
        {
                printf("Errore in wait\n");
                exit(9);
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
