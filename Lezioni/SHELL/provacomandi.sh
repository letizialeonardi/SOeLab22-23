#!/bin/sh
echo SONO UN ALTRO ESEMPIO SEMPLICE DI FILE COMANDI
echo La directory di partenza
echo $HOME
echo la PATH $PATH
PATH=/
export PATH
echo La PATH $PATH
echo La directory corrente
pwd
cd .. 
echo Ho cambiato directory  
echo La directory corrente
pwd
echo Verifica dove ti trovi dopo avermi eseguito e verifica anche il valore di PATH
