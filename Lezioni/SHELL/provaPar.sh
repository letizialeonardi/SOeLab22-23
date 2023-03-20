#!/bin/sh
#Poco significativo se non si passano parametri (meglio 4)
echo I PARAMETRI SONO
echo 'Il valore di $1 ===>' $1
echo 'Il valore di $2 ===>' $2
echo 'Il valore di $3 ===>' $3
echo 'Il valore di $4 ===>' $4
echo 'Il valore di $* (TUTTI I PARAMETRI, a parte $0) ===>' $*
echo 'Il valore di $# (NUMERO DEI PARAMETRI) ===>' $#
shift
echo DOPO LO SHIFT, SONO
echo 'Il valore di $1 ===>' $1
echo 'Il valore di $2 ===>' $2
echo 'Il valore di $3 ===>' $3
echo 'Il valore di $* (TUTTI I PARAMETRI, a parte $0) ===>' $*
echo 'Il valore di $# (NUMERO DEI PARAMETRI) ===>' $#
ls DIR.sh
echo 'Il valore di $? (VALORE DI RITORNO ULTIMO COMANDO) ===>' $?
ls pippo 
echo 'Il valore di $? (VALORE DI RITORNO ULTIMO COMANDO) ===>' $?
echo 'Il valore di $$ (NUMERO DEL PROCESSO IN ESECUZIONE) ===>' $$
