#!/bin/sh
#FCR.sh
#file comandi ricorsivo che scrive sul file temporaneo il valore del conteggio

cd $1	#ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

read conta < /tmp/contatmp	#leggiamo il conteggio attuale in una variabile di shell

for i in *	#per tutti i file/dir
do
	if test -f $i	#se e' un file, dato che dobbiamo SOLO contare i file!
	then 	
		conta=`expr $conta + 1`	#incrementiamo la variabile di shell
	fi
done
#inseriamo una stampa di puro debug: UNA VOLTA VERIFICATO IL FUNZIONAMENTO POTREMO COMMENTARLA!
echo DEBUG-in `pwd` conta = $conta
echo $conta > /tmp/contatmp	#aggiorniamo il conteggio memorizzato nel file temporaneo

#passiamo ora alla parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i			#ATTENZIONE: il nome della dir va sempre passato in forma assoluta!
		read conta < /tmp/contatmp	#leggiamo il valore del conteggio per la sotto-gerarchia appena esplorata
	fi
done
