#!/bin/sh
#file comandi Cercadir.sh
#ricerca in breadth-first
cd $1	#ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

#OSSERVAZIONE IMPORTANTE: SE AVESSIMO FATTO UN if COME NELLA VERSIONE CHE CERCAVA I FILE INSERENDO CHIARAMENTE test -d AVREMMO IL PROBLEMA CHE NON SI TERREBBE IN CONTO CHE IL NOME PASSATO POSSA ESSERE QUELLO DELLA DIR DI PARTENZA! QUINDI E' OPPORTUNO CHE FACCIAMO QUESTO CONTROLLO CON UN CASE!
case $1 in
*/$2) 	#in questo modo controlliamo anche il nome della gerarchia di partenza!
	echo la directory $2 si trova in `pwd` ;;	#trovata directory
esac

#passiamo ora alla parte ricorsiva
for i in *
do
if test -d $i -a -x $i 
then
	#stampa di debugging
	echo Stiamo per esplorare la directory `pwd`/$i
	#invocazione ricorsiva: ATTENZIONE NOME ASSOLUTO dir!!!
	$0 `pwd`/$i $2	#possiamo usare $0
fi
done

