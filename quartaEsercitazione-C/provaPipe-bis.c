/* FILE: provaPipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char** argv)
{  	
	int fd1, fd2; 	/* variabili per valore di ritorno open */
	int piped[2]; 	/* array di due interi per la pipe */

	if (argc != 3) 		/* per prima cosa (come negli script) controlliamo il numero di parametri ==> ce ne deve essere esattamente 2! */
	{ 	printf("Errore nel numero di parametri dato che argc = %d\n", argc);
		exit(1); 	/* in caso di errore (esattamente come negli script) dobbiamo uscire tornando sempre un numero diverso! */
	}

	if ((fd1 = open(argv[1], O_RDONLY)) < 0)  	/* dobbiamo sempre controllare il valore di ritorno delle open: ATTENZIONE ALLE PARENTESI! */
	{ 	printf("Errore in apertura file dato che fd1 = %d\n", fd1);
        	exit(2); 
	}	
	/* NOTA BENE: non serve l'else perche' in caso di errore di fa la exit! */
	printf("Valore di fd1 = %d\n", fd1);

	if ((fd2 = open(argv[2], O_RDONLY)) < 0)	/*  apriamo un altro file per verificare che verra' usato un altro elemento della tabella dei file aperti del singolo processo! */
	{ 	printf("Errore in apertura file dato che fd2 = %d\n", fd2);
        	exit(3); 
	}
	printf("Valore di fd2 = %d\n", fd2);

	close(fd1);	/* chiudiamo e quindi eliminiamo le informazioni relative all'elemento della tabella dei file aperti identificato da fd1 */

	/* si crea una pipe */
	if (pipe (piped) < 0 ) 
	{ 
                printf("Errore nella creazione pipe\n");
		exit (3); 
	}
	printf("creato pipe con piped[0]= %d \n", piped[0]);
	printf("creato pipe con piped[1]= %d \n", piped[1]);

	exit (0);
}
