#!/bin/sh
#file comandi Cercadir.sh
#ricerca in breadth-first
#SOLUZIONE CON IL COMANDO basename (CHE PERO' IN QUESTA SOLUZIONE NON SAREBBE NECESSARIO USARE!)
dir= 		#variabile per salvare il risultato del comando basename
cd $1

dir=`basename $1`	#basename ricava solo il nome relativo semplice di un file/dir
#basename e' un comando (verificarne il funzionamento con il man) non visto a lezione/esercitazione, che in questo caso non era indispensabile usare (si veda infatti soluzione alternativa), ma invece nella soluzione dell'esame del 20 Gennaio 2021 era indispensabile!

case $dir in
$2) 	#in questo modo controlliamo anche il nome della gerarchia di partenza!
	echo la directory $2 si trova in `pwd` ;;	#trovato directory
esac

#passiamo ora alla parte ricorsiva
for i in *
do
if test -d $i -a -x $i 
then
	#stampa di debugging
	echo Stiamo per esplorare la directory `pwd`/$i
	#invocazione ricorsiva: ATTENZIONE NOME ASSOLUTO dir!!!
	$0 `pwd`/$i $2	#possiamo usare $
fi
done
