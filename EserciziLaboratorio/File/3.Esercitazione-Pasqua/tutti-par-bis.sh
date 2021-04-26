#!/bin/bash

#controllo che i paramentri non siano 0
if test $# -eq 0
    then   
    echo PARAMETRI INSUFFICENTI
    exit 1
fi

#non sono sicuro la soluzione sia aggiungere la variabile c e tenere conto del 
# numero dei PARAMETRI
c=0
for i in $*
do
c=`expr $c + 1` 
echo '$'$c = $i 
done
