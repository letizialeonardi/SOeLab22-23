/* file myhead3.c */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main  (int argc, char **argv)
{ 	int i, n; 	/* i serve per contare le linee, n per sapere quante linee devono essere mostrate (deriva dall'opzione) */
	char c; 	/* per leggere i caratteri da standard input o da file e scriverli su standard output */
	int fd; 	/* per la open */
	int par = 0; 	/* per tenere traccia se e' usato un nome di file o meno  */
	char *op, *nf; 	/* per rendere piu' leggibile il codice */

	/* controllo numero di parametri */
	if (argc > 3)	/* controllo lasco */
	{ 	
		printf("Errore: Necessario 0, 1 oppure 2 argomenti per %s\n", argv[0]); 
		exit(1); 
	}

	switch (argc) 
	{
		case 3: op = argv[1]; nf = argv[2];	/* caso opzione e nome file */
			par = 1;			/* abbiamo il nome di un file */
			if (op[0] != '-')
			{ 	
				printf ("Errore: Necessario il simbolo di opzione\n");
				exit(2); 
			}
                        n = atoi(&(op[1]));	/* convertiamo */
                        if (n <= 0)
                        {
                                printf("Errore: l'opzione non e' corretta\n");
                                exit(3);
                        }
			break;

		case 2: op = argv[1];
			if (op[0] != '-') 	/* abbiamo solo il nome del file e non abbiamo l'opzione */
			{ 	nf = op; 
				n = 10; 
				par = 1;
			}
			else	/* abbiamo l'opzione e NON il nome del file */
                	{
                        	n = atoi(&(op[1])); 	/* convertiamo */
                        	if (n <= 0)
                        	{
                                	printf("Errore: l'opzione non e' corretta\n");
                                	exit(4);
                        	}
                	}
			break;

		case 1: n = 10;  /* NON abbiamo ne' opzione ne' nome file */
			break;
	}

	if (par == 1)	/* se abbiamo un nome di file dobbiamo aprire il file */
	{ 	
		if ((fd = open(nf, O_RDONLY)) < 0) 
		{	 
			printf("Errore: FILE NON ESISTE\n"); 
			exit(5);
		}
	}
	else
   		fd = 0; /* si deve considerare lo standard input nella lettura */

	i = 1; /* inizializzo il conteggio delle linee a 1 */
	while (read (fd, &c, 1) != 0)
      	{ 	
		if (c == '\n') i++; 	/* se troviamo un terminatore di linea incrementiamo il conteggio */
		write(1, &c, 1);	/* scriviamo comunque il carattere qualunque sia */
		if (i > n) break;	/* se il conteggio supera n allora usciamo dal ciclo di lettura */ 
      	}

	exit(0);	
}
