#!/bin/sh
#Soluzione dell'esame del 13 Luglio 2022

case $# in
0|1|2|3)echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 dirass L C1 C2 ...
	exit 1;;
*) 	echo DEBUG-da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo primo parametro sia una directory traversabile
case $1 in
/*) 	if test ! -d $1 -o ! -x $1
	then
		echo $1 non directory o non traversabile
		exit 2
	fi;;
*) 	echo $1 non nome assoluto
	exit 3;;
esac

G=$1 #salviamo il primo parametro (N.B. nella variabile il cui nome viene specificato nel testo)

#dobbiamo controllare che il secondo parametro sia un numero e sia strettamente maggiore di 0
case $2 in
*[!0-9]*) echo $2 non numerico o non positivo
          exit 2;;
*)        echo DEBUG-secondo parametro giusto $2;;
esac

L=$2    #salviamo il numero nella variabile indicata dal testo

#quindi ora possiamo usare il comando shift due volte
shift
shift

#ora in $* abbiamo solo i caratteri e quindi possiamo fare i controlli relativi
for i 
do
	case $i in
	?)  echo DEBUG-$i e\' un carattere ;;
	*)  echo $i non e\' un carattere 
	    exit 4;;
	esac
done

#controlli sui parametri finiti 
PATH=`pwd`:$PATH
export PATH

# azzeriamo il file temporaneo
> /tmp/nomiAssoluti     #nome indicato dal testo

#invochiamo il file comandi ricorsivo con la gerarchia, il numero, il file temporaneo e i caratteri
#NOTA BENE: i caratteri li dobbiamo passare per ultimi!!!
FCR.sh $G $L /tmp/nomiAssoluti $*

echo Il numero di file che soddisfa le specifiche e\' `wc -l < /tmp/nomiAssoluti`

for F in `cat /tmp/nomiAssoluti`
do
	echo DEBUG-stiamo per invocare la parte C con file $F lunghezza $L e caratteri $*
	13Lug22 $F $L $*
done

#cancelliamo da ultimo il file temporaneo
rm /tmp/nomiAssoluti
