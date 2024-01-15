#!/bin/sh
#Soluzione della Prima Prova in itinere del 13 Aprile 2022
#versione che usa diversi file temporanei e usa FCR.sh per tutte le fasi

#controllo sul numero di parametri: deve essere maggiore o uguale a 3 
case $# in
0|1|2)	echo Errore: numero parametri $# quindi pochi parametri. Usage is $0 numero dirass1 dirass2 ...
	exit 1;;
*) 	echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#dobbiamo controllare che il primo parametro sia un numero e sia strettamente maggiore di 0
case $1 in
*[!0-9]*) echo $1 non numerico o non positivo
	  exit 2;;
*) 	  echo primo parametro giusto $1;;
esac

X=$1	#salviamo il numero nella variabile indicata dal testo e poi facciamo shift
shift

#ora in $* abbiamo solo i nomi delle gerarchie e quindi possiamo fare i controlli sui nomi assoluti e sulle directory in un for
for G 
do
	case $G in
	/*) if test ! -d $G -o ! -x $G
	    then
	    echo $G non directory o non attraversabile
	    exit 3
	    fi;;
	*)  echo $G non nome assoluto; exit 4;;
	esac
done
#controlli sui parametri finiti possiamo passare alle Q fasi dopo aver settato ed esportato il path
PATH=`pwd`:$PATH
export PATH

n=1 #serve per dare un nome diverso ai file temporanei e, volendo, per dire in che fase siamo
for G
do
	> /tmp/nomiAssoluti$$-$n 	#creiamo/azzeriamo il file temporaneo: N.B. l'indicazione $$ si poteva anche omettere, ma in questo caso era coerente con la specifica che indicava che la PARTE INIZIALE del nome dei vari file temporanei!
	echo DEBUG-fase $n per $G
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero X e il file temporaneo
	FCR.sh $G $X /tmp/nomiAssoluti$$-$n   
	#la specifica indicava che doveva essere riportato su standard output il conteggio di OGNI SINGOLA gerarchia!
	echo Il numero di file trovati in questa fase e\' `wc -l < /tmp/nomiAssoluti$$-$n`
	n=`expr $n + 1`
done

#terminate tutte le ricerche ricorsive cioe' le Q fasi
#dobbiamo procedere con il confronto richiesto
for file1 in `cat  /tmp/nomiAssoluti$$-1`	#per ogni file trovato nella prima gerarchia
do
	n=1 	#idem come prima: ATTENZIONE QUESTA INIZIALIZZAZIONE E' DA FARE ALL'INTERNO DI QUESTO CICLO!
	for i	#per ognuna delle gerarchie
	do
		if test $n -gt 1	#saltiamo il file temporaneo della prima gerarchia; N.B. in alternativa prima di iniziare questa parte di confronto si poteva usare il comando shift e quindi avere in $* solo le gerarchie a partire dalla G2
		then	
			for file2 in `cat  /tmp/nomiAssoluti$$-$n`
			do
				#Stampa di DEBUG
				echo DEBUG-STIAMO PER CONTROLLARE il file $file1 e il file $file2
				if diff $file1 $file2 > /dev/null 2>&1	#basta usare il comando diff che riporta 0 se i file sono uguali, altrimenti un numero diverso da 0: da notare sia la ridirezione dello standard output che dello standard error su /dev/null
				then 	echo I file $file1 e $file2 sono uguali
				#else	echo I file $file1 e $file2 sono diversi 	#a seconda dei testi doveva essere riportato che i file fossero uguali o diversi e nel secondo caso era da fare questo else andando a non fare nulla nello then o mettendo una condizione inversa
				fi
			done 
		fi
		n=`expr $n + 1`
	done
done	

#ci vuole la fase di cancellazione di tutti i file temporanei
n=1 #idem come prima
for G
do
	#eliminiamo il file temporaneo n-esimo
	rm /tmp/nomiAssoluti$$-$n
	n=`expr $n + 1`
done
