#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv)
{       
        int i=0; 	/* variabile contatore dentro il ciclo 'teoricamente' infinito */
	int fd;		/* variabile per valore di ritorno open */

	if (argc != 2) 	 /* per prima cosa (come negli script) controlliamo il numero di parametri ==> ce ne deve essere esattamente 1! */
	{       printf("Errore nel numero di parametri dato che argc = %d\n", argc);
		exit(1);        /* in caso di errore (esattamente come negli script) dobbiamo uscire tornando sempre un numero diverso! */
	}

	while (1)	/* ciclo infinito */
	{ 	
		if ((fd = open(argv[1], O_RDONLY)) < 0)	/* OSSERVAZIONE: apriamo sempre lo stesso file per verificare la dimensione della tabella dei file aperti del singolo processo! */
		{       printf("Errore in apertura file dato che fd = %d\n", fd);
		/* dopo aver segnalato l'errore e prima di uscire stampiamo il valore corrente dell'indice */
			printf("Valore di i = %d\n", i);
                	exit(2); 
		}
		else i++;
	}
	exit(0);
}
