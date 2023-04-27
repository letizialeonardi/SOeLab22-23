#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


int mia_random(int n)
{
   int casuale;
	casuale = rand() % n;	/* con rand() viene calcolato un numero pseudo casuale e con l'operazione modulo n si ricava un numero casuale compreso fra 0 e n-1 */
	return casuale;
}

int main (int argc, char **argv)
{
	int N; 					/* numero di figli */
	int *pid;				/* array dinamico di pid per fork */
	int i, j; 				/* indici per i figli */
	int pidFiglio, status, ritorno;		/* per wait e valore di ritorno figli */

	/* controllo sul numero di parametri: essattamente uno */
	if (argc != 2)
	{
		printf("Errore numero di parametri: %s vuole un numero e invece argc=%d\n", argv[0], argc);
		exit(1);
	}

	/* convertiamo il parametro in numero */
	N=atoi(argv[1]);
	if (N <= 0 || N >= 155)  /* facciamo il controllo richiesto dalla specifica */
	{
		printf("Errore l'unico parametro non e' un numero strettamente positivo o non e' strettamente minore di 155: %d\n", N);
		exit(2);
	}
 
	/* allocazione pid */
	/* ATTENZIONE: non viene ammesso l'uso dei VARIABLE LENGHT ARRAY (VLA) */
	/* OSSERVAZIONE: abbiamo bisogno di questo array dinamico dato che la specifica richiede che il padre stampi, insieme con il valore di ritorno dei figli e PID, anche il numero d'ordine derivante dalla creazione! */
	if ((pid=(int *)malloc(N*sizeof(int))) == NULL)	/* bisogna sempre controllare che la malloc abbia avuto successo! */
	{
        	printf("Errore allocazione pid\n");
        	exit(3);
	}

	printf("Sono il processo padre con pid %d\n", getpid());
	srand(time(NULL)); 	/* inizializziamo il seme per la generazione random di numeri  */
	/* DA NOTARE CHE NEI TESTI DI ESAMI DOVE SI DEVE USARE LA GENERAZIONE RANDOM E' SCRITTO IN ESPLICITO CHI DEVE INIZIALIZZARE IL SEME E QUANDO: IN QUESTO CASO NON
 *  E' SCRITTO ED E' STATO DECISO DI FARLO FARE AL PADRE PRMA DI GENERARE IL FIGLIO */

	/* creazione figli */
	for (i=0;i<N;i++)
	{
		if ((pid[i]=fork())<0)	/* memorizziamo il pid nel posto giusto dell'array dinamico! */
		{
			printf("Errore creazione figlio\n");
			exit(4);
		}
		if (pid[i]==0)
		{  /* codice figlio */
		   int r; 			/* per valore generato random */
			printf("Sono il figlio %d di indice %d\n", getpid(), i);
			r=mia_random(101+i); /* dobbiamo usare 101 dato che mia_random restituisce un numero fra 0 e n-1 */
			/* ogni figlio deve tornare il numero random calcolato */
			exit(r);
		}
	} /* fine for */

	/* codice del padre */

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		if ((pidFiglio=wait(&status)) < 0)
        	{
                	printf("Errore in wait %d\n", pidFiglio);
                	exit(5);
        	}
        	if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{ 
			ritorno=(int)((status >> 8) & 0xFF);
			/* la specifica richiede che il padre, oltre il valore di ritorno e il PID, stampi anche il numero d'ordine e questo puo' essere fatto solo analizzando il contenuto dell'array di pid e riportando l'indice di tale array. ATTENZIONE CHE SAREBBE ASSOLUTAMENTE ERRATO ANDARE A STAMPARE L'INDICE DEL FOR ESTERNO! */
			for (j=0; j < N; j++)
				if (pidFiglio == pid[j])
				{
        				printf("Il figlio con pid=%d di indice %d ha ritornato %d\n", pidFiglio, j, ritorno);	/* attenzione a stampare j e NON i!!! */
					break;
				}	
        	}	 
	}

	exit(0);
}
