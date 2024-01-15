/* Soluzione della Prova d'esame del 8 Giugno 2022 - SOLO Parte C */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MSGSIZE 3	/* COSTANTE CON NOME INDICATO DAL TESTO E CON IL MINIMO VALORE POSSIBILE: 2 caratteri piu' il terminatore di linea/stringa */

typedef int pipe_t[2];

int main(int argc, char **argv) 
{
   /* -------- Variabili locali ---------- */
   int pid;			/* process identifier per le varie fork() */
   int N; 			/* numero di file passati sulla riga di comando (uguale al numero di processi figli) */
   int Npipe; 			/* numero di pipe */
   int fd; 			/* file descriptor per apertura file */
   int status;			/* variabile di stato per la wait */
   pipe_t *piped;		/* array dinamico di pipe descriptors per comunicazioni figli-padre */
   int n;			/* indice per i figli */
   int i, j;			/* indici per i cicli */
   char linea[MSGSIZE];		/* array di caratteri per memorizzare la linea: come indicato dal testo si puo' supporre una lunghezza di ogni linea di MSGSIZE caratteri compreso il terminatore di linea */
   char buffer[MSGSIZE];	/* array di caratteri per memorizzare la stringa ricevuta dal primo figlio (stessa dimensione di linea) */
   int ritorno; 		/* variabile che viene ritornata da ogni figlio al padre e che contiene la lunghezza media delle linee (supposta strettamente minore di 255); questa variabile viene usata anche dal padre per recuperare appunto il valore tornato dai singoli figli */
   /* ------------------------------------ */

	printf("DEBUG-Sono il padre con pid %d\n", getpid());
	
	/* Controllo sul numero di parametri */
	if (argc < 4) /* Meno di tre parametri */  
	{
		printf("Errore nel numero dei parametri dato che argc = %d\n", argc);
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;

	/* Calcoliamo il numero di pipe: ATTENZIONE CI VOGLIONO N-1 pipe! */
	Npipe = N - 1;
	
	/* Allocazione dell'array di Npipe pipe descriptors */
	piped = (pipe_t *) malloc (Npipe*sizeof(pipe_t));
	if (piped == NULL)
	{
		printf("Errore nella allocazione della memoria\n");
		exit(2);
	}
	
	/* Creazione delle Npipe pipe figli-padre */
	for (i=0; i < Npipe; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella creazione della pipe\n");
			exit(3);
		}
	}

	/* Ciclo di generazione dei figli */
	for (n=0; n < N; n++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork del figlio %d-esimo\n", n);
			exit(4);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("DEBUG-Sono il processo figlio di indice %d e pid %d e sono associato al file %s\n", n, getpid(), argv[n+1]);

			if (n == 0) /* se e' il primo figlio il codice e' differente */
			{
				/* Chiusura delle pipe non usate nella comunicazione */
				for (j=0; j < Npipe; j++)
				{
					close(piped[j][0]);
				}
			}
			else /* se siamo negli altri figli a parte il primo */
			{

				/* Chiusura delle pipe non usate nella comunicazione */
				for (j=0; j < Npipe; j++)
				{
					close(piped[j][1]);
					if ((n-1) != j) close(piped[j][0]);
				}
			}

			/* TUTTI i figli devono aprire il file associato */
			if ((fd=open(argv[n+1], O_RDONLY)) < 0)
			{
                                printf("Errore nella open del file %s\n", argv[n+1]);
                                exit(N); /* in caso di errore nei figli decidiamo di tornare un valore crescente da N in poi */
                       	}
			if (n == 0) /* se e' il primo figlio il codice e' differente */
			{
				/* il primo figlio legge dal file una linea alla volta: N.B. la lettura si puo' fare direttamente di MSGSIZE caratteri! */
		        	while (read(fd, linea, MSGSIZE))
				{
					/* dobbiamo trasformare la linea in una stringa */
					linea[MSGSIZE-1] = '\0'; 
					//printf("DEBUG-Sono il PRIMO processo figlio di indice %d e pid %d e ho letto dal file %s\n", n, getpid(), linea);
					for (j=0; j < Npipe; j++)
					{
						write(piped[j][1], linea, MSGSIZE);	/* il primo figlio deve mandare ogni linea (trasformata in stringa) a tutti i fratelli! */
                       			}
                       		}
                       	}
			else /* se siamo negli altri figli a parte il primo */
			{
				/* dobbiamo ricevere le stringhe inviate dal primo figlio */
				while (read(piped[n-1][0], buffer, MSGSIZE))
				{
			           	/* dato che il primo figlio ha inviato delle stringhe, il figlio corrente le puo' usare direttamente */
					//printf("DEBUG-Sono il processo figlio di indice %d e pid %d e ho letto dalla pipe %s\n", n, getpid(), buffer);
					/* anche gli altri figli devono leggere il proprio file una linea alla volta */
	                               	while (read(fd, linea, MSGSIZE))
					{
						/* dobbiamo trasformare la linea in una stringa */
						linea[MSGSIZE-1] = '\0';
						printf("DEBUG-Sono il processo figlio di indice %d e pid %d e ho letto dal file %s\n", n, getpid(), linea);

						/* ora dobbiamo creare un processo nipote */ 
                        			if ( (pid = fork()) < 0)	/* dobbiamo creare un nipote per ogni coppia di nomi di file */
			                       	{
			                       		printf("Errore nella fork di creazione del nipote\n");
			                                exit(N+1);
			                        }
			                        if (pid == 0)
			                        {
			                        	/* codice del nipote: attenzione all'indice da usare per associare ogni nipote al giusto file! */
			                                printf("DEBUG-Sono il processo nipote del figlio di indice %d e ho pid %d e faro' il diff fra i file %s e %s\n", i, getpid(), linea, buffer);
			                                /* chiusura della pipe non usata */
			                                close(piped[n-1][0]);

							/* ridirezionamo lo standard output su /dev/null perche' ci interessa solo se il comando diff ha successo o meno */
							close(1);
							open("/dev/null", O_WRONLY);
							/* ridirezionamo anche lo standard error su /dev/null perche' ci interessa solo se il comando diff ha successo o meno */
							close(2);
							open("/dev/null", O_WRONLY);

							execlp("diff", "diff", buffer, linea, (char *)0); /* attenzione prima il nome ricevuto e poi il proprio */

						 	/* si esegue l'istruzione seguente SOLO in caso di fallimento della execlp */
							/* ATTENZIONE SE LA EXEC FALLISCE NON HA SENSO NE' FARE printf("Errore in execlp\n"); ne' perror */
							exit(-1); /* torniamo un -1 che sara' interpretato come 255 e quindi identificato come errore */
			                        }
					 	/* il figlio deve aspettare ogni nipote. Non e' richiesta la stampa la inseriamo lo stesso per controllo: stampiaamo il suo pid con il valore ritornato */
						if ((pid = wait(&status)) < 0)
						{
							printf("Errore in wait\n");
							exit(-1);
						}
						if ((status & 0xFF) != 0)
						{
							printf("Nipote con pid %d terminato in modo anomalo\n", pid);
							exit(-1);
						}
						else
						{
							/* ATTENZIONE: stampa non richiesta */
							printf("DEBUG-Il nipote con pid=%d ha ritornato %d\n", pid, ritorno=(status >> 8) & 0xFF);
							/* omettiamo il controllo se il nipote NON riesce ad eseguire la exec! */
							if (ritorno == 0)
								printf("I file %s e %s sono uguali\n", buffer, linea); 	/* attenzione prima il nomee ricevuto e poi il proprio */
							else	/* la stampa seguente e' solo di DEBUG dato che il testo chiedeva solo di indicare se i file erano uguali! */
								printf("DEBUG-I file %s e %s sono diversi\n", buffer, linea);

			              		}
			              	}
					/* finito di confrontare il nome letto dalla pipe, bisogna tornare all'inizio del file capo per confrontare il prossimo nome con tutti gli altri */
					lseek(fd, 0L, 0);
				}
			}
			/* torniamo il valore richiesto dal testo */
			exit(n);
		}
	}
	
	/* Codice del padre */
	/* Il padre chiude tutti i lati delle pipe dato che non le usa */
	for (i=0; i < Npipe; i++)
	{
		close(piped[i][0]);
		close(piped[i][1]);
	}

	/* Il padre aspetta tutti i figli */
	for (i=0; i < N; i++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
		printf("Errore in wait\n");
		exit(5);
		}

		if ((status & 0xFF) != 0)
    		printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 
			ritorno=(int)((status >> 8) &	0xFF); 
			printf("Il figlio con pid=%d ha ritornato %d (se 255 significa che il figlio e' terminato con un errore)\n", pid, ritorno);
		}
	}
	exit(0);
}
