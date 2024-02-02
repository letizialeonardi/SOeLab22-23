/* Soluzione della Seconda Prova in Itinere del 31 Maggio 2019 (solo ParteC) */
/* versione 1 del testo turni 1 e 2 */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef int pipe_t[2];
typedef struct {
        int pid_nipote;         /* campo c1 del testo */
        int lung_linea;         /* campo c2 del testo */
        char linea_letta[250];  /* campo c3 del testo */
	} Strut;

int main(int argc, char **argv)
{
        /* -------- Variabili locali ---------- */
        int pid;                        /* process identifier per le fork() */
        int N;                          /* numero di file passati sulla riga di comando (uguale al numero di numeri) */
        int status;                     /* variabile di stato per la wait */
        pipe_t *piped;                  /* array dinamico di pipe descriptors per comunicazioni figli-padre  */
        pipe_t p;                       /* una sola pipe per ogni coppia figlio-nipote */
        int i, j;                       /* indici per i cicli */
	int nr;                      	/* variabile per valore di ritorno della read */
        char opzione[3];                /* variabile stringa che serve per passare al comando sort l'opzione corretta (-f in questo caso) */
                                        /* N.B. Bastano 3 caratteri (il simbolo di opzione '-', r/f e il terminatore di stringa) */
        int ritorno;                    /* variabile che viene ritornata da ogni figlio al padre */
        Strut S;                        /* struttura usata dai figli e dal padre */

        /* ------------------------------------ */

        /* Controllo sul numero di parametri */
        if (argc < 4 ) /* Meno di tre parametri */
        {
                printf("Errore nel numero dei parametri\n");
                exit(1);
        }

        /* Calcoliamo il numero di file passati */
        N = argc - 1;

        /* Allocazione dell'array di N pipe descriptors */
        piped = (pipe_t *) malloc (N*sizeof(pipe_t));
        if (piped == NULL)
        {
                printf("Errore nella allocazione della memoria\n");
                exit(2);
        }

        /* Creazione delle N pipe figli-padre */
        for (i=0; i < N; i++)
        {
                if (pipe(piped[i]) < 0)
                {
                        printf("Errore nella creazione della pipe\n");
                        exit(3);
                }
        }

        /* Le N pipe nipoti-figli deriveranno dalla creazione di una pipe in ognuno dei figli che poi creeranno un nipote */

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
                        /* in caso di errori nei figli o nei nipoti decidiamo di tornare -1 che corrispondera' al valore 255 che non puo' essere un valore accettabile di ritorno */

                        /* Chiusura delle pipe non usate nella comunicazione con il padre */
                        for (j=0; j < N; j++)
                        {
                                close(piped[j][0]);
                                if (i != j) close(piped[j][1]);
                        }

                        /* prima creiamo la pipe di comunicazione fra nipote e figlio */
                        if (pipe(p) < 0)
                        {
                                printf("Errore nella creazione della pipe\n");
                                exit(-1); /* si veda commento precedente */
                        }

                        if ( (pid = fork()) < 0) /* ogni figlio crea un nipote */
                        {
                                printf("Errore nella fork di creazione del nipote\n");
                                exit(-1); /* si veda commento precedente */
                        }
                        if (pid == 0)
                        {
                                /* codice del nipote */
                                /* in caso di errori nei figli o nei nipoti decidiamo di tornare -1 che corrispondera' al valore 255 che non puo' essere un valore accettabile di ritorno */

                                /* printf("DEBUG-Sono il processo nipote del figlio di indice %d e pid %d sto per eseguire il comando sort -f per il file %s\n", i, getpid(), argv[(i*2)+1]); */
                                /* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
                                close(piped[i][1]);

                                /* ogni nipote deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
                                close(1);
                                dup(p[1]);
                                /* ogni nipote adesso puo' chiudere entrambi i lati della pipe: il lato 0 NON viene usato e il lato 1 viene usato tramite lo standard ouput */
                                close(p[0]);
                                close(p[1]);

                                /* Costruiamo la stringa di opzione per il comando sort */
                                sprintf(opzione, "-%c", 'f');
                                /* Il nipote diventa il comando sort: bisogna usare le versioni dell'exec con la p in fondo in modo da usare la variabile di ambiente PATH  */
                                execlp("sort", "sort", opzione, argv[i+1], (char *)0);

                                /* Non si dovrebbe mai tornare qui!!*/
                                /* usiamo perror che scrive su standard error, dato che lo standard output e' collegato alla pipe */
                                perror("Problemi di esecuzione della head da parte del nipote");
                                exit(-1); /* si veda commento precedente */
                        }
                        /* codice figlio */
                        /* chiusura lato della pipe nipote-figlio non usato */
                        close(p[1]);
                        /* adesso il figlio legge dalla pipe la PRIMA linea */
                        S.pid_nipote=pid; /* inizializziamo il campo con il pid del processo nipote */
                        j=0; /* inizializziamo l'indice della linea */
                        while (read(p[0], &(S.linea_letta[j]), 1))
                        {
                         	/* se siamo arrivati alla fine di una linea */
                                if (S.linea_letta[j] == '\n')
                                {       
				   	/* aggiorniamo il campo con la lunghezza in caratteri della linea */
					S.lung_linea=j+1; /* la lunghezza della linea compreso il terminatore e' j+1 (se fosse richiesta la lunghezza della linea non compreso il terminatore, sarebbe solo j) */	
                                       	/* nell'array linea_letta abbiamo l'ultima linea ricevuta */
					break; /* appena leggiamo una linea, che sara' la prima, usciamo dal ciclo di lettura */
					/* N.B. Non serve azzerare j dato che usciamo dopo la lettura della prima linea */
				}	
                                else
                                       	j++; /* incrementiamo l'indice della linea */
                       	}
                        /* il figlio comunica al padre */
                        write(piped[i][1], &S, sizeof(S));

                        /* il figlio deve aspettare il nipote non solo per correttezza ma per evitare che il nipote riceva il segnale SIGPIPE dato che il figlio legge solo la PRIMA linea */
                        pid = wait(&status);
                        if (pid < 0)
                        {
                               	printf("Errore in wait\n");
                                exit(-1); /* si veda commento precedente */
                        }
                        if ((status & 0xFF) != 0)
                        {
                                printf("Nipote con pid %d terminato in modo anomalo\n", pid);
                                exit(-1); /* si veda commento precedente */
                        }
                        else
                        {
                                /* questa stampa non e' richiesta dal testo */
                                /* printf("DEBUG-Il nipote con pid=%d ha ritornato %d\n", pid, (int)((status >> 8) & 0xFF)); */
				/* il figlio ritorna la lunghezza della linea inviata NON compreso il terminatore (se fosse compreso il terminatore il valore di lung_linea NON andrebbe decrementato) */
				ritorno=S.lung_linea-1;
                        	exit(ritorno); 
                	}
                }
        }

