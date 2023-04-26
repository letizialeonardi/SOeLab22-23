/* FILE: status1.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 

int main ()
{
     int pid; 				/* pid per fork */
     int pidFiglio, status, ritorno;    /* per wait padre */

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

	/* il padre aspetta il figlio in questo caso interessandosi del valore della exit del figlio */
	if ((pidFiglio=wait(&status)) < 0)
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

	if ((status & 0xFF) != 0)
    		printf("Figlio terminato in modo involontario (cioe' anomalo)\n");
    	else
    	{
    		/* selezione del byte "alto" */
    		ritorno = status >> 8;
    		ritorno &= 0xFF;
		/* o anche direttamente:
		ritorno=(int)((status >> 8) & 0xFF);  */
    		printf("Per il figlio %d lo stato di EXIT e` %d\n", pid, ritorno);
    	}

	exit(0);
}
