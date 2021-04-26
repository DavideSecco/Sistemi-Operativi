#!/bin/bash

#Con un editor, scrivere un file comandi di nome ultimo-par.sh che riporti su standard output l'ultimoparametro della linea di comando e che riporti anche la lista dei parametri escluso l’ultimo. Verificarne il funzionamento.

#stampo ultimo parametro:

c=0

for i in $*
    do
    c=`expr $c + 1`
    if test $c -eq $#
        then
        echo Ultimo valore è: $i
    fi
done

echo "Tutti i valori tranne l'ultimo sono: "
c=0

for i in $*
    do
    c=`expr $c + 1`
    if test $c -lt $#
        then
        echo $i
    fi
done
    
