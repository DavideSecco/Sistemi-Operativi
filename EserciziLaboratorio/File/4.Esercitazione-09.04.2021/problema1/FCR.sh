#!/bin/bash

cd $1

numero=false
stampa=false

#ciclo che scorre tutti i file
for i in *
    do  #controllo che il file sia leggibile
    if test -f $i -a -r $i
    then   #controllo che il numero di righe sia giusto
        if test `wc -l < $i` -eq $2
        then   #controllo che tutte quelle righe siano giuste
            for j in `cat $i`
                do
                case $j in
                *[!0-9]*) numero=false; break;;
                *)  numero=true;;
                esac
            done
        fi
    fi
    # controllo che il file vada bene
    # se va bene --> posso uscire e stampo tutto
    # se va male --> ricomincio

    if test $numero = true
        then stampa=true; break
    else
        numero=false
    fi
done

if test $stampa = true
    then
    echo Cartella:
    echo $1
    echo 
    ls -l
    stampa=$false
fi

for u in *
    do
    if test -d $u -a -x $u
        then
        echo 
        FCR.sh `pwd`/$u $2
    fi
done
