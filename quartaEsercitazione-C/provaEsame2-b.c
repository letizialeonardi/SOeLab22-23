#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* versione con 1 pipe e uso di una struttura */

int main(int argc, char *argv[])
{
   int pid;    		/* pid per fork */
   int N;    		/* numero di caratteri e quindi numero di processi */
   int fdr;     	/* per open */
   int i;     		/* indice */
   char c;      	/* per leggere dal file */
   int p[2];    	/* singola pipe */
   int pidFiglio, status, ritorno;    /* variabili per wait*/
   /* struttura per la comunicazione tra figli e padre */
   struct {
    		char c; 	/* carattere controllato */
    		long int n; 	/* numero di occorrenze del carattere */
          } msg;
  
  	if (argc < 4)   /* devono essere passati almeno TRE parametri (il nome di un file e almeno DUE carattere */
        {       
		printf("Errore nel numero di parametri dato che argc = %d\n", argc);
		exit(1);                                                         
  	}

  	/* numero dei caratteri passati sulla linea di comando */
  	N = argc - 2;	 /* ATTENZIONE DOBBIAMO TOGLIERE 2, il nome del programma eseguibile e il nome del file */

  	/* controllo singoli caratteri */
  	for (i=0; i < N; i++)
    		if (strlen(argv[i+2]) != 1)	/* ATTENZIONE: caratteri si trovano incrementando l'indice i di 2 */
    		{ 	printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[i+2]);
      			exit(2);
    		}

  	/* creo la singola pipe */
  	if (pipe(p) < 0)
  	{ 	printf("Errore nella creazione della pipe\n"); 
    		exit(3);
  	}
  
  	printf("DEBUG-Sono il processo padre con pid %d e sto per generare %d figli\n", getpid(), N);
              
  	for (i=0; i < N; i++)
  	{
    		/* creazione dei figli */
    		if ((pid = fork()) < 0)
    		{
      			printf ("Errore nella fork\n");
      			exit(4);
    		}
    
    		if (pid == 0) /* figlio */
    		{
     		 	printf("DEBUG-Figlio %d con pid %d: sono associato al carattere %c\n", i, getpid(), argv[i+2][0]);
  
      			/* chiude il lato della pipe che non usa */
      			close(p[0]);
      
      			/* ogni figlio apre lo stesso file */
      			if ((fdr = open(argv[1], O_RDONLY)) < 0)
      			{ 	printf("Errore nella apertura del file %s\n", argv[1]);
        			exit(-1);	/* torniamo -1 che verrà interpretato come 255 e quindi come errore */
      			}
      			/* inizializza la struttura */
      			msg.c = argv[i+2][0];	/* carattere associato */
      			msg.n = 0L;		/* contatore delle occorrenze */
      			/* conta le occorrenze del carattere associato */
      			while(read(fdr, &c, 1) > 0)	/* si legge il file un carattere alla volta fino alla fine */
      			{
        			if (c == argv[i+2][0]) 	/* se si e' trovato il carattere associato */
          				msg.n++;	 /* si incrementa il contatore */
      			}	
      			/* comunica al padre */
      			write(p[1], &msg, sizeof(msg));
      			exit(argv[i+2][0]);	/* torniamo il carattere cercato */
    		}
  	}
  
  	/* padre */
  	/* chiude il lato della pipe che non usa */
  	close(p[1]);
  
	/* legge dalla pipe i messaggi */
  	while (read(p[0], &msg, sizeof(msg)) > 0)	/* in questo caso si e' usato un while fino a che ci sono informazioni da leggere sulla unica pipe */
    		printf("%ld occorrenze del carattere %c nel file %s\n", msg.n, msg.c, argv[1]);	/* i long int si stampano con %ld */

  	/* Attesa della terminazione dei figli */
  	for(i=0;i < N;i++)
  	{
   		if ((pidFiglio = wait(&status)) < 0)
   		{
      			printf("Errore wait\n");
      			exit(5);
   		}
   		if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
   		else
   		{
                	ritorno=(int)((status >> 8) & 0xFF);
                	printf("Il figlio con pid=%d ha ritornato %c (%d se 255 problemi!)\n", pidFiglio, ritorno, ritorno); /* il padre stampa il valore ritornato come carattere; aggiunta la stampa come intero solo per intercettare un eventuale errore nel figlio */
   		}	
  	}
  	exit(0);
}
