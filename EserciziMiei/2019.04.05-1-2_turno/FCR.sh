#!/bin/bash

# $1 = cartella da esploarare
# $2 = nome file da trovare
# $3 = file temporaneo su cui scrivere

cd $1

for i in *
    do #controllo che sia un file leggibile
    if test -f $i -a -r $i
        then # controllo che il nome sia F
        if test $2 = F
            then # controllo che il numero di linee sia >= 4
            nl=`wc -l < $i`
            if test $nl -ge 4
                then 
                Stringa="`pwd`/$i $nl"
                echo $Stringa >> $3
            fi
        fi
    fi
done

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh `pwd`/$i $2 $3
    fi
done
