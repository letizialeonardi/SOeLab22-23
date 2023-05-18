/* FILE: pausekill.c  */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int ntimes = 0; /* variabile globale */

void handler(int signo)
{
	printf ("Processo %d ricevuto #%d volte il segnale %d\n", getpid(), ++ntimes, signo);
}

int main ()
{
   int pid, ppid;	/* pid figlio e padre */
	
   	/* agganciamo il segnale SIGUSR1 alla funzione gestore di nome handler */
	signal(SIGUSR1, handler);

	if ((pid = fork()) < 0) 
		/* fork fallita */
		exit(1);

	if (pid == 0)  /* figlio */
	{ 	/* l'aggancio al segnale viene ereditato */
		ppid = getppid(); /* PID del padre */
		for (;;) /* ciclo infinito */
		{ 
			printf("FIGLIO %d\n", getpid());
			sleep(1);
			kill(ppid, SIGUSR1);
			pause();
		}	
	}
	else 	/* ATTENZIONE IN QUESTO CASO DOBBIAMO METTERE l'ELSE PERCHE' IL FIGLIO NON ESEGUE EXIT MA UN CICLO INFINITO */
	/* padre */
	{ 
		for(;;) /* ciclo infinito */
		{ 
			printf("PADRE %d\n", getpid());
			pause();
			sleep(1);
			kill(pid, SIGUSR1);
		}
	}
	
	/* NOTA BENE: il padre NON può aspettare il figlio in questo caso! */
}
