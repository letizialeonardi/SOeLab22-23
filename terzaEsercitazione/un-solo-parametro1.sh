#!/bin/sh
#file comandi un-solo-parametro1.sh: invocazione un-solo-parametro1.sh stringa
#versione con controllo numero parametri fatto con un case

case $# in
1) echo Un solo parametro $1;;			#unico caso corretto
*) echo $# non numero giusto di parametri! 	#casi NON corretti!
   exit 1;;
esac

#il controllo se nome assoluto, relativo o relativo semplice VA FATTO OBBLIGATORIAMENTE con un case!
case $1 in
  /*) echo "$1 e' un nome assoluto" ;;		#solo se inizia con il carattere / e' un nome assoluto
  */*) echo "$1 e' un nome relativo" ;;		#se contiene il carattere / allora e' un nome relativo
  *) echo "$1 e' un nome relativo semplice" ;; 	#dato che non corrisponde ai casi precedenti e' sicuramente un nome relativo semplice!
esac
#NOTA BENE: abbiamo controllato solo che tipo di nome sia e non che sia il nome di un file o di una directory!
