#!/bin/sh
#file comandi file-dir.sh

cd $1 	#ci spostiamo nella directory il cui nome e' passato come parametro

F=0 	#variabile per contare i file
D=0  	#variabile per contare le directory

for i in * 	#per tutti i file della directory corrente
do
	if test -f $i	#se e' un file
  	then
    		echo [F] `pwd`/$i	#stampiamo il nome premettendo un [F}
    		F=`expr $F + 1`	#incrementiamo il numero di file
  	else
		if test -d $i
		then 
    			echo [D] `pwd`/$i	#se e' una dir, stampiamo il nome premettendo un [D}
    			D=`expr $D + 1`	#incrementiamo il numero di directory
		else #controllo aggiunto per sicurezza, ma difficile che si trovi qualcosa che non e' file o directory
			echo Elemento corrente non file o directory
		fi	
  	fi
done

if test $F -eq 0 -a $D -eq 0 	#per sicurezza controlliamo di non essere in una directory vuota
then
    echo "ATTENZIONE: la directory $1 e' vuota"
    exit 3
fi
#stampiamo i due conteggi
echo Numero di file = $F
echo Numero di directory = $D
