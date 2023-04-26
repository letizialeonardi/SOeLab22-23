/* FILE: provaWait.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main ()
{
   int pid, pidFiglio; /* pid per fork e pidFiglio per wait */

	if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	}

	if (pid == 0)
	{ 	/* figlio */
		printf("Esecuzione del figlio\n");
		sleep(4); 	/* si simula con un ritardo di 4 secondi che il figlio faccia qualcosa! */
		exit(5);	/* si torna un valore che si suppone possa essere derivante dall'esecuzione di un compito assegnato al figlio */
	}

	/* padre */
	printf("Generato figlio con PID = %d\n", pid);
	/* il padre aspetta il figlio disinteressandosi del valore della exit del figlio */
	if ((pidFiglio=wait((int *)0)) < 0)
	{
		printf("Errore in wait\n");
		exit(2);
	}

	if (pid == pidFiglio) 
		printf("Terminato figlio con PID = %d\n", pidFiglio);
	else 
	{       /* problemi */
		printf("Il pid della wait non corrisponde al pid della fork!\n");
		exit(3);
	}

	exit(0);
}
