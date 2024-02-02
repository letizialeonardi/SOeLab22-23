/* Soluzione della Prova d'esame del 12 Settembre 2018 - Parte C */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>

typedef int pipe_t[2];

int main(int argc, char **argv) 
{
   /* -------- Variabili locali ---------- */
   int pid;			/* process identifier per le fork() */
   int N; 			/* numero di file passati sulla riga di comando */
   int status;			/* variabile di stato per la wait */
   pipe_t *pipedFP;		/* array dinamico di pipe descriptors per comunicazioni figli-padre  */
   pipe_t *pipedNP;		/* array dinamico di pipe descriptors per comunicazioni nipoti-padre  */
   int fd;			/* file descriptor che serva sia ai figli che ai nipoti */
   int i, j;			/* indici per i cicli */
   char ch; 			/* variabile che serve per leggere i caratteri sia da parte dei figli che dei nipoti */
   long int trasformazioni=0;	/* variabile che mantiene il numero delle trasformazioni effettuate sia dai figli che dai nipoti */
   int ritorno; 		/* variabile che viene ritornata da ogni figlio al padre e da ogni nipote al figlio */
   /* ------------------------------------ */
	
	/* Controllo sul numero di parametri (ci devono essere almeno di due parametri */
	if (argc < 3) /* Meno di due parametri */  
	{
		printf("Errore nel numero dei parametri, dato che argc=%d\n", argc);
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;
	
	/* Allocazione dell'array di N pipe descriptors figli-padre */
	pipedFP = (pipe_t *) malloc(N*sizeof(pipe_t));
	if (pipedFP == NULL)
	{
		printf("Errore nella allocazione della memoria pipe figli-padre\n");
		exit(2);
	}
	
	/* Allocazione dell'array di N pipe descriptors nipoti-padre */
	/* N.B. In questo caso i nipoti devono comunicare direttamente con il padre e quindi e' il padre che deve creare una pipe per ogni nipote e non il figlio come invece si puo' fare nei testi in cui la comunicazione deve essere nipote-figlio */
	pipedNP = (pipe_t *) malloc (N*sizeof(pipe_t));
	if (pipedNP == NULL)
	{
		printf("Errore nella allocazione della memoria pipe nipoti-padre\n");
		exit(3);
	}
	
	/* Creazione delle N pipe figli-padre e delle N pipe nipoti-padre */
	for (i=0; i < N; i++)
	{
		if (pipe(pipedFP[i]) < 0)
		{
			printf("Errore nella creazione della pipe %d-esima figli-padre\n", i);
			exit(4);
		}
		if (pipe(pipedNP[i]) < 0)
		{
			printf("Errore nella creazione della pipe %d-esima nipoti-padre\n", i);
			exit(5);
		}
	}

	/* Ciclo di generazione dei figli */
	for (i=0; i < N; i++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork %d-esima\n", i);
			exit(6);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("DEBUG-Sono il processo figlio di indice %d e pid %d sto per creare il nipote che leggera' sempre dal mio stesso file %s\n", i, getpid(), argv[i+1]);
			/* Chiusura delle pipe non usate nella comunicazione con il padre  */
			for (j=0; j < N; j++)
			{
				close(pipedFP[j][0]);
				if (i != j) close(pipedFP[j][1]);
			}

			/* N.B. Le chiusure per le altre pipe vanno fatte assolutamente dopo la creazione del nipote! */
			if ( (pid = fork()) < 0)
			{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-1); /* decidiamo, in caso di errore, di tornare -1 che verra' interpretato come 255 e quindi un valore NON accettabile */
			}	
			if (pid == 0) 
			{
				/* codice del nipote */
				printf("DEBUG-Sono il processo nipote del figlio di indice %d e pid %d\n", i, getpid());
				/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(pipedFP[i][1]);
				/* Chiusura delle pipe non usate nella comunicazione con il padre  */
				for (j=0; j < N; j++)
				{
					close(pipedNP[j][0]);
					if (i != j) close(pipedNP[j][1]);
				}

				/* sia il figlio che il nipote devono aprire (con due open separate per avere l'I/O pointer separato) lo stesso file in lettura/scrittura, dato che dovranno operare delle trasformazioni */
				if ((fd=open(argv[i+1], O_RDWR)) < 0)
				{
                                	printf("Errore nella open del file %s da parte del nipote\n", argv[i+1]);
					exit(-1); /* decidiamo, in caso di errore, di tornare -1 che verra' interpretato come 255 e quindi un valore NON accettabile */
                        	}
		        	while (read(fd, &ch, 1))
				{	
					/* controlliamo se abbiamo trovato un carattere alfabetico minuscolo */
					if (islower(ch))  
					{ 	
						/* questo carattere deve essere trasformato nel corrispondente carattere alfabetico MAIUSCOLO e quindi per prima cosa bisogna tornare indietro di una posizione */
						lseek(fd, -1L, SEEK_CUR);
					  	ch = ch - 32; /* trasformiamo il carattere da minuscolo in MAIUSCOLO togliendo 32 */
					  	write(fd, &ch, 1); /* scriviamolo sul file */
						/* e aggiorniamo il numero di trasformazioni */
						trasformazioni++;
					}
					else
						; /* non si deve fare nulla */
				}
				/* il nipote deve inviare al padre il numero di trasformazioni operate */
				write(pipedNP[i][1], &trasformazioni, sizeof(trasformazioni));
				/* torniamo il valore richiesto dal testo operando una divisione intera per 256 */
				ritorno=trasformazioni/256;	
				exit(ritorno);
			}
			/* codice figlio */
			/* le pipe usate dal nipote vanno chiuse TUTTE */
			/* ATTENZIONE: SOLO DOPO AVERE CREATO IL NIPOTE! */
			for (j=0; j < N; j++)
			{
				close(pipedNP[j][0]);
				close(pipedNP[j][1]);
			}

			/* il figlio ha un codice molto simile al nipote */
			/* sia il figlio che il nipote devono aprire (con due open separate per avere l'I/O pointer separato) lo stesso file in lettura/scrittura, dato che dovranno operare delle trasformazioni */
			if ((fd=open(argv[i+1], O_RDWR)) < 0)
			{
                               	printf("Errore nella open del file %s da parte del figlio\n", argv[i+1]);
				exit(-1); /* decidiamo, in caso di errore, di tornare -1 che verra' interpretato come 255 e quindi un valore NON accettabile */
                        }
		       	while (read(fd, &ch, 1))
			{	
				/* controlliamo se abbiamo trovato un carattere numerico */
				if (isdigit(ch))  
				{ 	
					/* questo carattere deve essere trasformato nel carattere spazio e quindi per prima cosa bisogna tornare indietro di una posizione */
					lseek(fd, -1L, SEEK_CUR);
				  	ch = ' '; /* trasformiamo il carattere nello spazio */
				  	write(fd, &ch, 1); /* scriviamolo sul file */
					/* e aggiorniamo il numero di trasformazioni */
					trasformazioni++;
				}
				else
					; /* non si deve fare nulla */
			}
			/* il figlio deve inviare al padre il numero di trasformazioni operate */
			write(pipedFP[i][1], &trasformazioni, sizeof(trasformazioni));
			/* il figlio deve aspettare il nipote e stampare il suo pid con il valore ritornato (come richiesto dal testo) */
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
				printf("Il nipote con pid=%d ha ritornato %d\n", pid, ritorno=(int)((status >> 8) & 0xFF));

			/* torniamo il valore richiesto dal testo operando una divisione intera per 256 */
			ritorno=trasformazioni/256;	
			exit(ritorno); 
		}
	}
	
	/* Codice del padre */
	/* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
	{
		close(pipedFP[i][1]);
		close(pipedNP[i][1]);
 	}

	/* Il padre recupera le informazioni dai figli in ordine di indice */
	for (i=0; i < N; i++)
	{
		read(pipedFP[i][0], &trasformazioni, sizeof(trasformazioni));
		printf("Il figlio di indice %d ha operato %ld trasformazioni di caratteri numerici in carattere spazio sul file %s\n", i, trasformazioni, argv[i+1]);
		read(pipedNP[i][0], &trasformazioni, sizeof(trasformazioni));
		printf("Il nipote di indice %d ha operato %ld trasformazioni di caratteri minuscoli in MAIUSCOLI sullo stesso file %s\n", i, trasformazioni, argv[i+1]);
	}	

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		if ((pid = wait(&status)) < 0)
		{
			printf("Errore in wait\n");
			exit(7);
		}

		if ((status & 0xFF) != 0)
    			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
    		else
		{ 
			ritorno=(int)((status >> 8) &	0xFF); 
		  	if (ritorno==255)
 				printf("Il figlio con pid=%d ha ritornato %d e quindi vuole dire che ci sono stati dei problemi\n", pid, ritorno);
		  	else  	printf("Il figlio con pid=%d ha ritornato %d\n", pid, ritorno);
		}
	}

	exit(0);
}
