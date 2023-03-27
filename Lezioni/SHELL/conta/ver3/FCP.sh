#!/bin/sh
#FCP.sh dirass 
#versione che usa una variabile di shell locale e il valore di ritorno di un file comandi

#la parte dei controlli e' uguale in tutte le versioni!
case $# in	#controlliamo di avere esattamente un parametro 
1)	echo DEBUG-OK numero di parametri $#;;
*) 	echo Errore: Usage is $0 dirass 
	exit 1;;
esac

#se arriviamo qui vuol dire che e' stato passato un solo parametro
#ora controlliamo che sia il nome assoluto di una gerarchia e quindi directory traversabile
case $1 in
/*) if test ! -d $1 -o ! -x $1	#se nome assoluto, controlliamo sia una dir traversabile
	    then
	    echo $1 non directory o non traversabile
	    exit 2
	    fi;;
*)  echo $1 non nome assoluto; exit 3;;
esac

#se arriviamo qui vuol dire che sono finiti i controlli sui parametri ed e' andato tutto bene
#quindi possiamo passare a settare la var. di ambiente PATH (con export)
PATH=`pwd`:$PATH
export PATH

#invocazione del secondo file comandi
FCR.sh $*   #$* ===> $1

echo Il numero di file totale della gerarchia = $?
#OSSERVAZIONE: NON SI HA ALCUNA INFORMAZIONE SU QUALI SIANO I NOMI DEI FILE TROVATI!