/* Codice del padre */
/* Il padre chiude i lati delle pipe che non usa */
        for (i=0; i < N; i++)
                close(piped[i][1]);

/* Il padre recupera le informazioni dai figli: in ordine di indice */
        for (i=0; i<N; i++)
        {
         /* si legge la struttura inviata  dal figlio i-esimo */
            nr = read(piped[i][0], &S, sizeof(S));
            if (nr != 0)
            {
            	/* Nota bene: la stampa della linea con il formato %s richiede che il padre inserisca il terminatore di stringa nel posto giusto e cioe' DOPO il terminatore di linea */
                S.linea_letta[S.lung_linea]='\0'; /* dato che lung_linea rappresenta la lunghezza della linea, compreso il terminatore, per avere l'indice giusto SENZA SOVRASCRIVERE lo '\n' dobbiamo usare tale valore come indice */
                printf("Il nipote con pid %d ha letto dal file %s questa linea '%s' che ha lunghezza (compreso il terminatore) di %d caratteri:\n",  S.pid_nipote, argv[i+1], S.linea_letta,S.lung_linea);
             }
         }

        /* Il padre aspetta i figli */
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
                { 	ritorno=(int)((status >> 8) & 0xFF);
                  	printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi nel figlio o nel nipote)\n", pid, ritorno);
                }
        }
        exit(0);
}
