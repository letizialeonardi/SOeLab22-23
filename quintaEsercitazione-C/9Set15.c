/* versione con N pipe di comunicazione/sincronizzazione fra padre e figli e altre N di comunicazione fra figli e padre; inoltre uso del segnale SIGKILL */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

typedef int pipe_t[2];

int main (int argc, char **argv)
{
   int N; 				/* numero di file */
   int *pid;				/* array di pid per fork: N.B. in questo caso serve un array di pid perche' il padre deve inviare il segnale SIGKILL per terminare forzatamente i figli per i quali il confronto non abbia avuto successo */
   int *confronto;			/* array dinamico di interi per sapere se si deve ancora mandare l'indicazione di leggere al figlio corrispondente */
   pipe_t *pipeFiglioPadre;		/* array di pipe di comunicazione fra figli e padre */
   pipe_t *pipePadreFiglio;		/* array di pipe di comunicazione/sincronizzazione fra padre e figlio. NOTA BENE: questa sincronizzazione potrebbe essere fatta tramite l'invio di segnali da parte del padre ai figli */
   int i,j; 				/* contatori */
   int fd; 				/* file descriptor */
   int pidFiglio, status, ritorno;	/* per valore di ritorno figli */
   char c,ch; 				/* carattere per leggere dal file da parte dei figli e dalle pipe da parte del padre*/
   char token='v'; 			/* carattere che serve per sincronizzare i figli: 'v' vai, 't' termina */

	/* ci vogliono almeno due file per i figli che insieme con AF fa un totale di almeno 3 parametri */
	if (argc < 4)
	{
		printf("Errore numero di parametri\n");
		exit(1);
	}

	N = argc-2; /* calcoliamo il numero di file passati, a parte AF, e quindi il numero di figli da creare */
	printf("DEBUG-Sono il padre %d e il numero di processi da creare sono %d\n", getpid(), N);

	/* allocazione pid */
	if ((pid=(int *)malloc(N*sizeof(int))) == NULL)
	{
        	printf("Errore allocazione pid\n");
        	exit(2);
	}

	/* allocazione confronto */
	if ((confronto=(int *)malloc(N*sizeof(int))) == NULL)
	{
        	printf("Errore allocazione confronto\n");
        	exit(3);
	}

	/* inizializzazione di confronto: tutti i valori a 1 perche' all'inizio si deve mandare l'indicazione al figlio di leggere! */
	for (i=0;i<N;i++)
		confronto[i]=1;

	/* allocazione pipe figli-padre */
	if ((pipeFiglioPadre=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
	{
		printf("Errore allocazione pipe padre\n");
		exit(4);
	}	

	/* allocazione pipe padre-figli */
	if ((pipePadreFiglio=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
	{
		printf("Errore allocazione pipe padre\n");
		exit(5);
	}

	/* creazione pipe */
	for (i=0;i<N;i++)
		if(pipe(pipeFiglioPadre[i])<0)
		{
			printf("Errore creazione pipe\n");
			exit(6);
		}

	/* creazione di altre N pipe di comunicazione/sincronizzazione con il padre: N.B. invece di queste N pipe il padre, si potrebbe usare lo strumento dei segnali */
	for (i=0;i<N;i++)
		if(pipe(pipePadreFiglio[i])<0)
		{
			printf("Errore creazione pipe\n");
			exit(7);
		}

	/* apertura file assegnato al padre (AF) */
	if ((fd=open(argv[argc-1],O_RDONLY)) < 0)
	{
        	printf("Impossibile aprire il file %s\n", argv[argc-1]); 
        	exit(8);
	}

	/* creazione figli con salvataggio dei pid nell'array */
	for (i=0;i<N;i++){
		if ((pid[i]=fork())<0)
		{
			printf("Errore creazione figli\n");
			exit(9);
		}
		if (pid[i]==0)
		{ 	/* codice figlio */
			printf("DEBUG-Sono il figlio %d di indice %d\n", getpid(), i);
			/* chiusura pipe inutilizzate */
			for (j=0;j<N;j++)
			{ 
				/* il figlio non legge da nessuna pipeFiglioPadre e non scrive su nessuna pipePadreFiglio */
				close (pipeFiglioPadre[j][0]);
				close (pipePadreFiglio[j][1]);
				if (j != i) 
 				{	 /* inoltre non scrive e non legge se non su/dalle sue pipe */
					close (pipeFiglioPadre[j][1]);
					close (pipePadreFiglio[j][0]);
	 			}
			}	
			/* apertura file */
			if ((fd=open(argv[i+1],O_RDONLY)) < 0)
			{
				printf("Impossibile aprire il file %s\n", argv[i+1]);
				exit(-1); /* deciso di tornare -1 che verra' interpretato come 255 e quindi INSUCCESSO */
			}
			
			/* ATTENZIONE LO SCHEMA DA UTILIZZARE E' DIVERSO DA QUELLO DEL 15 LUGLIO 2015 DATO CHE IN QUEL CASO SI DEVE USCIRE DAL CICLO APPENA FINISCE IL PROPRIO FILE ASSOCIATO; INVECE IN QUESTO CASO SI DEVE TERMINARE QUANDO LO STABILISCE IL PADRE! */
			/* ciclo di lettura da pipe per ricevere l'indicazione dal padre e quindi lettura dal file */
        		while (read(pipePadreFiglio[i][0], &token, 1))
        		{
				/* se il token ricevuto e' 't' allora il padre ha terminato la lettura del suo file e quindi anche questo processo che ha sempre avuto successo nei confronti carattere per carattere deve terminare il ciclo */
				if (token == 't') break;
				/* non c'e' bisogno dell'else dato che se il token e' 't' si esce dal ciclo; quindi sicuramente il token e' 'v' cioe' il padre ha indicato di leggere dal file */
                		read(fd,&c,1);
                		/* printf("HO LETTO IL TOKEN per il carattere %c\n", c); */
				/* il carattere letto va mandato al padre per il confronto */
                		write(pipeFiglioPadre[i][1],&c,1);
        		}
			printf("DEBUG: Sono il figlio %d di indice %d e ho finito di leggere il file %s\n", getpid(), i, argv[i+1]); 
         		/* il figlio e' riuscito a terminare la letture del proprio file e quindi si deve ritornare al padre il valore 0 per indicare successo */
        		exit(0);
		}
	} /* fine for */

	/* codice del padre */
	/* chiusura pipe */
	for(i=0;i<N;i++)
	{ /* il padre non legge da nessuna pipePadreFiglio e non scrive su nessuna pipeFiglioPadre */
		close (pipePadreFiglio[i][0]);
		close (pipeFiglioPadre[i][1]);
	}

	/* il padre deve leggere i caratteri dal file assegnato e confrontarli con quelli inviati dai figli: si ricorda che i file hanno tutti la stessa lunghezza dato che questo e' garantito dalla parte shell */
	while (read(fd, &ch, 1))
		for(i=0;i<N;i++)
		{
			/* il padre manda l'indicazione di leggere ad ogni figlio per ogni carattere solo se confronto e' ancora 1 */
			if (confronto[i])
			{
				write(pipePadreFiglio[i][1], &token, 1);
				/* il padre riceve il carattere letto dal figlio e lo confronta con il suo */
				read(pipeFiglioPadre[i][0],&c,1);
                		/* printf("SONO IL PADRE: HO LETTO il carattere %c e il figlio %c\n", ch, c);  */
				if (ch != c)
                       		{
					/* i caratteri sono diversi e quindi bisogna resettare il valore corrispondente di confronto  */
					/* printf("SONO IL PADRE: HO LETTO il carattere %c e il figlio %c e quindi mi segno che non devo più mandare indicazione a questo figlio %d\n", ch, c, pid[i]);  */

					confronto[i]=0;
				}	
			}	
		}	
	
	/* una volta che il padre ha terminato la lettura del file AF */
	for(i=0;i<N;i++)
		if (!confronto[i])
		{
			/* terminiamo forzatamente tutti i figli che hanno fallito il confronto e che sono bloccati sulla read dalla pipe con il padre */
        		if (kill(pid[i], SIGKILL) == -1) /* controlliamo che la kill non fallisca a causa della terminazione di uno dei figli, anche se in questo caso non dovremmo avere mai questo caso */
               		printf("Figlio con pid %d non esiste e quindi e' gia' terminato\n", pid[i]);;
    		}	
 		else 
		{
			/* per i figli invece per cui il confronto non e' fallito mandiamo una indicazione di uscire dal ciclo, altrimenti rimarrebbero bloccati sulla read e avremmo un deadlock: dato che il figlio aspetterebbe un token dal padre e il padre aspetterebbe che il figlio finisse (con la wait del codice seguente) */
			token='t';
			write(pipePadreFiglio[i][1], &token, 1);
 		}

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		if ((pidFiglio = wait(&status)) < 0)
        	{
                	printf("Errore in wait\n");
                	exit(10);
        	}

		if ((status & 0xFF) != 0)
              		printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{	ritorno=(int)((status >> 8) & 0xFF);
                	printf("Il figlio con pid=%d ha ritornato il valore %d (se 255 problemi)\n", pidFiglio, ritorno);
                	for (j=0; j < N; j++)
				if (pid[j] == pidFiglio) /* se un figlio termina normalmente vuol dire che non e' stato ucciso dal SIGKILL */
					printf("Questo significa che il figlio di indice %d ha verificato che il file %s e' uguale al file %s\n", j, argv[j+1], argv[argc-1]);
		}
	}

	exit(0);
}
