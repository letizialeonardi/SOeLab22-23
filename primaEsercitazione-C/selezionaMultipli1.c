/* file selezionaMultipli1.c */
/* VERSIONE CHE LEGGE n CARATTERI VIA VIA DAL FILE E RIPORTA SU STANDARD OUTPUT SOLO L'ULTIMO */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main  (int argc, char **argv)
{ 	
   int n; 		/* lunghezza corrispondente al secondo parametro */
   int nr; 		/* per la read */
   int i; 		/* indice del multiplo */
   char *buffer; 	/* buffer DINAMICO per leggere i caratteri dal file */
   int fd; 		/* per la open */

	/* conptrollo numero di parametri */
	if (argc != 3)
	{ 	
	        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		printf("Ci vogliono 2 argomenti: nome-file e numero strettamente positivo\n");
		exit (1); 
	}

	/* controllo primo parametro: apriamo il file in lettura */
	fd = open(argv[1], O_RDONLY); 
	if (fd < 0) 
	{ 
		printf("Errore: FILE %s NON ESISTE\n", argv[1]); 
		exit(2);
	}

	/* controllo secondo parametro */
	n = atoi(argv[2]); 	/* usiamo la funzione di libreria atoi: se si riesce a convertire in un numero strettamente positivo lo consideriamo un controllo sufficiente */
	if (n <= 0)
	{
        	printf("Errore: Il secondo parametro non e' un numero strettamente maggiore di 0\n");
        	exit(3);
	}

	/* allochiamo la memoria necessaria per il buffer sulla base del valore di n */
	buffer = (char *)(malloc(n * sizeof(char)));
	if (buffer == NULL)	/* BISOGNA SEMPRE CONTROLLARE IL VALORE DI RITORNO DELLA FUNZIONE malloc! */
	{
        	printf("Errore: problemi nella malloc\n"); 
		exit(4);
	}

	printf("Stiamo per selezionare i caratteri multipli di %d\n", n);
	i = 1; /* valore iniziale del multiplo */
	while ((nr=read (fd, buffer, n)) > 0)
	{ 	if (nr == n) 
		{ 
	 		/* possiamo selezionare l'n-esimo carattere */
			printf("Il carattere multiplo %d-esimo all'interno del file %s e' %c\n", i, argv[1], buffer[n-1]); 
			i++;    /* incrementiamo il conteggio */
		}
		else printf("Il file non ha una dimensione multipla di %d\n", n);
	}	

	exit(0);	
}
