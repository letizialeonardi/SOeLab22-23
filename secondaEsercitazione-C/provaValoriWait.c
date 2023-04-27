/* FILE: provaValoriWait.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main ()
{
     int pid;                           /* pid per fork */
     int pidFiglio, status, ritorno;    /* per wait padre */
     int valore;			/* valore per scanf da parte de figlio */

	if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	}

	if (pid == 0)
	{ 	/* figlio */
		printf("Esecuzione del figlio\n");
		/* questa volta facciamo fare qualche cosa al figlio */
		printf("Dammi un valore intero per provare la exit:\n");
		scanf("%d", &valore);
		if ((valore > 255) || (valore < 0)) 
			printf("ATTENZIONE IL VALORE SARA' TRONCATO!\n");
		else printf("Il valore fornito non verra' troncato!\n");
		exit(valore);
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
        	ritorno=(int)((status >> 8) & 0xFF);  
        	printf("Per il figlio %d lo stato di EXIT e` %d\n", pid, ritorno);
    	}

	exit(0);
}
