/* Soluzione della Prova d'esame del 8 Giugno 2016 - Parte C */
/* QUESTA SOLUZIONE NON GESTISCE LATO PADRE IL CASO CHE UN FIGLIO TERMINI PERCHE' IL FILE ASSOCIATO NON ESISTE (IL FIGLIO PERO' CONTROLLA IL RISULTATO DELLA OPEN E TERMINA IN CASO DI ERRORE): SI PUO' IPOTIZZARE CHE POSSA ANDARE BENE VISTO CHE LA PARTE C VIENE INVOCATA DA UNA PARTE SHELL, CHE SE SVOLTA CORRETTAMENTE, INVIA NOMI DI FILE ESISTENTI */
/* SE SI PROVA QUESTO PROGRAMMA CON UN FILE CHE CON ESISTE IL PADRE AL TENTATIVO DI SCRIVERE SULLA PIPE DI CONNESSIONE DI QUEL FIGLIO RICEVE UN SIGPIPE E TERMINA */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

typedef int pipe_t[2];

int mia_random(int n)	
{
   int casuale;
	casuale = rand() % n;	/* con rand() viene calcolato un numero pseudo casuale e con l'operazione modulo n si ricava un numero casuale compreso fra 0 e n-1 */
	return casuale;
}

int main(int argc, char **argv) 
{
   /* -------- Variabili locali ---------- */
   int pid;			/* process identifier per le fork() */
   int N; 			/* numero di file passati sulla riga di comando (uguale al numero di figli) */
   int H; 			/* numero passato come ultimo parametro e che rappresenta la lunghezza in linee dei file passati sulla riga di comando */
   int fdout; 			/* file descriptor per creazione file da parte del padre */
   int fd; 			/* file descriptor per apertura file */
   int status;			/* variabile di stato per la wait */
   pipe_t *pipedFP;		/* array dinamico di pipe descriptors per comunicazioni figli-padre */
   pipe_t *pipedPF;		/* array dinamico di pipe descriptors per comunicazioni padre-figli */
   int i, j;			/* indici per i cicli */
   char linea[255];		/* array di caratteri per memorizzare la linea, supponendo una lunghezza massima di ogni linea di 255 caratteri compreso il terminatore di linea */
   int valore; 			/* variabile che viene usata dal padre per recuperare il valore comunicato da ogni figlio e che contiene la lunghezza della linea corrente */
   int giusto; 			/* variabile che viene usata dal padre per salvare per ogni linea il valore inviato dal figlio selezionato in modo random */
   int r; 			/* variabile usata dal padre per calcolare i valori random e dal figlio per ricevere il numero dal padre */
   int ritorno=0; 		/* variabile che viene ritornata da ogni figlio al padre e che contiene il numero di caratteri scritti sul file (supposta minore di 255): valore iniziale 0 */
   /* ------------------------------------ */
	
	/* Controllo sul numero di parametri */
	if (argc < 6) /* Meno di cinque parametri */  
	{
		printf("Errore nel numero dei parametri, dato che argc=%d\n", argc);
		exit(1);
	}

	srand(time(NULL)); /* inizializziamo il seme per la generazione random di numeri  */

	/* Calcoliamo il numero di file passati */
	N = argc - 2;
	/* convertiamo l'ultima stringa in un numero */
	H = atoi(argv[argc-1]);
	/* controlliamo il numero: deve essere strettamente positivo e minore di 255 */
	if ((H <= 0) || (H >= 255))
	{
		printf("Errore nel numero passato %d\n", H);
		exit(2);
	}

	/* creazione file in /tmp */
	if ((fdout=open("/tmp/creato", O_CREAT|O_WRONLY|O_TRUNC, 0644)) < 0) /* usato la open in versione estesa per azzerare il file nel caso esista gia' */
	{
                printf("Errore nella creazione del file %s\n", "/tmp/creato");
                exit(3);
       	}
	/* NOTA BENE: tutti i figli avranno la possibilita' di accedere in scrittura al file creato dal padre (quindi non devono fare alcuna open) e avendo il file pointer condiviso non ci sara' bisogno di spostare l'I/O pointere */ 	

	/* Allocazione dei due array di N pipe descriptors*/
	pipedFP = (pipe_t *) malloc (N*sizeof(pipe_t));
	pipedPF = (pipe_t *) malloc (N*sizeof(pipe_t));
	if ((pipedFP == NULL) || (pipedPF == NULL))
	{
		printf("Errore nella allocazione della memoria per le 2*N pipe\n");
		exit(4);
	}
	
	/* Creazione delle N pipe figli-padre e delle N pipe padre-figli */
	for (i=0; i < N; i++)
	{
		if(pipe(pipedFP[i]) < 0)
		{
			printf("Errore nella creazione della pipe figli-padre\n");
			exit(5);
		}
		if(pipe(pipedPF[i]) < 0)
		{
			printf("Errore nella creazione della pipe padre-figli\n");
			exit(6);
		}
	}

	printf("DEBUG-Sono il padre con pid %d e sto per generare %d figli\n", getpid(), N);
	
	/* Ciclo di generazione dei figli */
	for (i=0; i < N; i++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork\n");
			exit(7);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("DEBUG-Sono il processo figlio di indice %d e pid %d e sono associato al file %s\n", i, getpid(), argv[i+1]);
			/* Chiusura delle pipe non usate nella comunicazione con il padre */
			for (j=0; j < N; j++)
			{
				close(pipedFP[j][0]);
				close(pipedPF[j][1]);
				if (i != j)  {
						close(pipedFP[j][1]);
						close(pipedPF[j][0]);
						}
			}
			/* apertura del file associato in sola lettura */
			if ((fd=open(argv[i+1], O_RDONLY)) < 0)
			{
                                printf("Errore nella open del file %s\n", argv[i+1]);
                                exit(-1); /* decidiamo di tornare -1 che verra' interpretato dal padre come 255 e quindi un valore non ammissibile! */
				/* OSSERVAZIONE IMPORTANTE: in realta' per come e' lo schema di comunicazione di questo compito, se un figlio muore in questa fase, il padre nella fase di invio dell'indice (calcolato in modo random) riceve il segnale SIGPIPE e quindi se il segnale non viene trattato l'azione di default e' la terminazione del padre che quindi NON attende i figli e non recupera neanche i valori di ritorno */
                       	}
			/* adesso il figlio legge dal file una linea alla volta */
			j=0;
		        while (read(fd, &(linea[j]), 1))
			{
				if (linea[j] == '\n') 
			 	{ 	/* dobbiamo mandare al padre la lunghezza della linea corrente compreso il terminatore di linea (come int) e quindi incrementiamo j */
				   	j++;
				   	write(pipedFP[i][1], &j, sizeof(j));
				   	/* il figlio Pi deve leggere il valore inviato dal padre */
				   	read(pipedPF[i][0], &r, sizeof(r));
				   	/* il figlio Pi deve controllare l'ammissibilita' del valore inviato dal padre rispetto alla lunghezza della linea corrente */
				   	if (r < j)
  				   	{
				    		/* dato che si riceve un indice che varia da 0 alla lung - 1 scelta dal padre e dato che j rappresenta la lunghezza della linea corrente, l'indice ricevuto per essere ammissibile deve essere strettamente minore di j */
                        	   		/* stampa di controllo:
			           		printf("Processo di indice %d sto per scrivere carattere %c nel file\n", i, linea[r]);
				   		*/ 
				   		/* ogni figlio usa, per scrivere sul file, il valore di fdout ottenuto per copia dal padre e condivide l'I/O pointer con il padre e tutti i fratelli */
						write(fdout, &(linea[r]), 1);
						/* il figlio incrementa il valore di ritorno */
						ritorno++;
		                   	}
				   	else
                        	   		/* stampa di controllo:
                                   		printf("Processo di indice %d non scrive nulla nel file\n", i);
			           		*/ 
				        	; /* non si deve fare nulla */
				   	j=0; /* azzeriamo l'indice per le prossime linee */
				}
				else 	j++; /* continuiamo a leggere */
			}
			/* il figlio Pi deve ritornare al padre il valore corrispondente al numero di caratteri scritti sul file */
			exit(ritorno);
  		}	
	}
	
	/* Codice del padre */
	/* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
 	{
		close(pipedFP[i][1]);
		close(pipedPF[i][0]);
        }

	/* Il padre recupera le informazioni dai figli: prima in ordine di linee e quindi in ordine di indice */
	for (j=1; j <= H; j++)
	{ 
		/* il padre calcola in modo random l'indice del figlio di cui considerera' il valore inviato */
		r=mia_random(N);
                /* stampa di controllo */
		printf("DEBUG-Il numero generato in modo random (per la linea %d) compreso fra 0 e %d per selezionare il figlio e' %d\n", j, N-1, r);
		for (i=0; i < N; i++)
		{ 
			/* il padre recupera tutti i valori interi dai figli */
			read(pipedFP[i][0], &valore, sizeof(valore));
			/* ma si salva solo il valore del figlio identificato in modo random */
			if (i == r) 
				giusto=valore;
		}	
		/* il padre calcola in modo random l'indice della linea che inviera' a tutti i figli */
		r=mia_random(giusto);
                /* stampa di controllo */
		printf("DEBUG-Il numero generato in modo random compreso fra 0 e %d per selezionare l'indice della linea %d-esima e' %d\n", giusto-1, j, r);
		/* il padre deve inviare a tutti i figli l'indice */
		for (i=0; i < N; i++)
			write(pipedPF[i][1], &r, sizeof(r));
	}	

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		if ((pid = wait(&status)) < 0)
		{
			printf("Errore in wait\n");
			exit(8);
		}

		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 	ritorno=(int)((status >> 8) &	0xFF); 
		  	printf("Il figlio con pid=%d ha ritornato %d (se 255 ci sono stati errori nel figlio)\n", pid, ritorno);
		}
	}

	exit(0);
}

