/* FILE: myecho.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv)
{
   int i; 	/* indice per for */

	printf("Sono myecho\n");
	for (i=0; i < argc; i++)
		/* si riportano su standard output tutti i parametri di invocazione (oltre che il nome del programma eseguibile stesso) */
		printf("Argomento argv[%d]= %s\n", i, argv[i]);
	exit(0);
}
