/* FILE: shellpipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main  (int argc, char **argv) 
{
   int i, j;  				/* i e j come indici */ 
   char *com1[10], *com2[10];		/* array di 10 puntatori a char, supposti sufficienti per memorizzare il primo comando (con i suoi parametri) e il secondo comando (con i suoi parametri) che sono presenti nel piping */ 
   int pid, piped[2];                   /* pid per fork, piped per pipe */
   int pidFiglio, status, ritorno;      /* per wait padre */

   	/* questo processo e' quello che simula la SHELL: PROCESSO P1 */
	/* si devono fornire nella linea comandi due comandi distinti, separati dal carattere !. Non si usa direttamente il metacarattere |, perche` questo viene interpretato dallo shell come una pipe */
	if (argc < 4)   /* ci devono essere almeno tre stringhe, due che corrispondono ai due comandi in piping e una che deve contenere il carattere ! */
	{ 	printf("Errore numero di parametri insufficiente dato che argc=%d\n", argc); 
		exit(1); 
	}
	/* se il numero dei parametri e' giusto andiamo a separare le stringhe che corrispondono al primo comando e quelle che corrispondono al secondo comando, ricordando che il separatore e' rappresentato dalla stringa (con un unico carattere) "!" */
	/* N.B. NON andiamo a COPIARE le stringhe corrispondenti ai vari argv[i] (anche perche' NON abbiamo allocato la memoria per i vari puntatori di com1 e com2, ma semplicemente copiamo il puntatore ai vari argv[i] nei vari elementi di com1 e com2 in modo da rendere piu' chiaro il codice */
	for (i=1; i < argc && strcmp (argv[i], "!"); i++) 
		com1[i-1] = argv[i];	
    	com1[i-1] = (char *)0;  /* terminatore della lista di stringhe */   
	i++;   
	for (j = 1; i < argc ; i++, j++) 
		com2[j-1] = argv[i];
  	com2[j-1] = (char *)0;  /* terminatore della lista di stringhe */

	/* creazione del figlio per eseguire il comando in pipe */
	if ((pid = fork()) < 0)
	{       printf("Errore creazione figlio da parte del processo P1 che simula la SHELL\n");
		exit(2);
       	}
	if (pid == 0)
	{
		/* figlio: lo chiamiamo processo P2 */
						
		/* il figlio P2 esegue il comando intero: crea la pipe */
		if (pipe(piped) < 0 )  
		{ 	printf("Errore in creazione pipe\n");
			exit(-1) /* torniamo un valore che verra' interpretato 255 dal padre (cioe' P1)  */; 
		}
		/*  CREAZIONE di un nuovo FIGLIO: processo P3  */
		if ((pid = fork()) < 0)    
		{ 	printf("Errore nella seconda fork\n");
			exit(-1) /* torniamo un valore che verra' interpretato 255 dal padre (cioe' P2)  */; 
       		}	
		if (pid == 0)      /* P3: figlio del FIGLIO P2 */
		{	
			close(1);  /* lo standard output va messo sulla pipe */
			dup(piped[1]); 
			close(piped[0]); close(piped[1]); /* non servono piu' */
        		execvp(com1[0], com1);  
        		exit(-1);   /* errore in caso si ritorni qui */
		}       
		/*  processo P2 (padre di P3) */
		close(0);  /* lo standard input va preso dalla pipe */
       		dup(piped[0]);  
        	close(piped[0]); close(piped[1]); /* non servono piu' */
		execvp(com2[0], com2);  
        	exit(-1);   /* errore in caso si ritorni qui */
		/* N.B. Non si puo' inserire il codice per aspettare il figlio P3 dato che P2 esegue una exec! */
	}	

	/* padre P1:  attende il figlio P2 */
	if ((pidFiglio=wait(&status)) < 0)
	{
		printf("Errore in wait\n");
		exit(3);

	}
	if ((status & 0xFF) != 0)
    	       	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
    	else
    	{	
  		ritorno=(int)((status >> 8) & 0xFF);
       		printf("Il figlio con pid=%d ha ritornato %d\n", pidFiglio, ritorno);
    	}	

	exit(ritorno);	/* viene ritornato al processo SHELL quanto ritornato dal figlio (P2) */
} 
