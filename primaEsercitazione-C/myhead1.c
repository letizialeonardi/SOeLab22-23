/* file myhead1.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main  (int argc, char **argv)
{   
    int i, n; 	/* i serve per contare le linee, n per sapere quante linee devono essere mostrate (deriva dall'opzione) */
    char c; 	/* per leggere i caratteri da standard input e scriverli su standard output */

	/* controllo numero di parametri */
	if (argc != 2) 	/* controllo stretto */
	{ 	
		printf("Errore: Necessario 1 argomento (nel formato -numero) per %s\n", argv[0]); 
		exit (1); 
	}

	if (argv[1][0] != '-')
	{ 	
		printf ("Errore: Necessario il simbolo di opzione\n");
		exit (2); 
	}

	n = atoi(&(argv[1][1]));	/* convertiamo il numero che parte dal secondo carattere e quindi escludendo il '-' */
	if (n <= 0)
	{
        	printf("Errore: l'opzione non e' corretta\n");
        	exit(3);
	}

	i = 1; /* inizializzo il conteggio delle linee a 1 */
	while (read (0, &c, 1))		/* lettura da standard input */
      	{	 	
		if (c == '\n') i++; 	/* se troviamo il terminatore di linea, incrementiamo il conteggio */
		write(1, &c, 1);	/* scriviamo comunque il carattere qualunque sia */
		if (i > n) break;	/* se il conteggio supera n, allora usciamo dal ciclo di lettura */ 
      	}

	exit(0);	
}
