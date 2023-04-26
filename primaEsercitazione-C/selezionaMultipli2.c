/* file selezionaMultipli2.c */
/* VERSIONE USA LSEEK PER SPOSTARSI NELLA POSIZIONE GIUSTA E QUINDI LEGGE VIA VIA SOLO IL CARATTERE n-ESIMO E LO RIPORTA SU STANDARD OUTPUT */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main  (int argc, char **argv)
{ 	
    int n; 			/* lunghezza corrispondente al secondo parametro */
    long int pos; 		/* per la lseek */
    long int lunghezza; 	/* per la lunghezza del file */
    int i; 			/* indice del multiplo */
    char ch; 			/* per leggere i caratteri dal filei: N.B. in questo caso NON abbiamo bisogno di un buffer DINAMICO */
    int fd; 			/* per la open */


	/* conptrollo numero di parametri */
	if (argc != 3)
	{ 	
	        printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
		printf("Ci vogliono 2 argomenti: nome-file e numero strettamente positivo\n");
		exit (1); 
	}

	/* controllo primo parametro: apriamo il file */
	fd = open(argv[1], O_RDONLY); 
	if (fd < 0) 
	{ 
		printf("Errore: FILE %s NON ESISTE\n", argv[1]); 
		exit(2);
	}

	/* controllo secondo parametro */
	n = atoi(argv[2]); 	/* usiamo la funzione di libreria atoi: se si riesce a convertire in un numero strettamente positivo o consideriamo un controllo sufficiente */
	if (n <= 0)
	{
        	printf("Errore: Il secondo parametro non e' un numero strettamente maggiore di 0\n");
        	exit(3);
	}

	/* calcoliamo la lunghezza del file */
	lunghezza=lseek(fd, 0L, SEEK_END); /* NOTA BENE: dopo l'invocazione di questa primitiva il file pointer si trova alla fine del file; non importa pero' riportarlo all'inizio dato che comunque all'interno del ciclo ci si spostera' sempre sul carattere 'giusto' */

	printf("Stiamo per selezionare i caratteri multipli di %d\n", n);
	i = 1; /* valore iniziale del multiplo */
	pos=0L; /* inizializzazione per entrare il ciclo */

	while (pos < lunghezza) /* se pos minore di lunghezza */
	{
		/* calcoliamo la posizione del carattere che deve essere letto */
        	pos=(long int)(i * n);
        	if (pos < lunghezza) 	/* dobbiamo controllare di essere sempre dentro al file */
		{
			/* chiamiamo la lseek passando come offset pos-1 dall'inizio del file: dobbiamo considerare -1 altrimenti leggeremmo il carattere sbagliato */
        		lseek(fd, pos-1, SEEK_SET);
        		read(fd, &ch, 1);
			printf("Il carattere multiplo %d-esimo all'interno del file %s e' %c\n", i, argv[1], ch); 
  			i++; 	/* incrementiamo il conteggio */
		}	
		else printf("Il file non ha una dimensione multipla di %d\n", n);
	}

	exit(0);	
}
