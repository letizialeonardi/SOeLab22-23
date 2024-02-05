#!/bin/sh
#Soluzione della Prova del 16 Febbraio 2022
# ATTENZIONE IL FILE FCP.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!

case $# in
0|1|2) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 C G1 G2 ...
       exit 1;;
*)     echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo $1
case $1 in
	?) echo $1 singolo carattere ora controlliamo sia un alfabetico minuscolo
	case $1 in
		[!a-z]) echo $1 non alfabetico minuscolo
		  	exit 2;;
		*) echo $1 carattere alfabetico minuscolo;;
	esac;;
	*)	echo $1 sbagliato dato che non singolo carattere
		exit 3;;
esac
C=$1 #NOME STABILITO DAL TESTO

#quindi ora possiamo usare il comando shift per eliminare il carattere
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

#azzeriamo il file temporaneo: usiamo un solo file temporaneo: il nome del file temporaneo era specificato nel testo e andava usato cosi' come indicato SENZA AGGIUNGERE $$
> /tmp/nomiAssoluti

for G 	#NOME STABILITO DAL TESTO
do
        echo fase per $G	
	#invochiamo il file comandi ricorsivo con la gerarchia, il carattere e il file temporaneo
	FCR.sh $G $C /tmp/nomiAssoluti
done

#N.B. Andiamo a contare le linee del file temporaneo e le salviamo nella variabile N (NOME STABILITO DAL TESTO)
N=`wc -l < /tmp/nomiAssoluti`
#stampa non richiesta 
echo DEBUG-Il numero di file trovati nelle gerarchie = $N

#se sono almeno due i file trovati invochiamo la parte C
if test $X -ge 2
then
	echo Sto per invocare 16Feb22 `cat /tmp/nomiAssoluti` $C
	16Feb22 `cat /tmp/nomiAssoluti` $C 	#N.B. Assolutamente non necessario inserire i nomi dei file trovati in una variabile!
else    echo NESSUNA INVOCAZIONE DELLA PARTE C DATO CHE $X minore di 2!
fi

#cancelliamo il file temporaneo
rm /tmp/nomiAssoluti

