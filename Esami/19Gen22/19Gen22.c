/* soluzione parte C esame del 19 Gennaio 2022: la comunicazione in ogni coppia va dal primo processo della coppia al secondo processo della coppia ed e' il secondo processo della coppia deve creare il file con terminazione ".mescolato" sul quale poi deve scrivere */
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

#define PERM 0644

typedef int pipe_t[2]; 		/* tipo di dato per contenere i file descriptors di una pipe */

int main(int argc, char **argv) 
{
	int N; 			/* numero di file: i processi figli saranno il doppio! */
	/* N nome specificato nel testo */
	int C; 			/* numero intero positivo dispari */
	int pid;		/* variabile per fork */
	pipe_t *pipe_ps;	/* array di pipe per la comunicazione dai figli primi della coppia ai figli secondi della coppia */
	int fd;			/* variabile per open */
	char *FCreato;		/* variabile per nome file da creare da parte dei processi figli primi della coppia */
	int fcreato;		/* variabile per creat */
	/* fcreato nome specificato nel testo */
	char *b;		/* variabile per leggere dai figli */
	/* b nome specificato nel testo */
	int nroTotale;		/* variabile per tenere traccia del numero di blocchi presenti nei file */
	int nro;		/* variabile per tenere traccia del numero di blocchi leti dalla propria meta' del file */
	/* nro nome specificato nel testo */
	int status, pidFiglio, ritorno;	/* per wait */
	int i, j;		/* indici per cicli */
	/* i nome specificato nel testo */
	int nr, nw;		/* per controllo su read/write */

/* Controllo sul numero di parametri: N deve essere maggiore o uguale a 1 */
if (argc < 3) 
{
	printf("Errore numero parametri dati che argc=%d\n", argc);
	exit(1);
}

/* controllo sull'ultimo parametro: numero dispari C */
C = atoi(argv[argc-1]);
if ( (C <= 0) || (C % 2 == 0) )
{
	printf("Errore numero C %s\n", argv[argc-1]);
	exit(2);
}

/* calcoliamo il numero dei file */
N = argc - 2; 

printf("DEBUG-Numero processi da creare %d con C=%d\n", 2*N, C);

/* allocazione memoria dinamica per buffer */
b=(char *)malloc(C*sizeof(char));
if (b == NULL)
{
        printf("Errore nella malloc per buffer b\n");
        exit(3);
}

/* allocazione memoria dinamica per pipe_ps. NOTA BENE: servono un numero di pipe che e' la meta' del numero di figli! */
pipe_ps=(pipe_t *)malloc(N*sizeof(pipe_t));
if (pipe_ps == NULL)
{
        printf("Errore nella malloc per le pipe\n");
        exit(4);
}

/* creazione delle pipe: ATTENZIONE VANNO CREATE solo N pipe */
for (i=0; i < N; i++) 
{
	if (pipe(pipe_ps[i])!=0) 
	{
		printf("Errore creazione delle pipe\n"); 
		exit(5);
	}
}

/* creazione dei processi figli: ne devono essere creati 2*N */
for (i=0; i < 2*N; i++) 
{
	pid=fork();
 	if (pid < 0)  /* errore */
   	{
		printf("Errore nella fork con indice %d\n", i);
      		exit(6);
   	}
	if (pid == 0) 
	{
 		/* codice del figlio: in caso di errore torniamo 0 che non e' un valore accettabile (per quanto risulta dalla specifica della parte shell) */
      		if (i < N) /* siamo nel codice dei figli primi della coppia */
		{
			/* stampa di debugging */
			printf("DEBUG-PRIMO DELLA COPPIA-Figlio di indice %d e pid %d associato al file %s\n",i,getpid(),argv[i+1]);
      			/* chiudiamo le pipe che non servono */
      			/* ogni figlio PRIMO della coppia scrive solo sulla pipe_ps[i] */
      			for (j=0;j<N;j++)
      			{
        			close(pipe_ps[j][0]);
        			if (j!=i) 	
        			{
        				close(pipe_ps[j][1]);
        			}
      			}
			
			/* ogni figlio deve aprire il suo file associato */
			fd=open(argv[i+1], O_RDONLY);
			if (fd < 0) 
			{
				printf("Impossibile aprire il file %s\n", argv[i+1]);
				exit(0); /* in caso di errore, decidiamo di tornare 0 che non e' un valore accettabile */
			}

			/* calcoliamo la lunghezza in blocchi del file */
			nroTotale = lseek(fd, 0L, 2) / C;
			/* bisogna riportare l'I/0 pointer all'inizio del file */
			nro=nroTotale/2;	/* ogni figlio legge meta' del file */
			lseek(fd, 0L, 0);
	 		for (j=0;j<nro;j++)
			{
				read(fd, b, C); 
				/* ogni blocco letto dal PRIMO processo della coppia deve essere inviato al processo SECONDO della coppia */
				nw=write(pipe_ps[i][1], b, C);
				if (nw != C) 
				{
					printf("Errore in scrittura su pipe %d\n", i);
					exit(0); 
				}
			}
		}
		else /* siamo nel codice dei figli secondi della coppia */
		{
			/* stampa di debugging */
			printf("DEBUG-SECONDO DELLA COPPIA-Figlio di indice %d e pid %d associato al file %s\n",i,getpid(),argv[i-N+1]);
			/* i figli secondi della coppia devono creare il file specificato */
			FCreato=(char *)malloc(strlen(argv[i-N+1]) + 11); /* bisogna allocare una stringa lunga come il nome del file associato + il carattere '.' + i caratteri della parola mescolato (9) + il terminatore di stringa: ATTENZIONE ALL'INDICE PER INDIVIDUARE IL FILE */
			if (FCreato == NULL) 
			{
				printf("Errore nelle malloc\n");
				exit(0);
			}
			/* copiamo il nome del file associato */
			strcpy(FCreato, argv[i-N+1]);
			/* concateniamo la stringa specificata dal testo */
			strcat(FCreato,".mescolato");
			fcreato=creat(FCreato, PERM);
 			if (fcreato < 0)
                	{	
                        	printf("Impossibile creare il file %s\n", FCreato);
                        	exit(0); 
                	}

                        /* chiudiamo le pipe che non servono */
                        /* ogni figlio SECONDO della coppia legge solo da pipe_ps[i-N] */
                        for (j=0;j<N;j++)
                        {
                                close(pipe_ps[j][1]);
                                if (j!= i-N)	 /* ATTENZIONE ALL'INDICE CHE DEVE ESSERE USATO */
                                {
                                        close(pipe_ps[j][0]);
                                }
                        }

			/* ogni figlio deve aprire il suo file associato: siamo nei figli secondi della coppia e quindi attenzione all'indice */
			fd=open(argv[i-N+1], O_RDONLY);
			if (fd < 0) 
			{
				printf("Impossibile aprire il file %s\n", argv[i-N+1]);
				exit(0); 
			}

			/* calcoliamo la lunghezza in blocchi del file */
			nroTotale = lseek(fd, 0L, 2) / C;
			nro=nroTotale/2;	/* ogni figlio legge meta' del file */
			/* bisogna posizionare l'I/0 pointer a meta' del file */
			lseek(fd, (long)nro * C, 0);
	 		for (j=0;j<nro;j++)
			{
				read(fd, b, C); 
				/* ogni blocco letto dal processo SECONDO della coppia, bisogna scriverlo sul file */
				write(fcreato, b, C);
				/* dobbiamo a questo punto aspettare il blocco dal processo PRIMO della coppia: attenzione all'indice */
				nr=read(pipe_ps[i-N][0], b, C);
				if (nr != C) 
				{
					printf("Errore in lettura da pipe %d\n", i-N);
					exit(0); 
				}
				/* ogni blocco ricevuto dal processo PRIMO della coppia, bisogna scriverlo sul file */
				write(fcreato, b, C);
        		}
		}	
		exit(nro); /* torniamo il numero di blocchi letti (supposto <= di 255) */
	}
}

/*codice del padre*/
/* chiudiamo tutte le pipe, dato che le usano solo i figli */
for (i=0;i<N;i++)
{
   close(pipe_ps[i][0]);
   close(pipe_ps[i][1]);
}

/* Attesa della terminazione dei figli */
for(i=0;i<2*N;i++)
{
   pidFiglio = wait(&status);
   if (pidFiglio < 0)
   {
      printf("Errore wait\n");
      exit(7);
   }
   if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
   else
   {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d (se 0 problemi)\n", pidFiglio, ritorno);
   }
}
exit(0);
}/* fine del main */
