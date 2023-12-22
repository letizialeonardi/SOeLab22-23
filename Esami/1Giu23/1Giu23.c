/* Soluzione della Seconda Prova in Itinere del 1 Giugno 2023 (solo ParteC) */
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
        char linea_letta[250];  /* campo c2 del testo */
                                /* bastano 250 caratteri per contenere ogni riga insieme con il terminatore di linea e con il terminatore di stringa (vedi specifica) */
        int lung_linea;         /* campo c3 del testo */
} Strut;
/* nome tipo indicato nel testo */

int main(int argc, char **argv)
{
        /* -------- Variabili locali ---------- */
        int pid;                        /* process identifier per le fork() */
        int N;                          /* numero di file passati sulla riga di comando (uguale al numero di numeri) */
	/* nome indicato nel testo */
        int status;                     /* variabile di stato per la wait */
        pipe_t *piped;                  /* array dinamico di pipe descriptors per comunicazioni figli-padre  */
        pipe_t p;                       /* una sola pipe per ogni coppia figlio-nipote */
        int n, j;                       /* indici per i cicli */
	/* n nome indicato nel testo */
	char buffer[250];		/* array di caratteri usato dai figli per ricevere dai nipoti */
	/* nome indicato dal testo */
	int nr;                      	/* variabile per valore di ritorno della read */
        int ritorno;                    /* variabile che viene ritornata da ogni figlio al padre */
        Strut S;                        /* struttura usata dai figli e dal padre */
	/* nome indicato nel testo */

        /* ------------------------------------ */

        /* Controllo sul numero di parametri */
        if (argc < 3 ) /* Meno di due parametri */
        {
                printf("Errore nel numero dei parametri dato che argc = %d\n", argc);
                exit(1);
        }

        /* Calcoliamo il numero di file passati e quindi di processi figli */
        N = argc - 1;

        /* Allocazione dell'array di N pipe descriptors */
        piped = (pipe_t *) malloc (N*sizeof(pipe_t));
        if (piped == NULL)
        {
                printf("Errore nella allocazione della memoria\n");
                exit(2);
        }

        /* Creazione delle N pipe figli-padre */
        for (n=0; n < N; n++)
        {
                if (pipe(piped[n]) < 0)
                {
                        printf("Errore nella creazione della pipe figli-padre per l'indice %d\n", n);
                        exit(3);
                }
        }

        /* Le N pipe nipoti-figli deriveranno dalla creazione di una pipe in ognuno dei figli che poi creeranno un nipote */

        /* Ciclo di generazione dei figli */
        for (n=0; n < N; n++)
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
			/* ogni figlio scrive solo su piped[n] */
                        for (j=0; j < N; j++)
                        {
                                close(piped[j][0]);
                                if (n != j) close(piped[j][1]);
                        }

                        /* prima creiamo la pipe di comunicazione fra nipote e figlio */
                        if (pipe(p) < 0)
                        {
                                printf("Errore nella creazione della pipe nipote-figlio per l'indice %d\n", n);
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

                                /* printf("DEBUG-Sono il processo nipote del figlio di indice %d e pid %d sto per eseguire il comando rev per il file %s\n", i, getpid(), argv[(i*2)+1]); */
                                /* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
                                close(piped[n][1]);

                                /* ogni nipote deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
                                close(1);
                                dup(p[1]);
                                /* ogni nipote adesso puo' chiudere entrambi i lati della pipe: il lato 0 NON viene usato e il lato 1 viene usato tramite lo standard output */
                                close(p[0]);
                                close(p[1]);

                                /* Il nipote diventa il comando rev: bisogna usare le versioni dell'exec con la p in fondo in modo da usare la variabile di ambiente PATH  */
                                execlp("rev", "rev", argv[n+1], (char *)0);
				/* avendo decido di seguire la specifica e quindi di usare il COMANDO rev e quindi NON avendo operato la ridirezione dello standard input, dobbiamo passare il nome del file come unico parametro di invocazione */

                                /* Non si dovrebbe mai tornare qui!!*/
                                /* usiamo perror che scrive su standard error, dato che lo standard output e' collegato alla pipe */
                                perror("Problemi di esecuzione della head da parte del nipote");
                                exit(-1); /* si veda commento precedente */
                        }
                        /* codice figlio */
                        /* chiusura lato della pipe nipote-figlio non usato */
                        close(p[1]);
                        /* adesso il figlio legge dalla pipe fino a che ci sono caratteri e cioe' linee inviate dal nipote tramite il comando rev */
                        j=0; /* inizializziamo l'indice della linea */
                        S.pid_nipote=pid; /* inizializziamo il campo con il pid del processo nipote */
                        while (read(p[0], &(buffer[j]), 1))
                        {
                             	/* se siamo arrivati alla fine di una linea */
                              	if (buffer[j] == '\n')
                               	{       
				   	/* aggiorniamo il campo con la lunghezza in caratteri della linea */
					S.lung_linea=j+1; /* la lunghezza della linea compreso il terminatore e' j+1 (se fosse richiesta la lunghezza della linea NON compreso il terminatore, sarebbe j) */	
                                       	/* nell'array linea_letta abbiamo l'ultima linea ricevuta */
                                       	j=0; /* riazzeriamo l'indice per la prossima linea */ 
				}	
                               	else
                                       	j++; /* incrementiamo l'indice della linea */
                        }
			/* in buffer abbiamo l'ultima linea letta e quindi la possiamo copiare nel campo corretto della struct che deve essere inviata al padre */
			for (j=0; j<S.lung_linea; j++)
				S.linea_letta[j]=buffer[j];
			/* N.B.: non si puo' usare strcpy dato che buffer NON e' una stringa; in alternativa al for andava bene usare sia memcpy(S.linea_letta, buffer, S.lung_linea) che strncpy con gli stessi parametri */
			/* ATTENZIONE CHE SE S.lung_linea rappresentasse la lunghezza SENZA il terminatore, nel for precedente o nel codice alternativo sopra indicato si dovrebbe usare S.lung_linea+1 */
                        /* il figlio comunica al padre */
                        write(piped[n][1], &S, sizeof(S));

                        /* il figlio deve aspettare il nipote per correttezza */
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
			       /* ATTENZIONE VA FATTA L'EXIT ANCHE IN QUESTO CASO, MENTRE NEL PADRE NON SI FA PERCHE' DEVE ASPETTARE TUTTI I FIGLI */
                        }
                        else
                        /* printf("DEBUG-Il nipote con pid=%d ha ritornato %d\n", pid, (int)((status >> 8) & 0xFF)); */
			/* il figlio ritorna la lunghezza della linea inviata NON compreso il terminatore */
			ritorno=S.lung_linea-1;	/* se il file NON esiste il nipote non avrà mandato nulla e quindi S.lung_linea sara' uguale a zero e quindi togliendo 1 si ottiene di tornare al padre un -1 che viene interpretato come un errore */
			/* in alternativa si poteva controllare il valore di ritorno del nipote che se diverso 0 vuol dire che c'e' stato un errore */
                        exit(ritorno); 
                }
        }

