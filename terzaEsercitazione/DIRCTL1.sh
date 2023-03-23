#!/bin/sh
echo SONO DIRCTL1.sh
if test $# -ne 1 	#controlliamo il numero di parametri e se non e' uguale a 1 diamo un errore!
then 	echo Errore: voglio un parametro, mentre ne hai passati $#
	exit 1
fi
echo 'Il valore di $0 ===>' $0
echo 'Il valore di $1 ===>' $1
if test -f $1	#controlliamo se e' un file
then 	echo $1 file
	echo "DEBUG-Ora eseguo ls -l $1"
	ls -l $1
else
	if test -d $1	#controlliamo se e' una directory
	then	echo $1 directory
		echo "DEBUG-Ora eseguo ls -ld $1"
		ls -ld $1
	else	#altrimenti non esiste
		echo $1 non file o non directory
	fi
fi
