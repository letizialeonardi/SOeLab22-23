/* Soluzione della Prova d'esame del 8 Settembre 2021: soluzione con due array di pipe */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char **argv) 
{
	/* -------- Variabili locali ---------- */
	int pid;			/* process identifier per le altre fork() */
	int N; 				/* numero di file passati sulla riga di comando */
	int nro; 			/* massimo delle lunghezze delle linee calcolate da ogni figlio */
	int nroLinea; 			/* numero delle linee calcolate da ogni figlio e dal padre */
	int fcreato; 			/* file descriptor per creazione file da parte del padre */
	int fd; 			/* file descriptor per apertura file */
	int status, ritorno;		/* variabili per la wait */
	pipe_t *pari;			/* array dinamico di pipe descriptors per comunicazioni figli pari-padre */
	pipe_t *dispari;		/* array dinamico di pipe descriptors per comunicazioni figli dispari-padre */
	int n;				/* indice per i figli */
	int i, j;			/* indici per i cicli */
	char linea[250];		/* array di caratteri per memorizzare la linea: come indicato dal testo si puo' supporre una lunghezza massima di ogni linea di 250 caratteri compreso il terminatore di linea */
	int nr1, nr2; 			/* variabili che vengono usate dal padre per sapere se i figli hanno inviato qualcosa */
	int L1, L2; 			/* variabili che vengono usate dal padre per recuperare le lunghezze inviate dai figli */
	/* ------------------------------------ */

	/* Controllo sul numero di parametri */
	if (argc < 2) /* Meno di un parametro */  
	{
		printf("Errore nel numero dei parametri\n");
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;
	printf("Sono il padre con pid %d e %d file e creero' %d processi figli\n", getpid(), N, 2*N);
	
	/* PER PRIMA COSA creazione file con mio cognome e terminazione log */
	if ((fcreato=creat("leonardi.log", 0644)) < 0)  
	{
              	printf("Errore nella creat del file %s\n", "leonardi.log");
                exit(2);
	}
 
	/* Allocazione dei due array di N pipe descriptors*/
	pari = (pipe_t *) malloc (N*sizeof(pipe_t));
	if (pari == NULL)
	{
		printf("Errore nella allocazione della memoria per le pipe pari\n");
		exit(3);
	}
	dispari = (pipe_t *) malloc (N*sizeof(pipe_t));
	if (dispari == NULL)
	{
		printf("Errore nella allocazione della memoria per le pipe dispari\n");
		exit(4);
	}
	
	/* Creazione delle N pipe figli pari-padre e figli dispari-padre*/
	for (i=0; i < N; i++)
	{
		if(pipe(pari[i]) < 0)
		{
			printf("Errore nella creazione della pipe pari\n");
			exit(5);
		}
		if(pipe(dispari[i]) < 0)
		{
			printf("Errore nella creazione della pipe dispari\n");
			exit(6);
		}
	}

	/* Ciclo di generazione dei figli: NOTA BENE DEVONO ESSERE 2 * N  */
	for (n=0; n < 2*N; n++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork del figlio %d-esimo\n", n);
			exit(5);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("Sono il processo figlio di indice %d e pid %d e sono associato al file %s\n", n, getpid(), argv[n/2+1]);
			/* con questa soluzione le chiusure delle pipe si complicano rispetto ad avere un solo array di 2*N pipe */
			if ((n%2) == 0) /* pari */
			{
				/* Chiusura delle pipe non usate nella comunicazione con il padre */
				for (j=0; j < N; j++)
				{
					close(pari[j][0]);
					if (n/2 != j) close(pari[j][1]);
					close(dispari[j][0]);
					close(dispari[j][1]);
				}
                       	}
			else /* dispari */
			{
				/* Chiusura delle pipe non usate nella comunicazione con il padre */
				for (j=0; j < N; j++)
				{
					close(dispari[j][0]);
					if (n/2 != j) close(dispari[j][1]);
					close(pari[j][0]);
					close(pari[j][1]);
				}
                       	}

			/* dopo la consegna, ci si e' accorti che i vari processi potevano direttamente individuare il file associato usando l'indice n/2+1 senza bisogn
o di distinguere caso indice dispari o pari e quindi la soluzione riporta questa semplicificazione; chiaramente le consegne che prevedevano codice differenziato sono st
ate considerate corrette! */
			if ((fd=open(argv[n/2+1], O_RDONLY)) < 0)
			{
                               	printf("Errore nella open del file %s\n", argv[n/2+1]);
                               	exit(-1); /* in caso di errore nei figli decidiamo di tornare -1 che corrispondera' per il padre al valore 255 che supponiamo non essere un valore accettabile di ritorno */
                       	}

			/* adesso il figlio legge dal file una linea alla volta */
			j=0; 		/* azzeriamo l'indice della linea */
			nroLinea=0; 	/* azzeriamo il numero della linea */
			nro=-1;		/* settiamo il massimo a -1 */
		        while (read(fd, &(linea[j]), 1))
			{
				if (linea[j] == '\n') 
			 	{ 
					/* dobbiamo mandare al padre la lunghezza della linea selezionata compreso il terminatore di linea (come int) e quindi incrementiamo j */
				   	j++;
					nroLinea++; /* la prima linea sarà la numero 1! */
					if ( ((n%2) != 0) && ((nroLinea%2) != 0) ) /* processo associato a linea dispari e numero di linea dispari */
					{
				   		write(dispari[n/2][1], &j, sizeof(j));
						/* verifichiamo e nel caso aggiorniamo il massimo */
						if (j > nro) 
							nro=j; 
				  	}
					if ( ((n%2) == 0) && ((nroLinea%2) == 0) ) /* processo associato a linea pari e numero di linea pari */
					{
				   		write(pari[n/2][1], &j, sizeof(j));
						/* verifichiamo e nel caso aggiorniamo il massimo */
						if (j > nro) 
							nro=j; 
				  	}
				   	j=0; /* azzeriamo l'indice per le prossime linee */
				}
				else j++; /* continuiamo a leggere */
			}
			/* ogni figlio deve ritornare al padre il valore corrispondente al massimo */
			exit(nro);
  			}	
		}
	
	/* Codice del padre */
	/* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
 	{
		close(pari[i][1]);
		close(dispari[i][1]);
	}

	/* Il padre recupera le informazioni dai figli: prima in ordine di file e quindi di linee */
	for (i=0; i < N; i++)
	{
	       	nroLinea=1; /* la prima linea ha numero 1 */
	       	printf("Le lunghezze delle linee del file %s sono:\n", argv[i+1]);
		do
		{
			/* il padre recupera le lunghezze delle linee da ogni figlio dispari e pari */
	       		nr1=read(dispari[i][0], &L1, sizeof(L1));
			nr2=read(pari[i][0], &L2, sizeof(L2));
			if (nr1 != 0)
			{
				printf("linea numero %d e' lunga %d\n", nroLinea, L1);
				nroLinea++;		/* incrementiamo il numero di linea */
			}	
			if (nr2 != 0)
			{
				printf("linea numero %d e' lunga %d\n", nroLinea, L2);
				nroLinea++;		/* incrementiamo il numero di linea */
			}	

		} while (nr1||nr2);	
		/* in alternativa si poteva fare un while(1) ed inserire dei break quando la nr1 o nr2 arrivano a 0 */
	}	

	/* Il padre aspetta tutti i figli */
	for (n=0; n < 2*N; n++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore in wait\n");
		exit(9);
		}

		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 
			ritorno=(int)((status >> 8) &	0xFF); 
			/* dobbiamo preparare la stringa da scrivere sul file creato: N.B. usiamo sprintf e linea che tanto il padre non ha usato */
			sprintf(linea,"Il figlio con pid=%d ha ritornato %d (se 255 significa che il figlio e' terminato con un errore)\n", pid, ritorno);
			write(fcreato, linea, strlen(linea));
		}
	}
	exit(0);
}
