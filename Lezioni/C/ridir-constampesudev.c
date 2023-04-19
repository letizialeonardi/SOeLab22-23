#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define PERM 0644   /* in ottale per diritti UNIX */

int fd; /* per file/dispositivo /dev/tty; NOTA BENE: fd l'abbiamo definita come variabile globale perche' ci serve sia in copyfile che nel main! */

int copyfile (char *f1, char * f2)
{    int infile, outfile, nread;	/* usati per open e valore ritorno read */
     char s[100] = "Valore di infile ";	/* stringa usata nella funzione sprintf */
     char s1[5];			/* stringa per infile e poi outfile */
     char s2[] = " e di outfile ";	/* stringa ulteriore */
     char s3[] = "\n";			/* stringa conclusiva */
     char buffer[BUFSIZ];  		/* usato per i caratteri */

	fd = open("/dev/tty", O_WRONLY);	/* apriamo in sola scrittura il terminale corrente */

	close(0);	/* vogliamo "simulare" le operazioni compiute dal SO per implementare la ridirezione, quindi per prima cosa facciamo una close dello standard input e quindi di 0! */ 
	if ((infile = open(f1, O_RDONLY)) < 0) return 2;
	/* ERRORE se non si riesce ad aprire in LETTURA il primo file */
	/* se la open ha successo viene occupato l'elemento di posto 0 della TFA del processo corrente e quindi il primo file verra' usato come standard input */

	close(1);	/* continuando la nostra "simulazione" delle operazioni compiute dal SO per implementare la ridirezione, dobbiamo ora passare a fare la close dello standard output e quindi di 1! */
	if ((outfile = creat(f2, PERM) ) <0)
	/* ERRORE se non si riesce a creare il secondo file */
        	{ close(infile); return 3; }
	/* se la creat ha successo viene occupato l'elemento di posto 1 della TFA del processo corrente e quindi il secondo file verra' usato come standard output */

	/* STAMPA DI DEBUGGER */
	sprintf(s1, "%d", infile);	/* ricaviamo la stringa corrispondente a infile */
	strcat(s, s1);			/* la concateniamo alla stringa s */
	strcat(s, s2);			/* quindi conteniamo s2 */
	sprintf(s1, "%d", outfile);	/* ricaviamo la stringa corrispondente a outfile */
	strcat(s, s1);			/* la concateniamo alla stringa s */
	strcat(s, s3);			/* quindi conteniamo s3 */
	write(fd, s, strlen(s));	/* ora scriviamo su fd e quindi su /dev/tty */

	while ((nread = read(infile, buffer, BUFSIZ)) > 0 )
	{ if (write(outfile, buffer, nread) < nread)
		/* ERRORE se non si riesce a SCRIVERE */
    		{ close(infile); close(outfile); return 4; }
	}
	/* N.B. infile vale 0 e outfile vale 1 e quindi abbiamo implementato "a mano" la ridirezione */

	/* se arriviamo qui, vuol dire che tutto e' andato bene */
	close(infile); 
	close(outfile); 
	return 0;
}

int main(int argc, char** argv)
{   int status;
    if (argc != 3)  /* controllo sul numero di parametri: devono essere esattamente 2! */
    {       printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
            printf("Ci vogliono 2 argomenti: nome-file-sorgente e nome-file-destinazione\n");
            exit(1);
    }
    status = copyfile(argv[1], argv[2]);   /* invochiamo la funzione copyfile passando le due stringhe passate come parametri */
    if (status != 0)
  	write(fd, "Ci sono stati degli errori durante la copia\n", 44);
	/* ATTENZIONE: non si può usare la printf! N.B. 44 sono i caratteri di cui e' costituita la stringa; se volessimo stampare come prima il valore di status bisognerebbe usare sprintf in modo analogo a prima! */
    exit(status);
}
