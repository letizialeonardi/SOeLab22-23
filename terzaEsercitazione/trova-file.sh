#!/bin/sh
#file comandi trova-file.sh 
case $# in 	#controlliamo che sia invocato con un solo parametro
1) echo DEBUG-OK un solo parametro $1 ;;				#caso giusto!
*) echo echo $# non numero giusto di parametri! Uso: $0 file	#casi NON giusti!
   exit 1;;
esac
#controllo che il primo parametro sia dato in forma relativa semplice 
case $1 in
*/*) echo Errore: $1 non in forma relativa semplice 	#notare che questa forma di controllo ingloba anche il caso /*
     exit 2;;
*) ;;
esac
if test -f $1 	#controlliamo che sia un file
then
	echo DEBUG-il file $1 esiste e si trova in `pwd`	#stampa aggiunta, ma non richiesta
	echo il suo nome assoluto e\' `pwd`/$1			#UNICA STAMPA RICHIESTA
else 	echo DEBUG-non esiste un file di nome $1 in `pwd`	#stampa aggiunta, ma non richiesta
fi
