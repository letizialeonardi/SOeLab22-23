#include <stdio.h>
#include <stdlib.h>

char *s=(char *)64000;

int main()
{
	printf("SONO UN PROGRAMMA CHE VA IN ERRORE e CREA UN CORE\n");
	printf("Dammi una stringa\n");
	printf("ATTENZIONE NON ABBIAMO ALLOCATO LA MEMORIA PER LA STRINGA\n");
	scanf("%s", s);
	printf("Non si arrivera' mai qui!!!!!!\n");

	exit(0);
}
