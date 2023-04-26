/* file parametri2.c */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main  (int argc, char **argv)
{ 	
   int fd; 	/* per la open */
   int N;	/* serve per convertire il secondo parametro */
   char C; 	/* serve per selezionare il terzo parametro */

	/* controllo numero di parametri */
	if (argc != 4)	/* controllo stretto: argc deve essere esattamente 4 (3+1)! */
	{ 	
		printf("Errore: Necessari esattamente 3 parametri per %s e invece argc=%d\n", argv[0], argc); 
		exit(1); 
	}

	/* controllo primo parametro: dato che viene detto che deve essere considerato il nome di un file andiamo a provare ad aprirlo (nella modalita' piu' tipica e cioe' sola lettura) */
	if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
        	printf("Errore: FILE %s NON ESISTE\n", argv[1]); 
		exit(2); 	/* come in SHELL sempre valori di ritorni differenti! */
	}

	/* controllo secondo parametro */
	N=atoi(argv[2]);	/* usiamo la funzione di libreria atoi: se si riesce a convertire in un numero strettamente positivo lo consideriamo un controllo sufficiente */
	if (N <= 0) 
	{ 	
		printf("Errore: Il secondo parametro %s non e' un numero strettamente maggiore di 0\n", argv[2]); 
		exit(3); 
	}

	/* controllo terzo parametro */
	if (strlen(argv[3]) != 1)	/* in alternativa si puo' anche verificare il carattere di posizione [1] della stringa argv[3]: se '\0' va bene, altrimenti errore! */
	{ 	
		printf("Errore: Il terzo parametro %s non e' un singolo carattere\n", argv[3]); 
		exit(4); 
	}

	/* isoliamo il singolo carattere per maggior comodita' */
	C=argv[3][0];
	printf("Eseguo il programma %s con %d parametri\n", argv[0], argc - 1); 
/* in questo caso, dato che il numero di parametri e' noto e sono comunque di 'tipo' diverso non usiamo un for per riportare il valore dei singoli parametri */
	printf ("Il primo parametro e' il nome di un file %s\nIl secondo parametro e' il numero strettamente positivo %d\nIl terzo parametro e' il singolo carattere %c\n", argv[1], N, C);

	exit(0);	
}
