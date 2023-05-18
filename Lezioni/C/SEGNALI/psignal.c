/* FILE: psignal.c */
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/* ricordarsi di usare questo programma usando la ridirezione su /dev/null */
void catchint(int); /* prototipo/dichiarazione della funzione che rappresenta il gestore/handler del segnale SINGINT */

int main() 
{   int i;	/* indice/contatore */

	/* agganciamo il segnale alla funzione gestore/handler di nome catchint */
	signal(SIGINT, catchint); 	/* il nome di una funzione corrisponde al puntatore al suo codice */

	for (;;)	/* ciclo infinito */
		for (i =0 ; i < 32000;  i++) 
			printf (" i vale %d\n", i);
}
 
void catchint(int signo)  /* alla funzione associata al segnale, il kernel passa il numero del segnale che l'attiva */
{ 
   int fd;
   char s[100] = "Segnale nro  ";
   char s1[5];
   char s2[] = " e count = ";
   char s3[] = "\n";
   static int count = 0;  /* variabile contatore definita static e quindi allocata non nello stack, ma nei dati globali che hanno tempo di vita pari a quella del programma e quindi del processo! */

 	/* NON si disabilita il segnale SIGINT: dato che abbiamo verificato che si usi la semantica BSD */
	sprintf(s1, "%d", signo);
	strcat(s, s1);
	strcat(s, s2);
	count++;
	sprintf(s1, "%d", count);
	strcat(s, s1);
	strcat(s, s3);
	/* viene usato un file di nome sig.log (che creeremo dalla shell) per memorizzare le informazioni relative al numero del segnale e al contatore */
	if ((fd = open("sig.log", O_WRONLY)) < 0) exit(1);	/* se non si riesce ad aprire in sola scrittura il file, usciamo con errore */
	lseek(fd, 0L, 2);	/* se la open ha avuto successo, spostiamo l'I/O pointer alla fine in modo da appendere la stringa che abbiamo 'costruito' */
	/* volendo si puo' usare anche la versione estesa della open che apre il file in APPEND */
	write(fd, s, strlen(s));
 	/* non si prevedono azioni di terminazione: ritorno al segnalato, SENZA BISOGNO DI RIPRISTINARE la catch function sempre secondo la semantica BSD */
}
