/* figlio-init.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
   int pid;

	/* si genera un processo figlio */
	if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	} 
	
	if (pid == 0) 
	{ 	/* figlio */ 
	   int n=0;
		printf("Esecuzione del figlio %d\n", getpid()); 
		/* per fare in modo che il figlio non termini subito gli facciamo aspettare un dato che deve essere fornito dall'utente */
		/* potevamo usare anche una sleep, ma in questo modo possiamo tenere più sotto controllo la situazione */
		printf("Fornisci un valore intero\n"); 
		scanf("%d", &n);
		printf("Numero letto dal figlio %d\n", n); 
		exit(0);
	}

	/* padre */
	printf("Ho generato il figlio %d. BYE BYE!!!\n", pid); 
	/* il padre termina senza fare la wait del figlio! */

	exit(0);
} 
