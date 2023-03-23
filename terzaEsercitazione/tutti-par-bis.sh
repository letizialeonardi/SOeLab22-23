#!/bin/sh
#file comandi tutti-par-bis.sh 
count=0	#variabile per contare i parametri via via nel for
case $# in 	#controlliamo il numero di parametri
0) echo $# non numero giusto di parametri!
   exit 1;;
*) echo DEBUG-$# numero giusto di parametri!;;
esac

#eseguiamo quanto richiesto e cioe' la stampa di tutti i parametri sia in forma sintetica che specifica con l’indicazione del loro ‘nome’ ($1, $2, etc.)
echo si sta eseguendo per tutti i parametri: $*
for i 
do
	count=`expr $count + 1` 	#incrementiamo il contatore ad ogni iterazione
	echo Parametro \$$count = $i	#N.B. \$ ha come effetto di scrivere il carattere $ sullo standard output, mentre $count rappresenta il numero crescente del contatore!
done

