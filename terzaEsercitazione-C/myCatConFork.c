/* FILE: myCatConFork.c */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <sys/wait.h>

int main (int argc, char** argv)
{
   int pid;             /* per valore di ritorno della fork */
   int pidFiglio;       /* per valore di ritorno della wait */
   int status;          /* per usarlo nella wait */
   int ritorno;         /* per filtrare valore di uscita del figlio  */

        if (argc != 2)
	{
		printf("Errore nel numero di parametri che deve essere uno (il nome del file da visualizzare): %d\n", argc);
		exit(1);
	 }

	/* generiamo un processo figlio dato che stiamo simulando di essere il processo di shell! */
	if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	}

	if (pid == 0)
	{ /* figlio */
	   int fd;

		/* simuliamo ridirezione dello standard input */
		close(0);
		if ((fd = open(argv[1], O_RDONLY)) < 0) 
		{ 	printf("ERRORE in apertura del file %s\n", argv[1]);
        		exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
		}
		printf("Ho aperto il file %s con fd = %d\n", argv[1], fd);
		printf("Esecuzione di programma che visualizza file gia` aperto con il programma mycat\n");
		execl("mycat", "mycat", (char *)0);

		/* si esegue l'istruzione seguente SOLO in caso di fallimento della execlp */
		printf("Errore in execl\n");
        	exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
	}	

	/* padre aspetta subito il figlio appunto perche' deve simulare la shell e la esecuzione in foreground! */
	if ((pidFiglio=wait(&status)) < 0)
	{
      		printf("Errore wait\n");
      		exit(2);
	}
	if ((status & 0xFF) != 0)
        	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
	else
	{
       		ritorno=(int)((status >> 8) & 0xFF);
       		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
	}

	exit(0);
}
