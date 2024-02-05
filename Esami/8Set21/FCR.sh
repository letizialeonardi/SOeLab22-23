#!/bin/sh
# Soluzione della prova d'esame dell'8 Settembre 2021
#
# File ricorsivo. Uso: $0 dirass car filetemp
# ATTENZIONE IL FILE FCR.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!
#file comandi ricorsivo che scrive il nome delle dir trovate sul file il cui nome e' passato come terzo argomento
# NOTA BENE: in commento e' stato inserito il comando che poteva essere usato per ricavare il nome relativo semplice, ma il cui uso, in questa soluzione, non era strettamente necessario!

#base=`basename $1` 	#variabile che ci serve per contenere solo il nome relativo semplice della dir corrente
cont=0			#variabile (il cui nome era specificato nel testo) per capire se abbiamo trovato almeno un file che soddisfa la specifica (leggibile) e quindi la directory e' giusta!

cd $1
#case $base in
case $1 in
*/$2?$2) #la dir ha il nome giusto e quindi ora verifichiamo che abbia almeno un file leggibile
	for F in *
	do
        	if test -f $F -a -r $F #se è un file ed e' leggibile
		then
			cont=`expr $cont + 1` 
		fi
	done;;
esac

if test $cont -ge 1 #se abbiamo trovato almeno un file dobbiamo salvare il nome della dir corrente nel file temporaneo
then
	echo DIRECTORY TROVATA CON NOME ASSOLUTO `pwd`
	#dobbiamo scrivere il nome della directory nel file temporaneo
	pwd >> $3  	#NON SERVE SCRIVERE echo `pwd` >> $3 dato che il comando pwd gia' da solo riporta su standard output la dir corrente!
			#in alternativa si poteva scrivere: echo $1 >> $3
fi

for i in *
do
        if test -d $i -a -x $i 
        then
                #invocazione ricorsiva
                $0 `pwd`/$i $2 $3
        fi
done
