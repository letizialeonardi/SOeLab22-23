#!/bin/sh
#file comandi Beginc.sh
d=	#la variabile d serve per memorizzare o il primo parametro oppure la directory corrente
f=	#la variabile f serve per memorizzare il parametro che rappresenta il nome relativo semplice del file (primo o secondo parametro a seconda dei casi)
case $# in
0) echo "POCHI PARAMETRI - Usage is: $0 [directory] file" 
   exit 1;;
1) d=`pwd`; f=$1;;
2) d=$1; f=$2;;
*) echo "TROPPI PARAMETRI - Usage is: $0 [directory] file"
   exit 2;;
esac
#controllo che d sia dato in forma assoluta
case $d in
/*) ;;
*) echo Errore: $d non in forma assoluta
   exit 3;;
esac
#controllo che f sia dato in forma relativa semplice 
case $f in
*/*) echo Errore: $f non in forma relativa semplice 
     exit 4;;
*) ;;
esac
#controllo che d sia una directory e che sia traversabile 
if test ! -d $d -o ! -x $d
then echo Errore: $d non directory o non attraversabile; exit 5
fi

PATH=`pwd`:$PATH
export PATH
echo Beginc.sh: Stiamo per esplorare la directory $d
Cercafile.sh $d $f	#invocazione del secondo file comandi (che e' quello RICORSIVO e fa in questo caso tutto il 'lavoro'!)
echo HO FINITO TUTTO
