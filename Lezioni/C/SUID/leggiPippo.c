/* FILE: leggiPippo.c */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
 
int main(int argc, char **argv)
{
   int fd;	/* per la open */
   char c;	/* per leggere i caratteri dal file pippo */

	printf("real-user id = %d\n", getuid());
	printf("effective-user id = %d\n", geteuid());
	printf("SONO %s\n", argv[0]);
	printf("Sto per leggere il file pippo\n");

	if ((fd=open("pippo", O_RDONLY)) < 0) 
	{ 	printf("ERRORE in apertura\n");
		exit(1);
	}

	/* andiamo a leggere un carattere alla volta dal file pippo anche se a livello di prestazioni non sarebbe la cosa piu' furba! */
	while(read(fd, &c, 1))
  		write(1, &c, 1);

	exit(0);
}
