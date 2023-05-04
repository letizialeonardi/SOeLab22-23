/* FILE: mylsConFork1.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char** argv)
{
   int pid;				/* per fork */
   int pidFiglio, status, ritorno;	/* per wait padre */

        if (argc != 2)
        {
                printf("Errore nel numero di parametri che deve essere esattamente uno (nome di file o directory): %d\n", argc);
                exit(1);
        }

	/* generiamo un processo figlio dato che stiamo simulando di essere il processo di shell! */
        if ((pid = fork()) < 0)
	{ 	/* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	}

	if (pid == 0)
	{ 	/* figlio */
		printf("Esecuzione di ls -l da parte del figlio con pid %d con parametro %s\n", getpid(), argv[1]);
		execlp("ls", "ls", "-l", argv[1], (char *)0); /* il processo sotto-shell usa sempre la versione con p! */
		/* si esegue l'istruzione seguente SOLO in caso di fallimento della execlp */
		printf("Errore in execlp\n");
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
       		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
	}	

	exit(0);
}
