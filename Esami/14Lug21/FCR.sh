#!/bin/sh
#FCR.sh 
#file comandi ricorsivo della prova del 14 Luglio 2021

cd $1
#la variabile cont ci serve per contare il numero di file, nome indicato nel testo
cont=0
#la variabile files ci serve per memorizzare i file 
files=

for F in *	#F nome indicato nel testo
do
	#controlliamo solo i nomi dei file 
	if test -f $F 
	then 
		#controlliamo la lunghezza del nome del file
		case $F in
		??) 	#caso giusto 
			#calcoliamo la lunghezza in linee del file per vedere se soddisfa le specifiche
			L=`wc -l < $F`
			if test $L -eq $3
			then
				#aggiorniamo la variabile del conteggio	
				cont=`expr $cont + 1`
				#salviamo il nome del file nella variabile apposita
				files="$files $F"		
			fi
			;;
		*) #non si deve fare nulla ;;
		esac
	fi
done

if test $cont -lt $2 -a $cont -ge 2 
then
	echo TROVATO direttorio  `pwd`
	#dobbiamo invocare la parte C passando i nomi dei file precedentemente salvati insieme con il valore M (ultimo parametro parte principale)
	echo invochiamo 14Lug21 $files $3
	14Lug21 $files $3
fi

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo il nome assoluto della dir
		$0 `pwd`/$i $2 $3
	fi
done
