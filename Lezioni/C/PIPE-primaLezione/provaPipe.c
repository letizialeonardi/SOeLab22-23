/* FILE: provaPipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main ()
{  	
   int piped[2]; 	/* array di due interi per la pipe */

	/* si crea una pipe */
	if (pipe(piped) < 0 ) 
	{ 
               	printf("Errore nella creazione pipe\n");
		exit(1); 
	}

	printf("creato pipe con piped[0]= %d \n", piped[0]);
	printf("creato pipe con piped[1]= %d \n", piped[1]);

	exit(0);
}
