/* FILE: figlio-zombieConStato.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main ()
{
   int pid, pidFiglio, n, status;

	/* si genera un processo figlio */
	if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	} 
	
	if (pid == 0) 
	{ 	/* figlio */ 
		printf("Esecuzione del figlio %d\n", getpid()); 
		/* facciamo terminare subito il figlio dato che ci serve termini PRIMA che il padre abbia effettuato la wait */
		exit(0);
	}

	/* padre */
	printf("Ho generato il figlio %d\n", pid); 
	printf("Esecuzione del padre %d\n", getpid()); 
	/* per fare in modo che il padre non faccia subito la wait gli facciamo aspettare un dato che deve essere fornito dall'utente */
	printf("Fornisci un valore intero\n"); 
	scanf("%d", &n);
	printf("Numero letto dal padre %d\n", n); 
	/* ora possiamo aspettare il figlio */
	if ((pidFiglio=wait(&status)) < 0)
	{
		printf("Errore in wait\n");
		exit(2);
	}

	if (pid == pidFiglio) printf("Terminato figlio con PID = %d\n", pidFiglio);
	else exit(3);

	if (WIFEXITED(status) == 0)
    		printf("Errore in status\n");
	else
    		printf("Per il figlio %d lo stato di EXIT e` %d\n", pid, WEXITSTATUS(status));

	exit(0);
} 
