#!/bin/sh
#Soluzione della Prova del 16 Giugno 2021

case $# in
0|1|2|3) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 H M dirass1 dirass2 ...
         exit 1;;
*)       echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo $1
case $1 in
	*[!0-9]*) echo $1 non numerico o non positivo
		  exit 2;;
	*) if test $1 -eq 0
	   then echo ERRORE: secondo parametro $1 uguale a zero
		exit 3
	   fi ;;
esac
H=$1 #NOME STABILITO DAL TESTO

#Controllo $2
case $2 in
	*[!0-9]*) echo $2 non numerico o non positivo
		  exit 4;;
	*) if test $2 -eq 0
	   then echo ERRORE: terzo parametro $2 uguale a zero
		exit 5
	   fi ;;
esac
M=$2 #NOME STABILITO DAL TESTO

#quindi ora possiamo usare il comando shift due volte per eliminare i due numeri
shift
shift

#ora in $* abbiamo solo i nomi delle gerarchie e quindi possiamo fare i controlli sui nomi assoluti e sulle directory in un for
for G 
do
        case $G in
        /*) if test ! -d $G -o ! -x $G
            then
            echo $G non directory
            exit 6
            fi;;
        *)  echo $G non nome assoluto; exit 7;;
        esac
done
#controlli sui parametri finiti possiamo passare alle Q fasi, dopo aver settato il path
PATH=`pwd`:$PATH
export PATH

for G #NOME STABILITO DAL TESTO
do
        echo fase per $G
	FCR.sh $G $H $M		#invochiamo il file comandi ricorsivo con la gerarchia e i due numeri passati come primi due parametri
done
