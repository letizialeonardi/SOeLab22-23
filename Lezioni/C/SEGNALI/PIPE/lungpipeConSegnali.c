/* FILE: lungpipeConSegnali.c */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int count;		/* variabile GLOBALE per contare i caratteri scritti sulla pipe (che non verranno letti da nessuno!) */

void alrm_action(int signo)
{
	printf("write bloccata dopo %d caratteri (ricevuto segnale SIGALRM che e' il nro %d)\n", count, signo);
	exit(2);
}

int main()
{
   int piped[2]; 	/* array per i lati lettura/scrittura della pipe  */
   char c = 'x'; 	/* non serviva inizializzare il valore del carattere che viene scritto sulla pipe, tanto nessuno lo legge */

	if (pipe(piped) < 0) { printf("Errore\n"); exit(1); }

	/* agganciamo l'allarme al gestore specifico */
	signal(SIGALRM, alrm_action);

	for (count = 0;;)	/* ciclo infinito con azzeramento del contatore */
	{
		/* settiamo l'allarme */
		alarm(10);
		write(piped[1], &c, 1); 	/* scrittura sulla pipe */
		/* se siamo riusciti a scrivere, resettiamo l'allarme */
		alarm(0);

		if ((++count % 1024) == 0)	/* per evitare di stampare il contatore tutte le volte, lo stampiamo solo a multipli di 1024 */
			printf("%d caratteri nella pipe\n", count);
	}

	exit(0); /* non si arrivera' mai qui dato che abbiamo un ciclo infinito con sospensione del processo ad un certo punto sulla write */
}
