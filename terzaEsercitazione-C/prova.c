/* FILE: prova.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
   char *argin[2]; 	/* array statico di puntatori a char che serve per passare i parametri alla execvp */
   int x;		/* per leggere valore dallo standard input */

	argin[0]="prova";	/* argin[0] e' un puntatore a char cui viene assegnato il puntatore alla stringa "prova" */
	argin[1]= (char *)0;	/* argin[1] e' un puntatore a char cui viene assegnato il valore 0 come puntatore a char */

	printf("Esecuzione di prova\n");
	printf("Dimmi se vuoi finire (valore 0)!\n");
	scanf("%d", &x);
	if (x != 0)
	{
		execvp(argin[0], argin);

		/* si esegue l'istruzione seguente SOLO in caso di fallimento della execvp */
		printf("Errore in execvp\n");
		exit(1);	/* torniamo quindi un valore diverso da 0 per indicare che ci sono stati dei problemi */
	}
	else 	exit(x);	/* notare che x sara' 0! */
}
