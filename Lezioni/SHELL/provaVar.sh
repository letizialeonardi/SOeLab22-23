#!/bin/sh
#per definire un commento si deve usare il carattere # 
#il commento termina alla fine della linea
i=12 				#si definisce la variabile i e le si assegna un valore
#ATTENZIONE NESSUNO SPAZIO PRIMA E DOPO =
echo Variabile i vale $i 	#i vale la stringa 12
j=$i+1 				#si definisce la variabile j come concatenazione della stringa
				#contenuta nella variabile i e della stringa 1
#ATTENZIONE DI NUOVO NESSUNO SPAZIO NEANCHE PRIMA E DOPO IL +
echo Variabile j vale $j
#se si vuole ottenere l'operazione + bisogna usare il comando expr, usando la valutazione di comandi (backquote!)
j=`expr $i + 1`			#si chiede di assegnare alla variabile j la somma del contenuto di $i considerato come numero e 1!
#NOTA BENE: nel comando expr sono vitali gli SPAZI!
echo Variabile j vale $j
#ATTENZIONE: questi comandi si possono usare anche interattivamente
#NON solo in un file comandi
