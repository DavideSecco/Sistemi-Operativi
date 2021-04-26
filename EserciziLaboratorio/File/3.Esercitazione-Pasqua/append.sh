#!/bin/bash

if test $# -eq 0 -o $# -ge 3
then 
    echo NUMERO ERRATO DI PARAMETRI
    exit 1
fi

case $# in
1)  echo Caso 1: Scrivi quello che vuoi mettere in append nel file
    read input
    echo $input >> $1;;
2)  echo Caso 2: Scrivo il contenuto del primo paramentro nel file col nome del secondo paramentro
    echo $1 >> $2;;
esac
