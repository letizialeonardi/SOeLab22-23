/* FILE: padreSenzaFigli.c*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main ()
{
	printf("Sono il processo padre con pid %d\n", getpid());

	/* padre (!?!) */
	if (wait((int *)0) < 0)	/* nonostante questo processo non abbia creato figli, invoca la primitiva wait; chiaramente il codice potrebbe essere, 'a monte', molto più complicato e potrebbe essere NON chiaro il fatto che non e' stato creato alcun processo figlio: quindi bisogna sempre controllare il valore di ritorno della wait! */
	{
		printf("Errore in wait\n");
		exit(1);
	}
	exit(0);	/* chiaramente in questo caso NON si eseguira' mai questa istruzione, dato che si uscira' sempre con errore */
}
