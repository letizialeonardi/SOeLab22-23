#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>	/* per la wait */
#include <time.h>	/* per la inizializzazione del seme per la rand */


int mia_random(int n)
{
   int casuale;
	casuale = rand() % n;	/* con rand() viene calcolato un numero pseudo casuale e con l'operazione modulo n si ricava un numero casuale compreso fra 0 e n-1 */
	return casuale;
}

int main()	/* nessun parametro necessario */
{
   int pid;        /* per valore di ritorno della fork */
   int pidFiglio;  /* per valore di ritorno della wait */
   int status;     /* per usarlo nella wait */
   int ritorno;    /* per filtrare valore di uscita del figlio  */

	printf("Sono il processo padre con pid %d\n", getpid());
	srand(time(NULL)); /* inizializziamo il seme per la generazione random di numeri  */
	/* DA NOTARE CHE NEI TESTI DI ESAMI DOVE SI DEVE USARE LA GENERAZIONE RANDOM E' SCRITTO IN ESPLICITO CHI DEVE INIZIALIZZARE IL SEME E QUANDO: IN QUESTO CASO NON E' SCRITTO ED E' STATO DECISO DI FARLO FARE AL PADRE PRMA DI GENERARE IL FIGLIO */

	if ((pid = fork()) < 0)
	{
	        printf("ERRORE nella fork\n");
	        exit(1);
	}

	if (pid == 0)
	{
	   /* figlio */
           int r; /* per valore generato random */

	        printf("Sono il processo figlio con pid %d e sono stato generato dal processo padre con pid %d\n", getpid(), getppid());
	        r=mia_random(100);
	        /* il figlio deve tornare il valore random calcolato che sicuramente e' minore di 100 */
	        exit(r);
	}

	/* padre */
	/* il padre aspetta il figlio */
	if ((pidFiglio=wait(&status)) < 0)
	{
	        printf("ERRORE nella wait %d\n", pidFiglio);
	        exit(2);
	}

	if (pid == pidFiglio) printf("Terminato figlio con PID = %d\n", pidFiglio);
	else
	{       /* problemi */
	        printf("Il pid della wait non corrisponde al pid della fork!\n");
	        exit(3);
	}

	if ((status & 0xFF) != 0)
	        printf("Figlio terminato in modo involontario\n"); /* non stiamo a fare exit con errore, dato che in questo caso il problema e' del figlio e non incide sul padre */
	else
	{
	        ritorno=(int)((status >> 8) & 0xFF);
	        printf("Il figlio con pid=%d ha ritornato %d\n", pidFiglio, ritorno);
	}

	/* padre fa un'altra wait: MA NON CI SONO PIU' FIGLI DA ASPETTARE */
	/* di nuovo come nell'esercizio padreSenzaFigli.c il codice potrebbe essere, 'a monte', molto più complicato e potrebbe essere NON chiaro il fatto che e' stato creato un solo processo figlio : quindi bisogna sempre controllare il valore di ritorno della wait! */
	if (wait((int *)0) < 0)	/* in questo caso si è usata la wait disinteressandoci del valore ddi ritorno dato che avremo errore! */
	{
		printf("Errore in wait\n");
		exit(4);
	}

	exit(0); /* N.B. Non si arrivera' mai qui perche' il padre uscira' con la exit(4)! */
}
