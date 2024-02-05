#!/bin/sh
#Soluzione della Prova del 19 Gennaio 2022
# ATTENZIONE IL FILE FCP.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!

case $# in
0|1|2|3) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 C X G1 G2 ...
         exit 1;;
*)       echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo $1
case $1 in
	*[!0-9]*) echo $1 non numerico o non positivo
		  exit 2;;
	*) if test $1 -eq 0
	   then echo ERRORE: primo parametro $1 uguale a zero
		exit 3
	   else 
	   	if test `expr $1 % 2` -eq 0
	   	then echo ERRORE: primo parametro $1 NON dispari
		     exit 4
		fi
	   fi ;;
esac
C=$1 #NOME STABILITO DAL TESTO

#Controllo $2
case $2 in
	*[!0-9]*) echo $2 non numerico o non positivo
		  exit 5;;
	*) if test $2 -eq 0
	   then echo ERRORE: secondo parametro $2 uguale a zero
		exit 6
	   else 
	   	if test `expr $2 % 2` -ne 0
	   	then echo ERRORE: secondo parametro $2 NON pari
		     exit 4
		fi
	   fi ;;
esac
X=$2 #NOME STABILITO DAL TESTO

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
            exit 5
            fi;;
        *)  echo $G non nome assoluto; exit 6;;
        esac
done
#controlli sui parametri finiti possiamo passare alle Q fasi, dopo aver settato il path
PATH=`pwd`:$PATH
export PATH

#azzeriamo il file temporaneo: usiamo un solo file temporaneo: il nome del file temporaneo era specificato nel testo e andava usato cosi' come indicato SENZA AGGIUNGERE $$
> /tmp/nomiAssoluti

for G
do
        echo fase per $G	
	#invochiamo il file comandi ricorsivo con la gerarchia, il primo parametro (cioe' il carattere) e il file temporaneo
	FCR.sh $G $C $X /tmp/nomiAssoluti
done

#N.B. Andiamo a contare le linee del file temporaneo
#dato che il numero di file trovati lo dobbiamo stampare 
echo Il numero di file trovati nelle gerarchie = `wc -l < /tmp/nomiAssoluti`

#ora dobbiamo invocare la parte C passando C e i file trovati
echo Sto per invocare 19Gen22 `cat /tmp/nomiAssoluti` $C
19Gen22 `cat /tmp/nomiAssoluti` $C

#cancelliamo il file temporaneo
rm /tmp/nomiAssoluti

