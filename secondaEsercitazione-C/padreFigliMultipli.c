#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char **argv)
{
    int N;                                  /* numero di figli */
    int pid;                                /* pid per fork */
    int i;                                  /* indice per i figli */
    int pidFiglio, status, ritorno;         /* per wait e valore di ritorno figli */

	/* controllo sul numero di parametri: esattamente uno */
	if (argc != 2)
	{
	        printf("Errore numero di parametri: %s vuole solo un numero e invece argc=%d\n", argv[0], argc);
	        exit(1);
	}

	/* convertiamo il parametro in numero */
	N=atoi(argv[1]);
	if (N <= 0 || N >= 255) /* facciamo il controllo richiesto dalla specifica */
	{
	        printf("Errore l'unico parametro non e' un numero strettamente positivo o non e' strettamente minore di 255: %d\n", N);
	        exit(2);
	}

	printf("Sono il processo padre con pid %d e sto per generare %d figli\n", getpid(), N);

	/* creazione figli */
	for (i=0; i < N; i++)
	{
	        if ((pid=fork())<0)
	        {
	                printf("Errore creazione figlio per l'indice %d\n", i);
	                exit(3);
	        }

		if (pid==0)
	        {       /* codice figlio */
	                printf("Sono il figlio %d di indice %d\n", getpid(), i);
	                /* ogni figlio deve tornare il suo indice ordine */
	                exit(i);
			/* al di la' del valore ritornato, e' ASSOLUTAMENTE INDISPENSABILE che ci sia la presenza di una exit per fare in modo che il processo figlio termini e NON esegua il ciclo for, che invece deve essere eseguito solo dal padre */
	        }
	} /* fine for */

	/* codice del padre */
	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
        	pidFiglio = wait(&status);
		if (pidFiglio < 0)
		{	printf("Errore in wait\n");
		        exit(4);
		}

		if ((status & 0xFF) != 0)
		        printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
		else
		{
		        ritorno=(int)((status >> 8) & 0xFF);
		        printf("Il figlio con pid=%d ha ritornato %d\n", pidFiglio, ritorno);
		}
	}

	exit(0);
}
