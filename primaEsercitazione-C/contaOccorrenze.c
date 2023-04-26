/* file contaOccorrenze.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main  (int argc, char **argv)
{ 	
    long int totale=0; 	/* serve per calcolare il numero di occorrenze: bisogna usare un long int come specificato nel testo */
    char Cx; 		/* serve per salvare il carattere, selezionandolo dal secondo parametro, che va cercato nel file */
    int fd; 		/* per la open */
    char c; 		/* per leggere i caratteri dal file */

	/* controllo numero di parametri */
	if (argc != 3) /* controllo stretto: argc deve essere esattamente 3 (2+1)! */
	{ 	
		printf("Errore: Necessari esattamente 2 parametri per %s e invece argc=%d\n", argv[0], argc); 
		exit(1); 
	}	

	/* controllo primo parametro: apriamo il file */
	if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
		printf("Errore: FILE %s NON ESISTE\n", argv[1]);
		exit(2);
	}

	/* controllo secondo parametro */
	if (strlen(argv[2]) != 1)
	{	 	
		printf("Errore: Il secondo parametro %s non e' un singolo carattere\n", argv[2]);
		exit(3); 
	}
	/* isoliamo il singolo carattere per maggior comodita' */
	Cx=argv[2][0];

	printf("Eseguo il programma %s con parametri %s e %c\n", argv[0], argv[1], Cx); 

	/* leggiamo il file UN SINGOLO CARATTERE ALLA VOLTA */
	while (read (fd, &c, 1))
	/* o anche while (read (fd, &c, 1) != 0) */
	{	if (c == Cx) 
			totale++;     /* se troviamo il carattere incrementiamo il conteggio */
	}

	/* stampa finale, attenzione, con %ld */
	printf ("Il numero totale di occorrenze del carattere %c nel file %s e' %ld\n", Cx, argv[1], totale);

	exit(0);	
}
