/* FILE:  mylsErrato.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
	printf("Esecuzione di ls: versione ERRATA!\n");  
	execl("/bin/ls1", "ls", "-l", (char *)0); 	/* abbiamo scritto il nome dell'eseguibile in modo scorretto e quindi la execl fallira' */

	/* si esegue l'istruzione seguente SOLO in caso di fallimento della execl */
	printf("Errore in execl\n");
	exit(1); 	 /* torniamo quindi un valore diverso da 0 per indicare che ci sono stati dei problemi */
}
