/* 
Si progetti, utilizzando il linguaggio C e le primitive di basso livello che operano sui file, un filtro che accetta due parametri: il primo parametro deve essere il nome di un file (F), mentre il secondo deve essere considerato un singolo carattere (C).  Il filtro deve operare una modifica del contenuto del file F: in particolare, tutte le occorrenze del carattere C nel file F devono essere sostituite con il carattere spazio. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv)
{  int fd;		/* variabile per valore di ritorno della open */
   char Cx;		/* serve per salvare il carattere, selezionandolo dal secondo parametro, che va cercato nel file */
   char c;		/* variabile per caratteri letti dal file */
        
	if (argc != 3)  /* controllo sul numero di parametri: devono essere esattamente 2! */
        {       printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
                printf("Ci vogliono 2 argomenti: nome-file e singolo carattere\n");
		exit(1);
	}

	/* ora facciamo il controllo se il primo parametro e' un file (plausibile fatto con una open) che deve essere sia leggibile che scrivibile: NOTA BENE che in questo caso la open va fatta in lettura e scrittura */
	if ((fd = open(argv[1], O_RDWR)) < 0)
        { 	printf("Errore in apertura file argv[1] = %s\n", argv[1]);
                exit(2); }

	if (strlen(argv[2]) != 1)	/* controlliamo che il secondo parametro sia un singolo carattere: N.B. Non viene chiesto che sia un carattere alfabetico o di una particolare categoria! */
        { 	printf("Errore: non singolo carattere dato che argv[2] = %s\n", argv[2]);
		exit(3); 
	}
	
	/* isoliamo il singolo carattere per maggior comodita' */
	Cx=argv[2][0];

	/* se arriviamo qui, vuol dire che tutto e' andato bene */
	while (read(fd, &c, 1)) /* ATTENZIONE: dobbiamo leggere un solo carattere alla volta */
	/* quindi usiamo &c; inoltre, NON serve indicare nulla nella condizione, dato che la read tornera' 1 in caso di successo e quindi si entrera' nel ciclo, oppure 0 e allora si uscira' dal ciclo perche' saremo arrivati alla fine del file */
          	if (c == Cx)	/* se il carattere corrente e' uguale al carattere passato ... */
                { 	lseek(fd, -1L, 1);	/* SI DEVE RIPORTARE INDIETRO IL FILE POINTER */
			/* si puo' usare anche SEEK_CUR invece che 1 come terzo parametro */
                  	write(fd, " ", 1);	/* si deve scrivere il carattere SPAZIO: N.B. se si vuole si puo' definire una variabile char blank=' ' e quindi passare &blank come secondo parametro */
                }
	exit(0);
}
