#!/bin/sh
# Soluzione della prova d'esame del 22 Aprile 2021
#
# File ricorsivo. Uso: $0 dirass stringa filetemp
# ATTENZIONE IL FILE FCR.sh DEVE ESSERE UN FILE IN CUI IL DIRITTO DI ESECUZIONE ALMENO PER L'UTENTE RISULTA SETTATO!
#file comandi ricorsivo che scrive il nome delle dir trovate sul file
#il cui nome e' passato come terzo argomento

trovato=false #variabile che ci servira' per capire se abbiamo trovato almeno un file che soddisfa la specifica (termina per .S) e quindi la directory e' giusta!

cd $1

for F in *
do
        if test -f $F #se è un file
	then
		#controlliamo se termina per la stringa cercata: ATTENZIONE USARE UN if test e' SBAGLIATO, dato che puo' funzionare se e solo se in ogni dir c'e' sempre e solo un file che rispetta la specifica!
		case $F in
		*.$2)  	#se il nome rispetta la specifica, settiamo trovato a true
			trovato=true;; #dato che il testo richiedeva almeno uno potremmo anche inserire un break!
		*) ;; 	  #altrimenti non facciamo nulla. N.B. Questa alternativa puo' anche essere omessa!
		esac
	fi
done

if test $trovato = true #se abbiamo trovato almeno un file dobbiamo salvare il nome della dir corrente nel file temporaneo
then
	#dobbiamo scrivere il nome della directory nel file temporaneo
	pwd >> $3  	#NON SERVE SCRIVERE echo `pwd` >> $3 dato che il comando pwd gia' da solo riporta su standard output la dir corrente!
			#in alternativa si poteva scrivere: echo $1 >> $3
fi

for i in *
do
        if test -d $i -a -x $i 
        then
                #invocazione ricorsiva passando sempre il nome assoluto della dir 
                $0 `pwd`/$i $2 $3
        fi
done
