#!/bin/sh
#Soluzione della Prova del 7 Giugno 2023
# ATTENZIONE IL FILE FCP.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!

case $# in
0|1|2) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 X G1 G2 ...
       exit 1;;
*)     echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo $1
#dobbiamo controllare che il primo parametro sia un numero, sia strettamente maggiore di 0 e strettamente minore di 4
case $1 in
*[!0-9]*) echo $1 non numerico o non positivo
          exit 2;;
*)        if test $1 -eq 0 -o $1 -ge 4   #sbagliato
          then
                  echo $1 uguale a 0 oppure maggiore o uguale a 4
                  exit 3
          else
                  echo DEBUG-primo parametro giusto $1
          fi;;
esac

X=$1    #salviamo il numero nella variabile indicata dal testo 

#quindi ora possiamo usare il comando shift per eliminare il numero 
shift

#ora in $* abbiamo solo i nomi delle gerarchie e quindi possiamo fare i controlli sui nomi assoluti e sulle directory in un for
for G
do
        case $G in
        /*) if test ! -d $G -o ! -x $G
            then
            echo $G non directory
            exit 4
            fi;;
        *)  echo $G non nome assoluto; exit 5;;
        esac
done
#controlli sui parametri finiti possiamo passare alle Q fasi, dopo aver settato il path
PATH=`pwd`:$PATH
export PATH

for G 	#NOME STABILITO DAL TESTO
do
        echo DEBUG-fase per $G	
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero X e il livello corrente che inizialmente lo poniamo a 0
        FCR.sh $G $X 0
        #il primo livello (di ogni gerarchia G) verra' contato come livello 1: se invece avessimo voluto contarlo come livello 0 allora il valore iniziale del terzo parametro doveva essere -1
done
