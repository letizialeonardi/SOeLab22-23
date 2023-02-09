#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/* FILE: myGrepConFork-ridStError.c */
int main (int argc, char** argv)
{
   int pid;             /* per valore di ritorno della fork */
   int pidFiglio;       /* per valore di ritorno della wait */
   int status;          /* per usarlo nella wait */
   int ritorno;         /* per filtrare valore di uscita del figlio  */

	if (argc != 3)
	{
       		printf("Errore nel numero di parametri che devono essere due (stringa da cercare e nome del file dove cercare): %d\n", argc);
       		exit(1);	
	}

	/* generiamo un processo figlio dato che stiamo simulando di essere il processo di shell! */
        if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(2);
	}

	if (pid == 0)
	{ 	/* figlio */
		printf("Esecuzione di grep da parte del figlio con pid %d\n", getpid());
		/* ridirezionamo lo standard output su /dev/null perche' ci interessa solo se il comando grep ha successo o meno */
		close(1);
		open("/dev/null", O_WRONLY);
		/* ridirezionamo anche lo standard error su /dev/null perche' ci interessa solo se il comando grep ha successo o meno */
		close(2);
		open("/dev/null", O_WRONLY);
		execlp("grep", "grep", argv[1], argv[2], (char *)0);

 		/* si esegue l'istruzione seguente SOLO in caso di fallimento della execlp */
		/* ATTENZIONE SE LA EXEC FALLISCE NON HA SENSO FARE printf("Errore in execlp\n"); DATO CHE LO STANDARD OUTPUT E' RIDIRETTO SU /dev/null e, in questo caso, non ha senso neanche scrivere sullo standard error che e' stato anch'esso ridiretto! */
       	 	exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
	}	

	/* padre aspetta subito il figlio appunto perche' deve simulare la shell e la esecuzione in foreground! */		
	if ((pidFiglio=wait(&status)) < 0)
	{
      		printf("Errore wait\n");
      		exit(3);
	}
	if ((status & 0xFF) != 0)
        	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
	else
	{
       		ritorno=(int)((status >> 8) & 0xFF);
       		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
	}

	exit (0);
}
