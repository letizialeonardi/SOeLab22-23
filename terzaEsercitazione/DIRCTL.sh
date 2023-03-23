#!/bin/sh
echo SONO DIRCTL.sh
if test $# -ne 1 	#controlliamo il numero di parametri e se non e' uguale a 1 diamo un errore!
then 	echo Errore: voglio un parametro, mentre ne hai passati $#
	exit 1
fi
echo 'Il valore di $0 ===>' $0
echo 'Il valore di $1 ===>' $1
echo "DEBUG-Ora eseguo ls -l $1"
ls -l $1
