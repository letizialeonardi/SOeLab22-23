#!/bin/sh
#FCR.sh 
#file comandi ricorsivo 
trovato=	#variabile che ci serve per capire se ci sono tutti i caratteri nel file 
nl=		#variabile che ci serve per calcolare il numero di linee del file corrente

cd $1 		#ci posizioniamo nella directory giusta
L=$2		#variabile in cui salviamo il numero di linee dei file da cercare
TMP=$3		#variabile in cui salviamo il nome del file temporaneo
#ora dobbiamo di nuovo usare shift tre volte per eliminare il nome della gerarchia, la lunghezza e il nome del temporaneo e quindi avere solo i caratteri; NON serve salvare il primo parametro!
shift
shift
shift

for F in *
do
	if test -f $F #solo file 
	then
		#ora dobbiamo fare un for per tutti i caratteri perche' ci deve essere una occorrenza di tutti i caratteri perche' un file sia quello giusto!
		trovato=true	#settiamo la variabile a true; N.B. VA FATTO OGNI VOLTA! 
		for i	#per tutti i caratteri
		do
			if ! grep $i $F > /dev/null 2>&1 #se il carattere corrente non e' presente, mettiamo trovato a false
			then
				trovato=false
			fi
		done
		#se trovato e' rimasto a true vuole dire che il file e' giusto e quindi stampiamo il nome assoluto (insieme con la sua lunghezza in linee anche se non era richiesto) e salviamo nel file temporaneo (in append) 
		if test $trovato = true
		then
			nl=`wc -l < $F`
			if test $nl -eq $L
			then
				echo Trovato un file che soddisfa le specifiche e il suo nome assoluto e\' `pwd`/$F e la sua lunghezza in linee e\' $L
				echo `pwd`/$F >> $TMP 
			fi
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio corrente
		$0 `pwd`/$i $L $TMP $*	#ATTENZIONE AI PARAMETRI! 
	fi
done
