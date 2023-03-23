#!/bin/sh
#file comandi primo-par.sh
FIRST=   #variabile per memorizzare il primo parametro 

if test $# -eq 0 #controlliamo il numero di parametri e se e' uguale a 0 diamo un errore!
then
  	echo Errore: ci vogliono dei parametri
  	exit 1
fi

FIRST=$1	#isoliamo il primo parametro
shift		#lo eliminiamo dalla lista dei parametri

#stampiamo il primo parametro e la lista degli altri parametri (escluso il primo)
echo Primo parametro = $FIRST
echo Lista parametri escluso primo = $*
