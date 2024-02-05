#!/bin/sh
#Soluzione della Prova del 14 Luglio 2021

case $# in
0|1|2|3) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 H K dirass1 dirass2 ...
         exit 1;;
*)       echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
esac

#dobbiamo isolare gli ultimi due parametri e intanto facciamo i controlli
num=1   #la variabile num ci serve per capire quando abbiamo trovato il penultimo e l'ultimo parametro
params= #la variabile params ci serve per accumulare i parametri a parte gli ultimi due
#in $* abbiamo i nomi delle gerarchie e i due numeri interi
for i
do
        if test $num -lt `expr $# - 1` #ci serve per non considerare gli ultimi due parametri che sono i numeri
        then
                #soliti controlli su nome assoluto e directory traversabile
                case $i in
                /*)     if test ! -d $i -o ! -x $i
                        then
                        echo $i non directory o non attraversabile
                        exit 2
                        fi;;
                *)      echo $i non nome assoluto; exit 3;;
                esac
                params="$params $i" #se i controlli sono andati bene memorizziamo il nome nella lista params
        else
        #abbiamo individuato gli ultimi due parametri e quindi facciamo il solito controllo su numerico e strettamente positivo
                #Controllo penultimo e ultimo parametro
		case $i in
		*[!0-9]*) echo $i non numerico o non positivo
			  exit 4;;
		*) if test $i -eq 0
	   	   then echo ERRORE: parametro $i uguale a zero
		   	exit 5
	   	   fi ;;
		esac
		#se i controlli sono andati bene salviamo il penultimo e ultimo parametro
		if test $num -eq `expr $# - 1`
		then
			H=$i	#H nome indicato nel testo
		else
			M=$i	#M nome indicato nel testo
        	fi
        fi
       	num=`expr $num + 1` #incrementiamo il contatore del ciclo sui parametri
done

#controlli sui parametri finiti possiamo passare alle Q fasi, dopo aver settato il path
PATH=`pwd`:$PATH
export PATH

#ora in $params abbiamo solo i nomi delle gerarchie 
for G in $params	#G nome indicato nel testo
do
        echo fase per $G
	FCR.sh $G $H $M		#invochiamo il file comandi ricorsivo con la gerarchia e i due numeri passati come ultimi due parametri
done
