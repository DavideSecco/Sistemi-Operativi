#!/bin/bash

#mi sposto nella directory passata
cd $1

# per ogni elemento della directory verifico:
# -> se è un file leggibile
# -> se ha numero di linee == $2

trovato=false

for i in *
    # verifico che sia un file e che sia leggibile
    do 
    if test -f $i -a -r $i
        then
        # verifico lunghezza del file
            if test `wc -l < $i` -eq $2
                then trovato=true
                echo
                echo file: $i
                echo $i >> $3
                break;
            fi
    fi
done

if test $trovato = true
    then 
    echo Directory: 
    echo $1
fi

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR2.sh $1/$i $2 $3
    fi
done

