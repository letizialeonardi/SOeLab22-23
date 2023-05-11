/* file provaEsame2-a.c */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* versione con N pipe */

/* tipo che definisce una pipe */
typedef int pipe_t[2];

int main(int argc, char *argv[])
{
   int pid;		/* pid per fork */
   int N; 		/* numero di caratteri e quindi numero di processi */
   int fdr; 		/* per open */
   int i, k; 		/* indici */
   long int cont;	/* per conteggio */
   char c;		/* per leggere dal file */
   pipe_t *p; 		/* array dinamico di pipe */
   int pidFiglio, status, ritorno;    /* variabili per wait*/
  
  	if (argc < 4)	/* devono essere passati almeno TRE parametri (il nome di un file e poi almeno DUE caratteri) */
  	{
    		printf("Errore nel numero di parametri dato che argc = %d\n", argc);
    		exit(1);
  	}

  	/* numero dei caratteri passati sulla linea di comando */
  	N = argc - 2;	/* ATTENZIONE DOBBIAMO TOGLIERE 2, il nome del programma eseguibile e il nome del file */
  
  	/* controllo singoli caratteri */
  	for (i=0; i < N; i++)
    	if (strlen(argv[i+2]) != 1)	/* ATTENZIONE: caratteri si trovano incrementando l'indice i di 2 */
    	{ 	printf("Errore nella stringa %s che non e' un singolo carattere\n", argv[i+2]); 
      		exit(2);
    	}

  	/* alloco l'array di pipe */
  	p = (pipe_t *)malloc(sizeof(pipe_t) * N);
  	if (p == NULL) 
  	{ 	printf("Errore nella creazione array dinamico\n");
      		exit(3);
  	}
  
  	/* creo N pipe */
  	for (i=0; i < N; i++)
    	if (pipe(p[i]) < 0)
    	{ 	printf("Errore nella creazione della pipe\n"); 
      		exit(4);
    	}
 
  	printf("DEBUG-Sono il processo padre con pid %d e sto per generare %d figli\n", getpid(), N);
              
  	for (i=0; i < N; i++)
  	{
    		/* creazione dei figli */
    		if ((pid = fork()) < 0)
    		{
      			printf ("Errore nella fork\n");
      			exit(5);
    		}
    
    		if (pid == 0) /* figlio */
    		{
      			printf("DEBUG-Figlio %d con pid %d: sono associato al carattere %c\n", i, getpid(), argv[i+2][0]);
  
      			/* chiude tutte le pipe che non usa */
      			for (k = 0; k < N; k++)
      			{	/* ogni figlio NON legge da nessuna pipe e scrive solo sulla sua che e' la i-esima! */
        			close(p[k][0]);
        			if (k != i)
          				close(p[k][1]);
      			}
      
      			/* ogni figlio apre lo stesso file */
      			if ((fdr = open(argv[1], O_RDONLY)) < 0)
      			{ 	printf("Errore nella apertura del file %s\n", argv[1]);
        			exit(-1); 	/* torniamo -1 che verrà interpretato come 255 e quindi come errore */
      			}

      			cont = 0L; 	/* conta le occorrenze del carattere associato e quindi va inizializzato a 0 */
      			while(read(fdr, &c, 1) > 0)	/* si legge il file un carattere alla volta fino alla fine */
      			{
        			if (c == argv[i+2][0])	/* se si e' trovato il carattere associato */
          				cont++;		/* si incrementa il contatore */
      			}
      			/* comunica al padre usando la i-esima pipe */
      			write(p[i][1], &cont, sizeof(cont));
			/* NOTA BENE: essendo cont un long int NON si potrebbe mai pensare di passare questa informazione al padre con una exit */
                        exit(argv[i+2][0]);     /* torniamo il carattere cercato */
    		}
  	}
  
  	/* padre */
  	/* chiude tutte le pipe che non usa */
  	for (k=0; k < N; k++)
  	{
    		close(p[k][1]);
  	}

  	/* legge dalle pipe i messaggi */
  	for (i=0; i < N; i++)
  	{
    		if (read(p[i][0], &cont, sizeof(cont)) > 0)
      			printf("%ld occorrenze del carattere %c nel file %s\n", cont, argv[i+2][0], argv[1]);	/* i long int si stampano con %ld */
  	}
  
	/* Attesa della terminazione dei figli */
  	for(i=0;i < N;i++)
  	{
   		if ((pidFiglio = wait(&status)) < 0)
   		{
      			printf("Errore wait\n");
      			exit(6);
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
