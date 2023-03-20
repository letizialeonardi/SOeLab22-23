#!/bin/sh
#file comandi con primo esempio di case
#chiediamo all'utente di fornire una risposta
echo 'Fornire una risposta (affermativa ===> Si, si, Yes, yes)'
#leggiamo la risposta dell'utente
read risposta
#verifichiamo con un case la risposta dell'utente
case $risposta in
S* | s* | Y* | y*) echo OK;;
*)		   echo NO;;
esac
