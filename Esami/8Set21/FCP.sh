#!/bin/sh
#Soluzione della Prova dell'8 Settembre 2021
# ATTENZIONE IL FILE FCP.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!

case $# in
0|1|2) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 C G1 G2 ...
         exit 1;;
*)       echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo $1
case $1 in
	?) echo $1 singolo carattere ;;
	*) echo ERRORE:  primo parametro $1 NON singolo carattere
	   exit 3;;
esac
C=$1

#quindi ora possiamo usare il comando shift per eliminare il primo parametro salvato nella variabile indicata dal testo 
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
	FCR.sh $G $C /tmp/nomiAssoluti
done

#N.B. Andiamo a contare le linee del file temporaneo
#dato che il numero di directory trovate lo dobbiamo stampare e poi lo dobbiamo controllare lo salviamo in una variabile
echo Il numero di directory trovate nelle gerarchie = `wc -l < /tmp/nomiAssoluti`

#ora per ogni directory trovata dobbiamo invocare la parte C passando solo i file contenuti (e non le sotto-directory)
files= 	#varibile di appoggio che ci servira' per raccogliere i nomi dei file
for i in `cat /tmp/nomiAssoluti`
do
	cd $i #ci spostiamo nella directory corrente
	files= 		#variabile di appoggio che resettiamo per ogni directory
	for j in *
	do
		if test -f $j #se e' un file lo salviamo in una variabile di appoggio
		then
			files="$files $j"
		fi
	done
	echo CHIAMIAMO LA PARTE C con $files
	8Sett21 $files
done	

#cancelliamo il file temporaneo
rm /tmp/nomiAssoluti

