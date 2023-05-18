/* FILE: pauseala.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define TRUE  1
#define FALSE 0
#define BELLS "\007\007\007"

int alarm_flag = FALSE;

void setflag(int signo)
{
	printf("STAMPA DI DEBUGGING: Processo con pid = %d ha ricevuto il segnale SIGALRM che e' il nro %d\n", getpid(), signo);
	alarm_flag = TRUE;
}

int main (int argc, char **argv)
{
   int pid;	/* per fork */
   int nsecs; 	/* per calcolo secondi */
   int j;	/* indice/contatore */

	if (argc <= 2) 
	{ 	printf ("Errore nel numero di parametri\n");
		 exit(1);
	}

	/* il primo parametro deve essere il numero di minuti che deve passare prima di far scattare l'allarme e il resto dei parametri sono le stringhe che saranno scritte sullo standard output quando scatta l'allarme */
	if ((nsecs = atoi(argv[1]) * 60 ) <= 0)
	{ 	printf ("Errore nel valore del tempo\n");
		exit(2);
	}

	if ((pid = fork()) < 0)
	{
		/* fork fallita */
		 printf("Errore in fork\n");
		 exit(1);
	}

	if (pid == 0)
	{
		/* figlio */
		/* installa l'azione specifica da eseguire alla ricezione del segnale SIGALRM */
		signal(SIGALRM, setflag);
		/* attiva l'allarme per il numero di secondi calcolato dal primo parametro (fornito in minuti) */
		alarm(nsecs);
		/* attende l'arrivo di un qualunque segnale */
		pause();
		/* se il segnale arrivato era quello dell'allarme, allora stampa su standard output le stringhe passate come parametri dal secondo in poi */
		if (alarm_flag == TRUE)
	      	{ 
			printf(BELLS); /* viene suonata la 'campanella' per 3 volte */
			for (j=2; j < argc; j++)
				printf("%s ", argv[j]);
			printf("\n");
	      	}
		exit(0); 	/* figlio termina */
	}

	/* padre */
	printf("Creazione del processo %d\n", pid);
	/* padre termina: simulazione esecuzione in background */
	exit(0);
}
