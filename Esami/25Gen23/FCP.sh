#!/bin/sh
#Soluzione dell'esame del 25 Gennaio 2023

case $# in
0|1|2) 	echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 dirass E1 E2
	exit 1;;
3) 	echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
*) 	echo Errore: numero parametri is $# quindi troppi parametri. Usage is $0 dirass E1 E2
	exit 1;;
esac

#Controllo primo parametro sia una directory
case $1 in
/*) 	if test ! -d $1 -o ! -x $1
	then
		echo $1 non directory
		exit 2
	fi;;
*) 	echo $1 non nome assoluto
	exit 3;;
esac

G=$1 #salviamo il primo parametro (N.B. nella variabile il cui nome viene specificato nel testo)

#quindi ora possiamo usare il comando shift
shift

#ora in $* abbiamo solo le due stringhe e quindi possiamo fare i controlli adeguati
for i 
do
	case $i in
	*/*) echo in una striga che sara\' usata come estensione non ci puo\' essere uno slash 
	     exit 4;;
	*)   #echo OK $i stringa corretta  
		;;
	esac
done

#solo per scrupolo controlliamo che le due stringhe passate NON siano uguali
if test $1 = $2
then
        echo le due stringhe $1 e $2 sono uguali e questo non e\' corretto
        exit 5
fi

#controlli sui parametri finiti 
PATH=`pwd`:$PATH
export PATH

#invochiamo il file comandi ricorsivo con la gerarchia e le due stringhe 
FCR.sh $G $*	#in alternativa invece che $* si poteva scrivere $1 e $2
