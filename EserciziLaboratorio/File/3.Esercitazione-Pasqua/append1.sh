#!/bin/bash

if test $# -ne 2
    then
    echo NUMERO DI PARAMETRI SBAGLIATO
    exit 1;
fi

case $1 in
*/*)    echo FILE 1: NON È UN NOME RELATIVO SEMPLICE
        exit 2;;
esac

case $2 in
*/*)    echo FILE 2: NON È UN NOME RELATIVO SEMPLICE;
        exit 3;;
esac

if test ! -f $1
    then 
    echo Il primo paramentro non è un file
    exit 4;
fi

if test ! -r $1
    then 
    echo Il primo paramentro è un file, ma non è leggibile
    exit 5;
fi

#testo le condizioni del secondo paramentro
echo Il secondo file:
if test -f $2       #il secondo file esiste
    then 
    echo  esiste
    if test -w $2
        then 
        echo è scrivibile
    else
        echo non è scrivibile 
    fi
else
    echo non esiste
    if test -w .
        then
        echo ma la directory è scrivibile
    else
        echo e la directory non è scrivibile
    fi
fi
