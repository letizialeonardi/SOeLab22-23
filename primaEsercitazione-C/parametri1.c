/* file parametri1.c */

#include <stdio.h>
#include <stdlib.h>

int main  (int argc, char **argv)
{ 	
   int i; /* i serve per scorrere i parametri */
   int N; /* N serve per salvare il numero dei parametri */

	/* controllo numero di parametri */
	if (argc < 2) 	/* controllo lasco: ci deve essere almeno un parametro e quindi argc deve essere >= 2, altrimenti errore */
	{ 	
		printf("Errore: Necessario almeno 1 parametro per %s e invece argc=%d\n", argv[0], argc); 
		exit(1); 
	}

	N = argc - 1; 	/* ricordarsi che argc conta sempre anche il nome dell'eseguibile */

	printf("Eseguo il programma %s con %d parametri\n", argv[0], N); 

	for (i = 0; i < N; i++)						/* facciamo partire l'indice del for da 0 e poi usiamo pero' i+1 */
		printf ("Il parametro di indice %d e' %s\n", i+1, argv[i+1]);
	exit(0);	
}
