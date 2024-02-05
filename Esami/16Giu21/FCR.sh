#!/bin/sh
#FCR.sh 
#file comandi ricorsivo della prova del 16 Giugno 2021

cd $1
#la variabile cont ci serve per contare il numero di file 
cont=0 #NOME STABILITO DAL TESTO
#la variabile files ci serve per memorizzare i file trovati
files=

for F in * #NOME STABILITO DAL TESTO
do
	#controlliamo solo i nomi dei file 
	if test -f $F 
	then 
		#controlliamo la lunghezza del nome del file
		case $F in
		??) 	#caso giusto 
			#calcoliamo la lunghezza in linee del file per vedere se soddisfa le specifiche
			L=`wc -l < $F`
			if test $L -eq $2
			then
				#aggiorniamo la variabile del conteggio	
				cont=`expr $cont + 1`
				#salviamo i nome del file nella variabile apposita
				files="$files $F"		
			fi
			;;
		*) #non si deve fare nulla ;;
		esac
	fi
done

if test $cont -lt $3 -a $cont -ge 2 
then
	echo TROVATO direttorio  `pwd`
	#dobbiamo invocare la parte C passando i nomi dei file precedentemente salvati
	echo invochiamo 16Giu21 $files
	16Giu21 $files
fi

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva
		$0 `pwd`/$i $2 $3
	fi
done
