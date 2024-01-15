#!/bin/sh
#FCR.sh 
#file comandi ricorsivo che scrive il nome dei file trovati sul file temporaneo
#il cui nome e' passato come terzo argomento

cd $1
#la variabile NR ci serve per il numero di linee
NR=

for F in *
do
	#controlliamo solo i nomi dei file leggibili come indicato nella specifica!
	if test -f $F -a -r $F
	then 	
		NR=`wc -l < $F`
		if test $NR -eq $2
		then
			#abbiamo trovato un file che soddisfa le specifiche e quindi inseriamo il suo nome assoluto nel file temporaneo
			echo `pwd`/$F >> $3
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva: ATTENZIONE A PASSARE IL NOME ASSOLUTO DELLA dir INDIVIDUATA!
		FCR.sh `pwd`/$i $2 $3
	fi
done

