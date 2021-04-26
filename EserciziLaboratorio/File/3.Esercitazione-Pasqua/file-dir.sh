#!/bin/bash

#occhio che funziona solo con file SENZA SPAZIO


cd $1
n_file=0
n_dir=0

for i in *
do
    if test -f $i
        then   
        echo [F] $i
        n_file=`expr $n_file + 1`
    else
        echo [D] $i
        n_dir=`expr $n_dir + 1`
    fi
done

echo
echo In totale sono state trovate:
echo $n_dir cartelle
echo $n_file file
