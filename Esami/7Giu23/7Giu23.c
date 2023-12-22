/* Soluzione parte C esame del 7 Giugno 2023: versione che calcola il massimo */
/* SI E' DECISO DI USARE LE PIPE PER INVIARE L'INDICAZIONE DA PADRE A FIGLI SE STAMPARE O MENO */
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

typedef int pipe_t[2]; 		/* tipo di dato per contenere i file descriptors di una pipe */

/* VARIABILI GLOBALI */
int *finito; 	/* array dinamico per indicare i figli che sono terminati */
		/* la semantica di questo array e' che ogni elemento vale 0 se il corrispondente processo NON e' finito, altrimenti vale 1 */
int N; 		/* numero di processi figli: deve essere una variabile globale perche' deve essere usata dalla funzione finitof */
/* nome indicato nel testo */

int finitof()
{
/* questa funzione verifica i valori memorizzati nell'array finito: appena trova un elemento uguale a 0 vuole dire che non tutti i processi figli sono finiti e quindi torna 0; tornera' 1 se e solo se tutti gli elementi dell'array sono a 1 e quindi tutti i processi sono finiti */
	int i;
	for (i=0; i < N; i++)
		if (!finito[i])
			/* appena ne trova uno che non ha finito */
			return 0; /* ritorna falso */
	return 1;
}

