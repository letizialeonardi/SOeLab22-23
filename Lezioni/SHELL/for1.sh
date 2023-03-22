#!/bin/sh
#file comandi con primo esempio di for
for i in *	#per tutti i file/directory della directory corrente (ATTENZIONE NON QUELLI IL CUI NOME INIZIA PER .)
do
	echo ELEMENTO CORRENTE $i	#visualizziamo il nome del file/directory
done
