/* Soluzione della Prova d'esame del 5 Giugno 2015 - SOLO Parte C */
/* VERSIONE RIDOTTA SENZA CREAZIONE NIPOTI */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef int pipe_t[2];	/* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char **argv) 
{
   /* -------- Variabili locali ---------- */
   int pid;			/* process identifier per le fork() */
   int M; 			/* numero di file passati sulla riga di comando (uguale al numero di file) */
   int status;			/* variabile di stato per la wait */
   pipe_t *piped;		/* array dinamico di pipe descriptors per comunicazioni figli-padre  */
   int j, k;			/* indici per i cicli */
   int lunghezza; 		/* variabile che viene comunicata da ogni figlio al padre */
   int ritorno;                 /* variabile usata dal padre per recuperare valore di ritorno di ogni figlio */
   /* ------------------------------------ */
	
	/* Controllo sul numero di parametri */
	if (argc < 3) /* Meno di due parametri */  
	{
		printf("Errore nel numero dei parametri, dato che argc=%d\n", argc);
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	M = argc - 1;
	
	/* Allocazione dell'array di M pipe descriptors */
	piped = (pipe_t *) malloc (M*sizeof(pipe_t));
	if (piped == NULL)
	{
		printf("Errore nella allocazione della memoria\n");
		exit(2);
	}
	
	/* Creazione delle M pipe figli-padre */
	for (j=0; j < M; j++)
	{
		if(pipe(piped[j]) < 0)
		{
			printf("Errore nella creazione della pipe\n");
			exit(3);
		}
	}

	printf("DEBUG-Sono il processo padre con pid %d e sto per generare %d figli\n", getpid(), M);
		
	/* Ciclo di generazione dei figli */
	for (j=0; j < M; j++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork\n");
			exit(4);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("DEBUG-Sono il processo figlio di indice %d e pid %d sto per creare il nipote che recuperera' l'ultima linea del file %s\n", j, getpid(), argv[j+1]);
			/* in caso di errori nei figli o nei nipoti decidiamo di tornare dei numeri negativi (-1 che corrispondera' per il padre al valore 255, -2 che corrispondera' a 254, etc.) che non possono essere valori accettabili di ritorno dato che il comando tail, usato avendo implementato la ridirezione in ingresso, puo' tornare solo 0 (perche' avra' sempre successo) */

			/* Chiusura delle pipe non usate nella comunicazione con il padre */
			for (k=0; k < M; k++)
			{
				close(piped[k][0]);
				if (k != j) close(piped[k][1]);
			}

			lunghezza=3000+j;
			/* il figlio comunica al padre */
			write(piped[j][1], &lunghezza, sizeof(lunghezza));

			exit(0);
		}
	}
	
	/* Codice del padre */
	/* Il padre chiude i lati delle pipe che non usa */
	for (j=0; j < M; j++)
		close(piped[j][1]);

	/* Il padre recupera le informazioni dai figli in ordine inverso di indice */
	for (j=M-1; j >= 0; j--)
	{
		/* il padre recupera tutti i valori interi dai figli */
		read(piped[j][0], &lunghezza, sizeof(lunghezza));
		printf("Il figlio di indice %d ha comunicato il valore %d per il file %s\n", j, lunghezza, argv[j+1]);
	}	

	/* Il padre aspetta i figli */
	for (j=0; j < M; j++)
	{
		if ((pid = wait(&status)) < 0)
		{
			printf("Errore in wait\n");
			exit(5);
		}

		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 	ritorno=(int)((status >> 8) &	0xFF); 
		  	if (ritorno!=0)
 				printf("Il figlio con pid=%d ha ritornato %d e quindi vuole dire che il nipote non e' riuscito ad eseguire il tail oppure il figlio o il nipote sono incorsi in errori\n", pid, ritorno);
		  	else  	printf("Il figlio con pid=%d ha ritornato %d\n", pid, ritorno);
		}
	}
	exit(0);
}
