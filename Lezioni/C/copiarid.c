#include <stdio.h>
#include <unistd.h>	
#include <stdlib.h>	

int main()
/* NOTA BENE: NON ABBIAMO BISOGNO DI PARAMETRI! */
{    	int nread;        		/* valore ritorno read */
	char buffer[BUFSIZ];           /* usato per i caratteri */

	while ((nread = read(0, buffer, BUFSIZ)) > 0 )	/* ciclo di lettura DALLO STANDARD INPUT (0) fino a che nread e' maggiore di 0 */
                write(1, buffer, nread);	/* scriviamo sempre nread byte SULLO STANDARD OUTPUT (1)! */
	exit(0);
}

