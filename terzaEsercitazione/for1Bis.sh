#!/bin/sh
#file comandi for1Bis.sh
for i in .*	#il solo * non riesce a fare match con i nomi che iniziano per . e quindi se li vogliamo considerare si deve usare il pattern .*
do
	echo ELEMENTO CORRENTE $i
done

