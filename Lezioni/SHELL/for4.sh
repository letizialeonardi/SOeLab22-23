#!/bin/sh
#file comandi con altro esempio di for in cui la lista specificata deriva dal comando cat su un file!
#supponiamo che nel file ttt siano stati memorizzati i nomi di alcuni file o directory
for i in `cat ttt` 	#la lista usata nel for corrisponde al contenuto di un file!
do
	echo ELEMENTO CORRENTE: $i
	if test -f $i	#se e' un file
	then 	cat $i	#mostriamo il contenuto
	fi
done
