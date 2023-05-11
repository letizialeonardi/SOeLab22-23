/* file provaEsame1.c */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h> 	/* per usare isdigit() e isalpha() */

int main(int argc, char *argv[])
{
   int p[2][2]; 	/* p e' un array di due pipe: l'elemento di posto 0 (p[0]) serve per la comunicazione fra i processi dispari e il padre, mentre l'elemento di posto 1 (p[1]) serve per la comunicazione fra i processi pari e il padre */
   int N; 		/* N e' il numero di processi da creare (uno per ogni file passato) */
   int pid;		/* pid variabile usata per il valore di ritorno della fork */
   int fdr;		/* fdr file descriptor usato da ogni figlio per aprire in lettura il proprio file */
   int i;		/* indice */
   int tot=0; 		/* tot variabile usata dal padre per contare i caratteri scritti su standard output */
   int nr0, nr1; 	/* nr0 numero di caratteri letti dalla pipe 'pari' e nr1  numero di caratteri letti dalla pipe 'dispari' */
   char ch, ch0, ch1; 	/* ch e' il carattere letto da ogni file; ch0 e' il carattere letto dal padre sulla pipe 'pari' e ch1 e' il carattere letto dal padre su quella 'dispari' */
   int pidFiglio, status, ritorno;    /* variabili per wait */
  
  	if (argc < 3) 	/* e' specificato che devono essere passati almeno due nomi di file! */
  	{ 	printf("Errore nel numero dei parametri: %d\n", argc);
    		exit(1);
  	}
              
  	N=argc-1;	/* calcoliamo il numero dei file passati e, di conseguenza, il numero di processi figli da creare */

  	/* creazione delle due pipe: quella 'pari' e quella 'dispari' */
  	if (pipe(p[0]) < 0) 	/* pipe per i caratteri numerici che sara' usata in scrittura dai figli dispari che sono quelli che leggono i file di posizione pari */
  	{ 	printf("Errore nella creazione della prima pipe\n");
    		exit(2);
  	}
  	if (pipe(p[1]) < 0) 	/* pipe per i caratteri alfabetici che sara' usata in scrittura dai figli pari che sono quelli che leggono i file di posizione dispari */
  	{ 	printf("Errore nella creazione della seconda pipe\n");
    		exit(3);
  	}
                
  	printf("DEBUG-Sono il processo padre con pid %d e sto per generare %d figli\n", getpid(), N);

  	for (i=0; i < N; i++)
  	{
    		/* si creano N figli */
    		if ((pid = fork()) < 0)
    		{	printf ("Errore nella fork\n");
      			exit(4);
    		}
    
    		if (pid == 0) /* ogni figlio */
    		{
  			/* commentiamo la stampa di debugger dei figli altrimenti si inteferisce con la stampa su standard ouput dei caratteri scritti dal padre 
			 * printf("DEBUG-Sono il processo figlio con pid %d e sono associato al file %s\n", getpid(), argv[i+1]);
			*/

			/* chiusura lati pipe non utilizzati */
      			close(p[0][0]); /* chiude i lati di lettura di entrambe le pipe */ 
      			close(p[1][0]);
      			close(p[i % 2][1]); /* chiude il lato di scrittura della pipe non usata: quindi se i e' pari si chiude il lato di scrittura di p[0], altrimenti se i e' dispari si chiude quello di p[1] */
      			/* apertura del file associato */
      			if ((fdr = open(argv[i+1], O_RDONLY)) < 0) /* il processo di indice i deve aprire il file di indice i+1 */
			/* N.B. i processi di indice pari (ad esempio 0) leggono i file di indice dispari (ad esempio argv[1]) e quindi devono selezionare i caratteri alfabetici, mentre i processi di indice dispari (ad esempio 1) leggono i file di indice pari (ad esempio argv[2]) e quindi devono selezionare i caratteri numerici */ 
      			{ 	printf("Errore nell'apertura del file %s\n", argv[i+1]);
        			exit(-1); /* in caso di errore torniamo -1 che verra' interpretato dal padre come 255 */
      			}
      			while(read(fdr, &ch, 1) > 0)	/* ogni figlio legge via via tutti i caratteri dal proprio file associato */
      			{
        			if ((((i % 2) == 0) && isalpha(ch)) || 		/* il file di indice dispari e' letto in realta' da un processo di indice pari e quindi si controlla che il carattere letto sia un alfabetico */
            				(((i % 2) == 1) && isdigit(ch)))   	/* il file di indice pari e' letto in realta' da un processo di indice dispari e quindi si controlla che il carattere letto sia un numerico */
          					write(p[(i+1) % 2][1], &ch, 1); /* comunica il carattere al padre, scrivendo sulla pipe 0 i numerici e sulla pipe 1 gli alfabetici */
      			}
      			exit(ch); /* viene specificato di tornare l'ultimo carattere letto dal figlio */
      			/* NOTA BENE: questa exit e' importantissima altrimenti ogni processo figlio rimarrebbe nel ciclo di creazione e a sua volta creerebbe dei figli che a loro volta creerebbero dei figli */
    		}
  	}

  	/* padre */
  	close(p[0][1]); /* chiude i lati di scrittura */
  	close(p[1][1]);
  	/* il testo specifica che si deve continuare a leggere dalle due pipe anche se non si riesce a garantire l'alternanza in output a causa del termine dei processi pari o dei processi dispari; appena pero' finiscono entrambi i 'tipi' di processi il padre deve smettere di leggere dalle pipe */
  	nr0 = read(p[0][0], &ch0, 1);
  	nr1 = read(p[1][0], &ch1, 1);
  	while ((nr0 != 0) || (nr1 != 0))
  	{
    		tot = tot + nr0 + nr1; /* il totale viene ricavato sommando via via i valori di ritorno delle read da pipe, ricordando che uno dei due puo' anche essere 0 nel qual caso una delle due write seguenti NON avra' effetto */
    		write(1, &ch1, nr1); /* ATTENZIONE ALL'ORDINE: prima un alfabetico e poi un numerico (secondo la specifica) */
    		write(1, &ch0, nr0);
    		nr0 = read(p[0][0], &ch0, 1);
    		nr1 = read(p[1][0], &ch1, 1);
  	}
  	printf("\nNumero di caratteri scritti sullo standard output: %d\n", tot); /* stampa finale del numero di caratteri scritti */
	
  	/* Attesa della terminazione dei figli */
  	for(i=0;i<N;i++)
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
