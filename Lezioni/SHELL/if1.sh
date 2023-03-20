#!/bin/sh
#file comandi con primo esempio di if
if grep $1 $2 	#la 'condizione' dell'if corrisponde all'esecuzione del comando grep
		#se il grep ha successo (valore di ritorno = 0) allora si eseguira' la strada then
		#altrimenti la strada else
then 	echo TROVATO STRINGA $1 NEL FILE $2
else 	echo NON TROVATO STRINGA $1 NEL FILE $2
fi
