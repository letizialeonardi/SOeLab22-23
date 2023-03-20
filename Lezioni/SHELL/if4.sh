#!/bin/sh
#file comandi con quarto esempio di if
if test ! -f $2 	#rispetto a if3.sh abbiamo aggiunto una verifica preliminare che il secondo parametro sia un file
#N.B. DI NORMA QUANDO SI DEVE FARE UN CONTROLLO SU UNA CERTA CONDIZIONE E SE QUELLA CONDIZIONE NON E' VERIFICATA SI DEVE PRODURRE UNA INDICAZIONE DI ERRORE, QUELLO CHE SI FA E' DI FARE IL CONTROLLO NEGATO DELLA CONDIZIONE IN MODO DA USCIRE SUBITO E RENDERE IL CODICE PIU' LEGGIBILE! 	
then	#se non e' un file diamo una indicazione di errore e usciamo con un valore != 0 e in particolare con 1
	echo il file $2 NON esiste
	exit 1
fi
#se invece e' un file usiamo lo stesso codice di prima (ad esempio di if3.sh)
grep $1 $2 > /dev/null 2>&1
if test $? -eq 0
then 	echo TROVATO STRINGA $1 NEL FILE $2
else 	echo NON TROVATO STRINGA $1 NEL FILE $2
fi
