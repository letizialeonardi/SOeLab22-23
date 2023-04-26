/* file selezionaLunghezzaLinea.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main  (int argc, char **argv)
{   int i, n; 		/* i serve per contare le linee, n lunghezza corrispondente al secondo parametro */
    int j; 		/* indice per il buffer che contiene la linea corrente */
    char buffer[256]; 	/* per leggere i caratteri dal file, supponendo che una linea non sia piu' lunga di 256 compreso il terminatore di linea e il terminatore di stringa perche' trasformeremo la linea in una stringa per poterla stampare con printf */
    int fd; 		/* per la open */
    int trovata=0; 	/* indica se e' stata trovata almeno una linea della lunghezza indicata */

	/* controllo numero di parametri */
     	if (argc != 3)  /* controllo stretto */
	{
		printf("Errore: Necessari 2 argomenti (nome file e numero di linea) per %s\n", argv[0]);
		exit (1); 
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
	  		if (n == j+1)   /* controlliamo se la lunghezza della linea correnta ha la lunghezza giusta e quindi la stampiamo dopo averla resa una stringa. N.B.: in questo caso la stampa va fatta dentro al ciclo perche' potremmo chiaramente trovare piu' linee della lunghezza cercata! */
			{ 	buffer[j+1] = '\0'; /* dobbiamo prima rendere la linea una stringa */
				printf("La linea numero %d del file %s ha la lunghezza cercata cioe' %d:\n%s", i, argv[1], n, buffer); 
				trovata=1;
			}
			j = 0; /* azzeriamo l'indice per la prossima linea */
		  	i++; 	/* e incrementiamo il conteggio delle linee */
      		}
		else j++;
	}

	if (!trovata)
		printf("Non e' stata trovata alcuna linea nel file %s di lunghezza %d\n", argv[1], n);

	exit(0);	
}
