#!/bin/sh
#file comandi ricorsivo dirass Char filetemp

cd $1

#per prima cosa azzeriamo il conteggio dei file che soddisfano la specifica 
contafile=0

for F in * 	#il nome della variabile da usare nel for era indicato nel testo!
do
	if test -f $F -a -r $F
	then
	  	#se è un file leggibile controlliamo se contiene il carattere che dobbiamo cercare
		if grep $2 $F > /dev/null 2>&1		#usiamo ridirezione dello standard output e dello standard error per non avere stampe fastidiose
		then
			echo `pwd`/$F >> $3 		#salviamo il nome assoluto del file nel file temporaneo!
			contafile=`expr $contafile + 1`	#incrementiamo il numero dei file trovati: poteva, in questo caso, andare bene anche una variabile usata come un booleano!
		fi
	fi	
done

#se ho trovato almeno un file 
if test $contafile -ge 1
then
  echo TROVATO DIRETTORIO `pwd`
fi

for i in *
do
if test -d $i -a -x $i
then
  #echo RICORSIONE in `pwd`/$i 
  $0 `pwd`/$i $2 $3
fi
done
