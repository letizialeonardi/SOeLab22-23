#!/bin/sh
#file comandi con secondo esempio di if
if grep $1 $2 > /dev/null 2>&1 	#rispetto a if1.sh ridiretto sia standard output che error su /dev/null
then 	echo TROVATO STRINGA $1 NEL FILE $2
else 	echo NON TROVATO STRINGA $1 NEL FILE $2
fi

