#!/bin/sh
#file comandi append1.sh invocazione append dadove sucosa
#controllo stretto sul numero di parametri
case $# in	#controlliamo il numero di parametri
2) echo DEBUG-Numero di par ok $#;; 		#caso giusto 
*) echo uso: append dadove sucosa	#casi NON giusti
   exit 1;;
esac
#controllo su nomi relativi
#poiche' i parametri sono piu' di 1 e il controllo che dobbiamo fare su entrambi e' lo stesso, possiamo usare un for i che implicitamente vuole dire for i in $#
for i 
do
	case $i in
   	*/*) echo $i non nome relativo semplice; exit 2 ;;	#casi NON giusti
   	*) echo DEBUG-OK nome $i ;;					#caso giusto
   	esac
done
#controllo su esistenza file $1 e sua leggibilita'
if test ! -f $1 -o ! -r $1
then
	echo $1 non esiste o non leggibile; exit 3
fi
#se arriviamo qui vuole dire che il file $1 esiste ed e' leggibile

#controllo esistenza file $2
if test ! -f $2
then
	#il file non esiste e quindi dobbiamo verificare di avere il diritto di scrittura sulla dir corrente e nel caso settiamo scrivibile a true
        if test ! -w .            #controllo sulla scrivibilita' della directory corrente
	then
		echo directory corrente non scrivibile; exit 4	#caso non giusto
	fi
else	#file esiste e controllo se scrivibile
	if test ! -w $2
	then 	echo file $2 non scrivibile; exit 5	#caso NON giusto
	fi
fi
#se arriviamo qui vuole dire o che il file $2 esiste ed e' scrivibile o che non esiste ma la directory corrente e' scrivibile
cat < $1 >> $2
