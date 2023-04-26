/* file selezionaLinea.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main  (int argc, char **argv)
{  int i, n; 		/* i serve per contare le linee, n lunghezza corrispondente al secondo parametro */
   int j; 		/* indice per il buffer che contiene la linea corrente */
   char buffer[256]; 	/* per leggere i caratteri dal file, supponendo che una linea non sia piu' lunga di 256 compreso il terminatore di linea ed il terminatore di stringa perche' trasformeremo la linea in una stringa per poterla stampare con printf */
   int fd; 		/* per la open */
   int trovata=0; 	/* indica se e' stata trovata la linea indicata */

	/* controllo numero di parametri */
	if (argc != 3)	/* controllo stretto */
	{ 	
		printf("Errore: Necessari 2 argomenti (nome file e numero di linea) per %s\n", argv[0]); 
		exit(1); 
	}

	/* controllo primo parametro: apriamo il file in lettura) */
	if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
		printf("Errore: FILE %s NON ESISTE\n", argv[1]);
		exit(2);
	}

	/* controllo secondo parametro */
	n = atoi(argv[2]); /* convertiamo il secondo parametro */
	if (n <= 0)
	{
        	printf("Errore: Il secondo parametro non e' un numero strettamente maggiore di 0\n");
        	exit(3);
	}

	i = 1; /* inizializzo il conteggio delle linee a 1 */
	j = 0; /* valore iniziale dell'indice del buffer */
	while (read (fd, &(buffer[j]), 1) != 0)		/* leggiamo un carattere alla volta e lo inseriamo nell'array buffer */
	{ 	
		if (buffer[j] == '\n') 	/* se il carattere corrente e' il terminatore di linea ... */
		{ 
			if (n == i) 	/* controlliamo se siamo sulla linea che dobbiamo selezionare */
			{ 	
				trovata=1;
				break; /* usciamo dal ciclo di lettura */
			}
			else		/* se non siamo sulla linea giusta */
  			{       
				j = 0; 	/* azzeriamo l'indice per la prossima linea */
		  		i++; 	/* e incrementiamo il conteggio delle linee */
               	 	}
      		}
		else j++;	/* altrimenti incrementiamo l'indice nel buffer */
	}	

	if (trovata)
	{	       
		/* dobbiamo prima rendere la linea una stringa */
		buffer[j+1] = '\0'; 		
		/* poi la stampiamo su standard output */
        	printf("La linea numero %d del file %s e':\n%s", n, argv[1], buffer);
	}
	else	/* se non abbiamo trovato la linea richiesta */
		printf("La linea numero %d non esiste in %s\n", n, argv[1]);

exit(0);
}