/* Codice del padre */
/* Il padre chiude i lati delle pipe che non usa */
        for (n=0; n < N; n++)
                close(piped[n][1]);

/* Il padre recupera le informazioni dai figli: in ordine di indice */
        for (n=0; n<N; n++)
        {
         /* si legge la struttura inviata  dal figlio i-esimo */
            nr = read(piped[n][0], &S, sizeof(S));
            if (nr != 0)
            {
            	/* Nota bene: la stampa della linea con il formato %s richiede che il padre inserisca il terminatore di stringa nel posto giusto, cioe' dopo il terminatore di linea dato che il testo specificava "mantenendo pero' il fatto che sia una linea!" */
                S.linea_letta[S.lung_linea]='\0'; /* dato che lung_linea rappresenta la lunghezza della linea, compreso il terminatore, per avere l'indice giusto lo usiamo direttamente */
                printf("Il nipote con pid %d ha letto dal file %s questa linea '%s' che ha lunghezza (compreso il terminatore) di %d caratteri:\n", S.pid_nipote, argv[n+1], S.linea_letta, S.lung_linea);
		/* si poteva scegliere qualunque ordine per stampare le varie informazioni richieste: nel caso in cui si fosse deciso che la linea fosse l'ultima informazione da stampare o se comunque si fosse deciso di stampare le varie informazioni su linee diverse e si fosse deciso di NON mettere nella stringa di formato lo '\n' dopo la stampa della linea, si doveva inserire un commento esplicativo */
             }
         }

        /* Il padre aspetta i figli */
        for (n=0; n < N; n++)
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