int main(int argc, char **argv) 
{
	int pid;		/* variabile per fork */
	pipe_t *pipe_fp;	/* array di pipe per la comunicazione figli-padre*/
	pipe_t *pipe_pf;	/* array di pipe per l'indicazione padre-figli: si veda commento all'inizio del file! */
	int fd;			/* variabile per open */	
	char linea[250];	/* variabile per leggere le linee dai file (si suppone bastino 250 caratteri) */
        /* nome indicato nel testo */
	char car;		/* variabile usata dal padre per leggere gli ultimi caratteri delle linee inviati dai figli */
        /* nome indicato nel testo */
	char chMax;		/* variabile usata dal padre per tenere traccia del carattere di codice ASCII massimo */
	char chControllo;	/* variabile per inviare indicazione ai figli */
	int indice; 		/* variabile usata dal padre per tenere traccia dell'indice del figlio che ha calcolato il massimo */
	int stampe;		/* numero di stampe fatte dai figli che va ritornato al padre */
	int status, pidFiglio, ritorno;	/* per wait */
	int n, j;		/* indici per cicli */
        /* n nome indicato nel testo */
	int nr, nw;		/* per controlli read e write su/da pipe */

/* Controllo sul numero di parametri */
if (argc < 3) 
{
	printf("Errore numero parametri %d\n", argc);
	exit(1);
}

N = argc - 1; 

/* allocazione memoria dinamica per finito */
finito = (int *) malloc(sizeof(int) * N);
if(finito == NULL)
{
	printf("Errore nella allocazione della memoria per array finito\n");
	exit(2);
}
	
/* ... e inizializzazione a 0: all'inizio nessun figlio e' finito */
memset(finito, 0, N*sizeof(int));

/* allocazione memoria dinamica per pipe_fp e pipe_pf */
pipe_fp=malloc(N*sizeof(pipe_t));
pipe_pf=malloc(N*sizeof(pipe_t));
if ((pipe_fp == NULL) || (pipe_pf == NULL))
{
	printf("Errore nelle malloc\n");
	exit(3);
}

/* creazione delle pipe */
for (n=0; n < N; n++) 
{
	if (pipe(pipe_fp[n])!=0) 
	{
		printf("Errore creazione delle pipe figli-padre\n");
		exit(4);
	}
	if (pipe(pipe_pf[n])!=0) 
	{
		printf("Errore creazione delle pipe padre-figli\n");
		exit(5);
	}
}

/* creazione dei processi figli */
for (n=0; n < N; n++) 
{
	pid=fork();
 	if (pid < 0)  /* errore */
   	{
		printf("Errore nella fork con indice %d\n", n);
      		exit(6);
   	}
	if (pid == 0) 
	{
 		/* codice del figlio */
		/* stampa di debugging */
      		printf("DEBUG-Figlio di indice %d e pid %d associato al file %s\n",n,getpid(), argv[n+1]);
      		/* chiudiamo le pipe che non servono */
      		/* ogni figlio scrive solo su pipe_fp[n] e legge solo da pipe_pf[n] */
      		for (j=0;j<N;j++)
      		{
        		close(pipe_fp[j][0]);
        		close(pipe_pf[j][1]);
        		if (j!=n)
        		{
        			close(pipe_fp[j][1]);
        			close(pipe_pf[j][0]);
        		}
      		}

		fd=open(argv[n+1], O_RDONLY);
		if (fd < 0) 
		{
			printf("Impossibile aprire il file %s\n", argv[n+1]);
			exit(-1); /* in caso di errore torniamo -1 che verra' interpretato dal padre come 255 che non e' un valore accettabile */
		}

		/* inizializziamo stampe */
		stampe=0;
		j=0; /* inizializzazione indice per la linea */
	 	while (read(fd, &linea[j], 1)) 
		{
			if (linea[j] == '\n') /* se siamo a fine linea */
			{
				/* inviamo l'ultimo carattere (escluso il terminatore di linea) al padre, avendo ipotizzato che ogni file contenga SEMPRE per ogni linea almeno un carattere oltre il terminatore di linea*/
				nw=write(pipe_fp[n][1], &linea[j-1], 1);
				if (nw != 1)
    				{		
                        		printf("Impossibile scrivere sulla pipe per il processo di indice %d\n", n);
                        		exit(-1); 
               	 		}
				/* aspettiamo dal padre se dobbiamo scrivere o meno */
				nr=read(pipe_pf[n][0], &chControllo, 1);
				if (nr != 1)
    				{		
                        		printf("Impossibile leggere dalla pipe per il processo di indice %d\n", n);
                        		exit(-1); 
         		 	}		
				/* printf("DEBUG-FIGLIO indice %d ho ricevuto dal padre %c\n", n, chControllo); */
				if (chControllo =='S') 
				{
					/* autorizzato, scriviamo */
					/* ma prima convertiamo la linea in stringa: DECISO DI MANTENERE IL TERMINATORE DI LINEA QUINDI USIAMO L'INDICE j+1! */
					linea[j+1]='\0';
					printf("Sono il figlio di indice %d e pid %d e ho trovato una linea con un ultimo carattere '%c' con codice ASCII maggiore degli altri nel file %s. Ecco la linea:\n%s", n, getpid(), linea[j-1], argv[n+1], linea);
					/* dato che linea ha gia' lo '\n' NON lo inseriamo nella printf */
					stampe++;	/* incrementiamo il numero di stampe fatte */
				} 
				else 
				{  /* nulla, si continua a leggere */
					;
				}
				j = 0; /* riportiamo a 0 l'indice della linea */
			}
			else
				j++; 	/* incrementiamo il valore di j per il prossimo carattere */
		}	
		exit(stampe); /* torniamo il numero di stampe fatte (supposto dal testo < di 255) */
	}
}

/*codice del padre*/
/* chiudiamo le pipe che non usiamo */
for (n=0; n < N; n++)
{
   close(pipe_fp[n][1]);
   close(pipe_pf[n][0]);
}

/* continuiamo a ciclare su tutti i figli in ordine, finche' tutti non hanno finito */
/* printf("DEBUG-PADRE valore della funzione finitof=%d\n", finitof()); */
while(!finitof())
{
	chMax=-1; /* ATTENZIONE: il valore del massimo, va resettato per ogni insieme di valori letti dal padre */
	for (n=0; n < N; n++) 
	{
		/* tentiamo di leggere il carattere dal figlio i-esimo: contestualmente viene aggiornato il valore del corrispondente elemento dell'array finito */
		finito[n] = (read(pipe_fp[n][0], &car, 1) != 1);  /* nel caso la read torni un valore diverso da 1, il corrispondente valore di finito viene settato a 1 dato che la condizione e' vera! */
		/* printf("DEBUG-PADRE per indice %d ha calcolato finito = %d\n", i, finito[i]); */

		if (!finito[n]) 
		{
			if (car > chMax)
			{
				/* dobbiamo aggiornare il massimo */
				chMax = car;
				/* debbiamo tenere conto dell'indice del processo che ha inviato il massimo */
				indice = n;
			}
		}
		/* else  printf("DEBUG-PADRE il figlio di indice %d non ha inviato nulla\n", i); */
	}
	/* bisogna mandare ai figli l'indicazione giusta (S per scrivere, N per NON scrivere) */
	/* OSSERVAZIONE IMPORTANTE: IN QUESTA SOLUZIONE NON BASTA LA VARIABILE FINITO DATO CHE LA FASE DI SCRITTURA DEL PADRE AI FIGLI VIENE FATTA NEL CICLO FOR CHE SEGUE E QUINDI IL PADRE DEVE AVERE MODO DI FARE LA SCRITTURA SE E SOLO SE IL FIGLIO GLI HA INVIATO QUALCOSA E QUINDI NON E' FINITO, ALTRIMENTI (IN CASO DI ASSENZA DI QUESTO CONTROLLO) SE UN FIGLIO E' FINITO E IL PADRE GLI MANDA COMUNQUE IL CARATTERE DI CONTROLLO, IL PADRE RICEVEREBBE UN SEGNALE SIGPIPE E MORIREBBE (A MENO CHIARAMENTE DI NON GESTIRE QUESTO SEGNALE CON UNA FUNZIONE HANDLER SPECIFICA)! */ 
	for (n=0; n < N; n++) 
	{
		if (n == indice) /* figlio che deve scrivere */
			chControllo = 'S';
		else chControllo = 'N';
		/* printf("DEBUG-PADRE per indice %d sta per mandare %c\n", i, chControllo); */
		if (!finito[n])
		{
			nw=write(pipe_pf[n][1], &chControllo, 1); 
			if (nw != 1)
				printf("PADRE non e' riuscito ad inviare nulla al figlio di indice %d \n", n);
		
		}
	}
/* printf("DEBUG-PADRE valore della funzione finitof=%d\n", finitof()); */
}

/* Attesa della terminazione dei figli */
for(n=0; n < N; n++)
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
                printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
   }
}
exit(0);
}/* fine del main */
