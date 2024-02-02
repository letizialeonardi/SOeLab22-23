/* Soluzione della Prova d'esame del 9 Giugno 2014 - Parte C */
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
   int pid;				/* process identifier per le fork() */
   int N; 				/* numero di file passati sulla riga di comando (uguale al numero di file) */
   int status;				/* variabile di stato per la wait */
   pipe_t *piped;			/* array dinamico di pipe descriptors per comunicazioni figli-padre  */
   pipe_t p;				/* una sola pipe per ogni coppia figlio-nipote */ 
   int i, j;				/* indici per i cicli */
   char numero[11];			/* array di caratteri per memorizzare la stringa corrispondente al numero di righe: ci vogliono 10 char per rappresentare un intero di 16 bit e poi ci sara' il carattere di "a capo" */
   int valore; 				/* variabile che viene comunicata da ogni figlio al padre e che contiene la conversione della stringa numero */
   int ritorno; 			/* variabile che viene ritornata da ogni figlio al padre e che contiene il ritorno del nipote */
   long int somma = 0; 			/* variabile usata dal padre per calcolare la somma di tutte le lunghezze comunicate dai figli */
   /* ------------------------------------ */
	
	/* Controllo sul numero di parametri */
	if (argc < 3) /* Meno di due parametri */  
	{
		printf("Errore nel numero dei parametri, dato che argc=%d\n", argc);
		exit(1);
	}

	/* Calcoliamo il numero di file passati */
	N = argc - 1;
	
	/* Allocazione dell'array di N pipe descriptors*/
	piped = (pipe_t *) malloc (N*sizeof(pipe_t));
	if (piped == NULL)
	{
		printf("Errore nella allocazione della memoria\n");
		exit(2);
	}
	
	/* Creazione delle N pipe figli-padre */
	for (i=0; i < N; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella creazione della pipe\n");
			exit(3);
		}
	}

	printf("DEBUG-Sono il processo padre con pid %d e sto per generare %d figli\n", getpid(), N);
		
	/* Ciclo di generazione dei figli */
	for (i=0; i < N; i++)
	{
		if ( (pid = fork()) < 0)
		{
			printf("Errore nella fork\n");
			exit(4);
		}
		
		if (pid == 0) 
		{
			/* codice del figlio */
			printf("DEBUG-Sono il processo figlio di indice %d e pid %d sto per creare il nipote che calcolera' il numero di linee del file %s\n", i, getpid(), argv[i+1]);
			/* Chiusura delle pipe non usate nella comunicazione con il padre  */
			for (j=0; j < N; j++)
			{
				close(piped[j][0]);
				if (i != j) close(piped[j][1]);
			}

			/* prima creiamo la pipe di comunicazione fra nipote e figlio */
		  	if(pipe(p) < 0)
                	{	
                        	printf("Errore nella creazione della pipe\n");
                                exit(-1); /* decidiamo di tornare -1 che verra' interpretato dal padre come 255 e quindi un valore non ammissibile! */
                	}

			if ( (pid = fork()) < 0)
			{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-1);
			}	
			if (pid == 0) 
			{
				/* codice del nipote */
				printf("DEBUG-Sono il processo nipote del figlio di indice %d e pid %d sto per calcolare il numero di linee del file %s\n", i, getpid(), argv[i+1]);
				/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(piped[i][1]);
				/* Ridirezione dello standard input:  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1])i; NOTA BENE: IN QUESTO CASO LA RIDIREZIONE E' OBBLIGATORIA PER AVERE SULLO STANDARD OUTPUT SOLO LA STRINGA CORRISPONDENTE AL NUMERO! */
				close(0);
				if (open(argv[i+1], O_RDONLY) < 0)
				{
                                	printf("Errore nella open del file %s\n", argv[i+1]);
                                	exit(-1); /* decidiamo di tornare -1 che verra' interpretato dal padre come 255 e quindi un valore non ammissibile! */
                        	}
				/* ogni nipote deve simulare il piping dei comandi nei confronti del figlio/padre e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
				close(1);
				dup(p[1]); 			
				/* ogni nipote adesso puo' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard ouput */
				close(p[0]);
				close(p[1]);
				/* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video): facoltativo! */
				close(2);
				open("/dev/null", O_WRONLY);
				
				/* Il nipote diventa il comando wc -l */		
				execlp("wc", "wc", "-l", (char *)0);
				/* attenzione ai parametri nella esecuzione di wc: solo -l e terminatore della lista */ 
				
				/* Non si dovrebbe mai tornare qui!!*/
				exit(-1); /* torno un valore diverso da zero per indicare insuccesso (vedi altri commenti nel seguito) */
			}
			/* codice figlio */
			/* ogni figlio deve chiudere il lato che non usa della pipe di comunicazione con il nipote */
			close(p[1]);
			/* adesso il figlio legge dalla pipe un carattere alla volta */
			j=0;
		        while (read(p[0], &(numero[j]), 1))
			{
/*
				printf("indice j= %d carattere letto da pipe %c\n", j, numero[j]);
*/
				j++;
			}
			/* all'uscita di questo ciclo while, nell'array numero ci saranno tutti i caratteri numerici corrispondenti al numero di linee del file e come ultimo carattere il terminatore di linea */
			/* converto l'array di char in stringa sostituendo allo '\n' il terminatore di stringa, controllando pero' di avere letto in effetti qualcosa */
			if (j!=0) /* se il figlio ha letto qualcosa */
			{
				numero[j-1]='\0';
				/* converto l'array di char in numero che bisogna comunicare al padre */
				valore=atoi(numero);
				/* printf("valore convertito dal figlio %d\n", valore); */
			}
			else 
			{      	/* questo e' il caso che il nipote sia incorso in un errore e che quindi non abbia eseguito il wc -l */
                               	valore=0;	/* se il figlio non ha letto nulla, inviamo 0 */
                               /* printf("il nipote non ha passato alcuna stringa da convertire\n"); */
                      	}

			/* il figlio comunica al padre */
			write(piped[i][1], &valore, sizeof(valore));

			/* il figlio deve aspettare il nipote per restituire il valore al padre */
			/* se il nipote è terminato in modo anomalo decidiamo di tornare -1 che verra' interpretato come 255 e quindi segnalando questo problema al padre */
			ritorno=-1;
			if ((pid = wait(&status)) < 0)
				printf("Errore in wait\n");
			if ((status & 0xFF) != 0)
    				printf("Nipote con pid %d terminato in modo anomalo\n", pid);
    			else
				/* stampa non richiesta che volendo si puo' commentare: SOLO DEBUGGING */
				printf("DEBUG-Il nipote con pid=%d ha ritornato %d\n", pid, ritorno=(int)((status >> 8) & 0xFF));
			exit(ritorno);
		}
	}
	
	/* Codice del padre */
	/* Il padre chiude i lati delle pipe che non usa */
	for (i=0; i < N; i++)
		close(piped[i][1]);

	/* Il padre recupera le informazioni dai figli in ordine di indice */
	for (i=0; i < N; i++)
	{
		/* il padre recupera tutti i valori interi dai figli */
		read(piped[i][0], &valore, sizeof(valore));
		/* stampa non richiesta, ma che male non fa */
		printf("DEBUG-Il figlio di indice %d ha convertito il valore %d per il file %s\n", i, valore, argv[i+1]);
		somma = somma + (long int)valore;
	}	
	printf("La somma risultante derivante dai valori comunicati dai figli e' %ld\n", somma); 

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
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
		  	if (ritorno==255)
 				printf("Il figlio con pid=%d ha ritornato %d e quindi vuole dire che il figlio ha avuto dei problemi oppure il nipote non è riuscito ad eseguire il wc oppure è terminato in modo anormale\n", pid, ritorno);
		  	else  	printf("Il figlio con pid=%d ha ritornato %d\n", pid, ritorno);
		}
	}

	exit(0);
}
