#!/bin/sh
#file comandi ultimo-par.sh
count=0 #variabile per contare i parametri via via nel for
LIST= 	#variabile per contenere la lista dei parametri senza l'ultimo
LAST=   #variabile per memorizzare l'ultimo parametro 

if test $# -eq 0 #controlliamo il numero di parametri e se e' uguale a 0 diamo un errore!
then
  	echo Errore: ci vogliono dei parametri
  	exit 1
fi

for i	#eseguiamo per tutti i parametri di invocazione  
do
	count=`expr $count + 1` 	#incrementiamo il contatore ad ogni iterazione
	if test $count -ne $# 		#se il contatore e' diverso dal numero dei parametri, vuole dire che non siamo ancora sull'ultimo parametro
  	then
    		LIST="$LIST $i"		#quindi memorizzamo il parametro corrente nella lista
  	else
    		LAST=$i			#altrimenti abbiamo isolato l'ultimo parametro
  fi
done

#stampiamo l'ultimo parametro e la lista degli altri parametri (escluso l'ultimo)
echo Ultimo parametro = $LAST
echo Lista parametri escluso ultimo = $LIST
