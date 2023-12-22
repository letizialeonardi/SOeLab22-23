#!/bin/sh
#FCR.sh 
#file comandi ricorsivo 
cont1=0	#variabile che ci serve per capire quanti file con estensione $2 ci sono (N.B. variabile il cui nome viene specificato nel testo)
files1=	#variabile per raccogliere i file con estensione $2 (cioe' E1 del testo)
cont2=0	#variabile che ci serve per capire quanti file con estensione $3 ci sono (N.B. variabile il cui nome viene specificato nel testo)
files2=	#variabile per raccogliere i file con estensione $3 (cioe' E2 del testo)

cd $1 	#ci posizioniamo nella directory giusta

for F in *	#(N.B. F variabile il cui nome viene specificato nel testo)
do	
	if test -f $F -a -r $F #controlliamo prima se il nome e' quello di un file leggibile 
	then
		case $F in
		*.$2) 	#se ha estensione $2 (cioe' E1 del testo) 
			cont1=`expr $cont1 + 1`
			files1="$files1 $F"
			;;
		*) ;;
		esac
		if test -w $F	#controlliamo se e' anche scrivibile
		then
			case $F in
			*.$3) 	#se ha estensione $3 (cioe' E2 del testo)
				cont2=`expr $cont2 + 1`
				files2="$files2 $F"
				;;
			*) ;;
			esac
		fi
	fi
done

#ora controlliamo se il valore di cont1 e cont2 sono diversi da 0 abbiamo trovato una dir giusta
if test $cont1 -ne 0 -a $cont2 -ne 0 
then
	echo Trovata una directory che soddisfa le specifiche e il suo nome assoluto e\' `pwd` 
	for f2 in $files2	#per ogni file leggibile e scrivibile con estensione E2
	do
		echo chiamiamo la parte in C con $files1 e $f2
		25Gen23 $files1 $f2
	done
fi 

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio 
		$0 `pwd`/$i $2 $3 
	fi
done
