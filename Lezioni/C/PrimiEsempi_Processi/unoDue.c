/* FILE: unoDue.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	printf("UNO\n");	/* l'unico processo (il padre) stampa la stringa "UNO" */

	fork();			/* l'unico processo (il padre) crea un processo figlio */

	printf("DUE\n");	/* che succede dopo la fork? */

	exit(0);		/* quanti processi eseguono questa exit? */
}
