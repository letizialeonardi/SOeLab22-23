#!/bin/sh
#file comandi for2Bis.sh
for i in p*	#la lista del for sara' costituita da tutti i nomi di file/directory il cui nome inizia per p; se tale lista fosse nulla il for non eseguira' nulla!
do
	echo ELEMENTO CORRENTE: $i	#stampiamo il nome corrente
	if test -f $i			#verifichiamo che sia un file 
	then 
		echo DEBUG-VISUALIZZAZIONE DI $i
		echo ===	#inseriamo delle separazioni per rendere piu' chiara la stampa
		cat $i
		echo ===	#inseriamo delle separazioni per rendere piu' chiara la stampa
		echo `pwd`/$i >> /tmp/t		#riga aggiunta: si va a scrivere in append il nome assoluto del file corrente
	else 
		#si puo' dare per scontato che se non e' un file e' una directory
		echo $i e\' una directory #NOTARE CHE SE NELLE STRINGHE STAMPATE CON echo VOGLIAMO USARE L'APOSTROFO (in questo caso usato come accento) DOBBIAMO USARE IL CARATTERE DI ESCAPE!
	fi
done
