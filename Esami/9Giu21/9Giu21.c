/* Soluzione della Prova d'esame del 9 Giugno 2021 - SOLO Parte C */
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
	int pidPrimo;			/* process identifier per la prima fork() */
	int pid;			/* process identifier per le altre fork() */
	int N; 				/* numero di file passati sulla riga di comando (uguale al numero di processi figli normali) */
	int X; 				/* numero che sara' la lunghezza in linee dei file passati sulla riga di comando */
	int fdout; 			/* file descriptor per creazione file da parte del padre */
	int fd; 			/* file descriptor per apertura file */
	int status;			/* variabile di stato per la wait */
	pipe_t	p;			/* pipe fra figlio che calcola lunghezza e padre */
	pipe_t *piped;			/* array dinamico di pipe descriptors per comunicazioni figli-padre */
	int n;				/* indice per i figli */
	int i, j;			/* indici per i cicli */
	char linea[200];		/* array di caratteri per memorizzare la linea: come indicato dal testo si puo' supporre una lunghezza massima di ogni linea di 200 caratteri compreso il terminatore di linea */
	int valore; 			/* variabile che viene usata dal padre per recuperare il valore comunicato da ogni figlio e che contiene la lunghezza della linea selezionata */
	int ritorno; 			/* variabile che viene ritornata da ogni figlio al padre e che contiene la lunghezza media delle linee (supposta strettamente minore di 255) */
	/* ------------------------------------ */

	printf("Sono il padre con pid %d\n", getpid());
	
	/* Controllo sul numero di parametri */
	if (argc < 3) /* Meno di due parametri */  
	{
		printf("Errore nel numero dei parametri\n");
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;
	
	/* PER PRIMA COSA creazione file in /tmp con mio nome e cognome */
	if ((fdout=creat("/tmp/LetiziaLeonardi", 0644)) < 0)  
	{
              	printf("Errore nella creat del file %s\n", "/tmp/LetiziaLeonardi");
                exit(2);
	}
 
	/* Creazione pipe fra figlio speciale che calcola lunghezza in linee e padre */
	if(pipe(p) < 0)
        {
               	printf("Errore nella creazione della pipe con figlio speciale\n");
                exit(3);
        }

	/* generiamo il processo figlio speciale che deve calcolare la lunghezza dei file: dato che i file e' noto che abbiano tutti la stessa lunghezza in linee questo figlio usera' il file argv[1], come indicato nel testo*/
	pidPrimo = fork();
	if (pidPrimo < 0)
	{       /* fork fallita */
        	printf("Errore nella prima fork\n");
        	exit(4);
	}

	if (pidPrimo == 0)
	{       /* figlio speciale che calcola lunghezza in linee di argv[1] */
        	printf("Esecuzione di wc -l da parte del figlio con pid %d\n", getpid());
        	/* ridirezionamo lo standard input in modo da leggere dal file passato come primo parametro */
        	close(0);
        	if (open(argv[1], O_RDONLY))
        	{       printf("Errore in apertura file %s\n", argv[1]);
                	exit(-1); /* ritorniamo -1 al padre che verra' interpretato come 255 e quindi come errore */
        	}
        	/* dobbiamo implementare il piping e quindi lo standard output lo dobbiamo agganciare al lato di scrittura della pipe p */
        	close(1);
        	dup(p[1]);
		close(p[0]); /* dobbiamo chiudere il lato di lettura che non viene usato */
		close(p[1]); /* possiamo chiudere il lato di scrittura che verra' usato tramite il fd 1 */
        	execlp("wc", "wc", "-l", (char *)0); /* passiamo come parametro solo l'opzione -l */

        	/* si esegue l'istruzione seguente SOLO in caso di fallimento della execlp */
        	/* ATTENZIONE SE LA EXEC FALLISCE NON HA SENSO FARE printf("Errore in execlp\n"); DATO CHE LO STANDARD OUTPUT E' AGGANCIATO ALLA pipe p ma dobbiamo usare perror*/
		perror("Errore nella esecuzone di wc -l");
        	exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
	}	

	/* padre chiude il lato che non usa di p */
	close(p[1]);
	/* il padre deve recuperare il valore calcolato dal wc -l dalla pipe leggendo tutti i caratteri (numerici) che saranno inviati dal figlio: poichè non si sa quanti caratteri saranno inviati il padre deve leggerli tutti; il padre puo' usare l'array linea */
	j=0;
        while (read(p[0], &(linea[j]), 1))
        {
        	j++;
        }
        /* converto l'array di char in stringa sostituendo allo '\n' il terminatore di stringa */
        if (j!=0) /*se il padre ha letto qualcosa */
        {
       		linea[j-1]='\0';
		X=atoi(linea);	/* dobbiamo convertire la stringa in numero */
		/* stampa di debugging NON richiesta */
		printf("Padre ricevuto valore %d\n", X);
	}
	else
      	{
		printf("Errore nel calcolo numero di linee da parte del figlio speciale\n");
		exit(5); /* se il figlio speciale NON ha inviato nulla NON si possono creare i figli normali e quindi il padre termina */
        }
	
	/* padre prima di creeare gli altri figli chiude da pipe usata con il figlio speciale */
	close(p[0]);

	/* Allocazione dell'array di N pipe descriptors*/
	piped = (pipe_t *) malloc (N*sizeof(pipe_t));
	if (piped == NULL)
	{
		printf("Errore nella allocazione della memoria\n");
		exit(6);
	}
	
	/* Creazione delle N pipe figli-padre */
	for (i=0; i < N; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella creazione della pipe\n");
			exit(7);
		}
	}

	/* Ciclo di generazione dei figli normali */
	for (n=0; n < N; n++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork del figlio %d-esimo\n", n);
			exit(8);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("Sono il processo figlio di indice %d e pid %d e sono associato al file %s\n", n, getpid(), argv[n+1]);
			/* Chiusura delle pipe non usate nella comunicazione con il padre */
			for (j=0; j < N; j++)
			{
				close(piped[j][0]);
				if (n != j) close(piped[j][1]);
			}

			if ((fd=open(argv[n+1], O_RDONLY)) < 0)
			{
                                printf("Errore nella open del file %s\n", argv[n+1]);
                                exit(-1); /* in caso di errore nei figli decidiamo di tornare -1 che corrispondera' per il padre al valore 255 che supponiamo non essere un valore accettabile di ritorno */
                       	}
			/* adesso il figlio legge dal file una linea alla volta */
			j=0; /* azzeriamo l'indice della linea */
		        while (read(fd, &(linea[j]), 1))
			{
				if (linea[j] == '\n') 
			 	{ 
					/* dobbiamo mandare al padre la lunghezza della linea selezionata  compreso il terminatore di linea (come int) e quindi incrementiamo j */
				   	j++;
				   	write(piped[n][1], &j, sizeof(j));
				   	/* e quindi la linea stessa; N.B. SBAGLIATO convertirla in stringa! */
				   	write(piped[n][1], linea, j);
					ritorno=j; /* salviamo la lunghezza corrente dato che dovremo ritornare al padre la lunghezza dell'ultima linea */
				   	j=0; /* azzeriamo l'indice per le prossime linee */
				}
				else j++; /* continuiamo a leggere */
			}
			/* ogni figlio deve ritornare al padre il valore corrispondente alla lunghezza dell'ultima linea inviata al padre */
			exit(ritorno);
  			}	
		}
	
	/* Codice del padre */
	/* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
		close(piped[i][1]);

	/* Il padre recupera le informazioni dai figli: prima in ordine di linee e quindi in ordine di indice */
        for (j=1; j <= X; j++)
		for (n=0; n < N; n++)
		{ 
			/* il padre recupera prima la lunghezza della linea da ogni figlio */
			read(piped[n][0], &valore, sizeof(valore));
			/* quindi recupera la linea selezionata dal ogni figlio */
			read(piped[n][0], linea, valore);
			/* adesso va scritta sul file; N.B. la scriviamo come linea e quindi SBAGIATO convertirla in stringa! */
			write(fdout, linea, valore);
		}	

	/* Il padre aspetta tutti i figli (quelli normali e lo speciale) e quindi N+1 */
	for (i=0; i < N+1; i++)
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
			if (pid == pidPrimo) /* facciamo una stampa differenziata per il figlio speciale */
				printf("Il figlio che ha calcolato la lunghezza in linee dei file con pid=%d ha ritornato %d (se 255 significa che il figlio e' terminato con un errore)\n", pid, ritorno);
		  
			else	
				printf("Il figlio con pid=%d ha ritornato %d (se 255 significa che il figlio e' terminato con un errore)\n", pid, ritorno);
		}
	}
	exit(0);
}
