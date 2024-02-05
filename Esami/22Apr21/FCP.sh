#!/bin/sh
#Soluzione della Prova del 22 Aprile 2021
# ATTENZIONE IL FILE FCP.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!

case $# in
0|1|2|3) echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 W S dirass1 dirass2 ...
         exit 1;;
*)       echo DEBUG-OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo $1
case $1 in
	*[!0-9]*) echo $1 non numerico o non positivo
		  exit 2;;
	*) if test $1 -eq 0
	   then echo ERRORE: primo parametro $1 uguale a zero
		exit 3
	   fi ;;
esac
W=$1	#nome specificato nel testo

#Controllo $2: ha senso che controlliamo che non contenga il carattere /
case $2 in
	*/*) echo $2 non deve contenere il carattere \/
	     exit 4;;
esac
S=$2	 #nome specificato nel testo

#quindi ora possiamo usare il comando shift due volte per eliminare i primi due parametri salvati nelle variabili indicate dal testo 
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
	#invochiamo il file comandi ricorsivo con la gerarchia, il secondo parametro (cioe' la stringa) e il file temporaneo; il numero passato come primo parametro NON serve al file riorsivo!
	FCR.sh $G $S /tmp/nomiAssoluti	#N.B. in questo caso non sarebbe stato indispensabile passare il nome del file
done

#N.B. Andiamo a contare le linee del file temporaneo
#dato che il numero di directory trovate lo dobbiamo stampare e poi lo dobbiamo controllare lo salviamo in una variabile
TOT=`wc -l < /tmp/nomiAssoluti`
echo Il numero di directory trovate nelle gerarchie = $TOT 
#dobbiamo verificare se tale numero e' maggiore o uguale a W
if test $TOT -ge $W 
then
	#dobbiamo chiedere all'utente, chiamandolo con il nome di battesimo, un numero
	echo -n "Letizia dammi per favore un numero che deve essere compreso fra 1 e $W "
	#ATTENZIONE BISOGNA USARE $W per fare capire all'utente il range e non scrivere solo W
	#poiche' in questo caso si e' usata l'opzione -n bisogna inserire uno spazio bianco per fare in modo che l'echo del numero inserito NON venga attaccato al valore di $W rendendo il tutto poco leggibile
        read X #il nome della variabile da usare era specificato nel testo
	#Controllo $X: i controlli su X sono molto semplici da inserire dato che basta adattare quelli che sono stati fatti per il primo parametro; cambia solo che va verificato anche che sia minore o uguale a $W
	case $X in
	*[!0-9]*) echo $X non numerico o non positivo
		  #cancelliamo il file temporaneo
                  rm /tmp/nomiAssoluti
		  exit 7;;
	*) if test $X -lt 1 -o $X -gt $W #ATTENZIONE NELLA VERIFICA SE IL NUMERO NON E' COMPRESO BISOGNA USARE -o e NON -a!
	   then echo ERRORE: $X non e\' compreso fra 1 e $W
		#cancelliamo il file temporaneo
		rm /tmp/nomiAssoluti
		exit 8
	   fi ;;
	esac
	echo -n il nome assoluto della directory corrispondente a $X =
	#dobbiamo selezionare la linea $X del file temporaneo
	head -$X < /tmp/nomiAssoluti | tail -1 
else
	echo $TOT non maggiore o uguale a $W #ATTENZIONE: questo caso NON deve essere considerato un errore!
fi

#cancelliamo il file temporaneo
rm /tmp/nomiAssoluti
