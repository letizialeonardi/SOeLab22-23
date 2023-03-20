#!/bin/sh
#file comandi con quinto esempio di if
if test $# -ne 2	#controlliamo che i parametri passati siano esattamente 2
then			#se non sono 2, diamo una indicazione di errore e usciamo con un valore != 0
	echo $# non numero giusto di parametri!
	exit 1		#partiamo con il primo numero !=0
fi
#se il numero dei parametri e' giusto usiamo lo stesso codice di prima (if4.sh)
if test ! -f $2
then	#se non e' un file diamo una indicazione di errore e usciamo con un valore != 0
	echo il file $2 NON esiste
	exit 2		#N.B. il valore tornato in caso di questo errore e' 2
fi
grep $1 $2 > /dev/null 2>&1
if test $? -eq 0
then 	echo TROVATO STRINGA $1 NEL FILE $2
else 	echo NON TROVATO STRINGA $1 NEL FILE $2
fi
