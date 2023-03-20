#!/bin/sh
#file comandi LeggiEMostra.sh 
#CONTROLLO SU PRESENZA DI UN SINGOLO PARAMETRO FATTO IN MODO DIVERSO DA PRIMA
if test -z $1
then 	echo Errore: voglio un parametro
	exit 1
fi
#in realta' il controllo precedente non e' un controllo stretto ma lasco e quindi in questo caso lo script accetta almeno un parametro, ma non da' errore nel caso di passaggio di piu' parametri e quindi se si vuole un controllo stretto si deve usare il controllo visto prima per if5.sh
#comunque se si arriva qui vuole dire che e' stato passato almeno un parametro ($1)
#controlliamo che il primo parametro sia un file e sia leggibile!
if test ! -f $1 -o ! -r $1
then 	echo Errore: $1 non file oppure non leggibile
	exit 2
fi
#se arriviamo qui vuol dire che $1 e' un file leggibile!
#chiediamo all'utente se vuole visualizzare il contenuto del file: N.B. forziamo l'output sul terminale corrente per non avere problemi se si usasse la ridirezione dello standard output
echo "vuoi visualizzare il file $1 (si/no)?" > /dev/tty
#leggiamo la risposta dell'utente
read var1
#se l'utente ha risposto 'si' allora procediamo con visualizzazione delle info e del file
if test $var1 = si
then
       	echo info del file $1 	
	ls -la $1	#potrebbe anche essere un nome che inizia con . e quindi usiamo anche -a
	echo contenuto del file $1
	cat $1
else 	echo niente stampa di $1
fi
