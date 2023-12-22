/* Soluzione della parte C del compito del 25 Gennaio 2023 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];

/* VARIABILI GLOBALI */
int *finito;    /* array dinamico per indicare i figli che sono terminati */
                /* la semantica di questo array e' che ogni elemento vale 0 se il corrisponde
		 * nte processo NON e' finito, altrimenti vale 1 */
int N;          /* numero di processi figli (N.B. nome indicato dal testo) */

int finitof()
{
/* questa funzione verifica i valori memorizzati nell'array finito: appena trova un elemento uguale a 0 vuole dire che non tutti i processi figli sono finiti e quindi torna 0; tornera' 1 se e solo se tutti gli elementi dell'array sono a 1 e quindi tutti i processi sono finiti */
  int i;
	for (i=0; i < N; i++)
		if (!finito[i])
		/* appena ne trova uno che non ha finito */
			return 0; /* ritorna falso */
 	return 1;
}

int main (int argc, char **argv)
{
int pid;		/* pid per fork */
pipe_t *piped;		/* array di pipe */
int n,j; 		/* indici(N.B. n nome indicato dal testo)  */
int fdr, fdw; 		/* file descriptor */
int pidFiglio, status, ritorno;	/* per valore di ritorno figli */
char chs[2];        	/* caratteri letti dai figli dal proprio file (N.B. nome indicato dal testo)*/
/* N.B: dato che il numero di caratteri che devono essere letti ogni volta e' noto STATISTICAMENTE si deve usare un array STATICO */
int nr,nw;              /* variabili per salvare valori di ritorno di read/write da/su file e pipe */

/* controllo sul numero di parametri almeno 3 file */
if (argc < 4)
{
	printf("Errore numero di parametri: argc = %d\n", argc);
	exit(1);
}

N = argc-2;	/* i file passati sono N+1 */
printf("Numero di processi da creare %d\n", N);

/* allocazione memoria dinamica per finito */
finito = (int *) malloc(sizeof(int) * N);
if(finito == NULL)
{
	printf("Errore nella allocazione della memoria per array finito\n");
	exit(2);
}

/* ... e inizializzazione a 0: all'inizio nessun figlio e' finito */
memset(finito, 0, N*sizeof(int));

/* allocazione pipe */
if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
{
	printf("Errore allocazione pipe\n");
	exit(3); 
}

/* creazione pipe */
for (n=0;n<N;n++)
	if(pipe(piped[n])<0)
	{
		printf("Errore creazione pipe\n");
		exit(4);
	}
	
/* apertura file che deve essere scritto dal padre: l'indice e' N+1 oppure argc-1 come si preferisce */
if ((fdw=open(argv[N+1],O_WRONLY))<0)
{
	printf("Impossibile aprire il file %s\n", argv[N+1]);
	exit(5);
}
lseek(fdw, 0L, SEEK_END); /* il padre si posizione alla fine del file, oppure in alternativa open in append */

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
	printf("Sono il figlio di indice %d e pid %d e sono associato al file %s\n", n, getpid(), argv[n+1]);
	/* nel caso di errore in un figlio decidiamo di ritornare -1 che non e' un valore accettabile */

	/* chiusura pipes inutilizzate */
	for (j=0;j<N;j++)
	{	
		close(piped[j][0]);
		if (j != n)
		{	
			close(piped[j][1]);
		}
	}

	/* apertura file che deve essere letto: quello associato */ 
	if ((fdr=open(argv[n+1],O_RDONLY))<0)
	{
		printf("Impossibile aprire il file %s\n", argv[n+1]);
		exit(-1);
	}
	ritorno=0; 	/* inizializziamo il valore di ritorno che rappresentera' il numero di caratteri inviati dal ogni figlio; questo valore e' supposto dal testo minore di 255 */
	/* con un ciclo leggiamo tutti i caratteri, a due alla volta, come richiede la specifica */
 	while ((nr=read(fdr, chs, 2)) != 0)
        {
		/* a questo punto si deve mandare al padre */
		nw=write(piped[n][1], chs, nr); /* ATTENZIONE: SI DEVE USARE nr E NON 2 */
        	/* controlliamo il risultato della scrittura */
        	if (nw != nr)
        	{
               		printf("Figlio %d ha scritto un numero di byte sbagliati %d su pipe %d\n", n, nw, n);
               		exit(-1);
        	}
		ritorno=ritorno+nr;	/* incrementiamo il conteggio dei caratteri inviati dal figlio */
        }
	exit(ritorno);
	}
} /* fine for */

/* codice del padre */
/* chiusura di tutte le pipe che non usa */
for(n=0;n < N;n++)
{
	close (piped[n][1]); 
}

while(!finitof())
{
	for (n=0; n < N; n++)
	{
		/* tentiamo di leggere i caratteri dal figlio i-esimo: contestualmente viene aggiornato il valore del corrispondente elemento dell'array finito */
		nr=read(piped[n][0], chs, 2);  
		finito[n] = (nr == 0);  
		/* nel caso la read torni un valore 0, il corrispondente valore di finito viene settato a 1 dato che la condizione e' vera! */
		if (!finito[n])
	        {
			write(fdw, chs, nr);
			/* il padre scrive sul file: ATTEZNIONE SI DEVE USARE nr e NON 2 */
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
                exit(7);
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
