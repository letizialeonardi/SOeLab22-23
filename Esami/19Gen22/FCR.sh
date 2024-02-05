#!/bin/sh
# Soluzione della prova d'esame del 19 Gennaio 2022
#
# File ricorsivo. Uso: $0 dirass numdisp numpari filetemp
# ATTENZIONE IL FILE FCR.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!
#file comandi ricorsivo che scrive il nome ASSOLUTO dei file trovati sul file temporaneo il cui nome e' passato come quarto argomento

trovato=false	#per verificare di avere trovato almeno un file

cd $1
for F in *
do
       	if test -f $F -a -r $F #se è un file ed e' leggibile
	then
		cont=`wc -c < $F`
		if test $cont -eq $3 -a `expr $cont % $2` -eq 0 #se la lunghezza in caratteri e' giusta e la lunghezza è multiplo intero di C
		then
			echo `pwd`/$F >> $4
			trovato=true
		fi
	fi
done

if test $trovato = true #se abbiamo trovato almeno un file dobbiamo riportare il nome della dir corrente 
then
	echo DIRECTORY TROVATA CON NOME ASSOLUTO `pwd`
fi

for i in *
do
        if test -d $i -a -x $i 
        then
                #invocazione ricorsiva
                $0 `pwd`/$i $2 $3 $4
        fi
done
