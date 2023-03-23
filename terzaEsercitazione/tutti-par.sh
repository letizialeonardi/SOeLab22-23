#!/bin/sh
#file comandi tutti-par.sh 
case $# in	#controlliamo il numero di parametri
0) echo $# non numero giusto di parametri!
   exit 1;;
*) echo DEBUG-$# numero giusto di parametri!;;
esac
  
#eseguiamo quanto richiesto e cioe' la stampa di tutti i parametri sia in forma sintetica che specifica
echo si sta eseguendo per tutti i parametri: $*
for i 
do
	echo Parametro = $i
done

