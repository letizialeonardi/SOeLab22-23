#!/bin/sh
#file comandi con primo esempio di for con aggiunta conteggio
cont=0 	#variabile che serve per contare gli elementi trovati

for i in *	#come per for1.sh
do
	cont=`expr $cont + 1`	#ad ogni iterazione incrementiamo questo contatore
	echo DEBUG-sono all\'iterazione numero $cont	#stampiamo il valore del contatore; ATTENZIONE: se vogliamo usare l'apostrofo e non vogliamo usare le doppie virgolette dobbiamo usare il carattere di escape
	echo "DEBUG-IN ALTERNATIVA: sono all'iterazione numero $cont"	#stampa alternativa per verifica
	echo ELEMENTO CORRENTE $i			#come per for1.sh
done
#stampiamo il conteggio finale
echo Il numero totale dei file o directory = $cont
