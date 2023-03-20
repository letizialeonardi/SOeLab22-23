#!/bin/sh
#file comandi con terzo esempio di if
grep $1 $2 > /dev/null 2>&1	#rispetto a if2.sh abbiamo 'estratto' il comando grep
if test $? -eq 0		#quindi ora nell'if usiamo un test verificando il valore di ritorno dell'ultimo comando e quindi del grep
then 	echo TROVATO STRINGA $1 NEL FILE $2
else 	echo NON TROVATO STRINGA $1 NEL FILE $2
fi

