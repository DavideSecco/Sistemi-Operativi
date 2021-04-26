#!/bin/sh

cd $1
echo 
echo Directory attuale:
echo $1

G=$1
shift

# controllo tutto il contenuto della directory alla ricerca di un file adatto
for i in *
    do # verifico che sia un file leggibile
    trovato=true
    if test -f $i -a -r $i
        then
        echo Analizzo file: $i
        for j in $*
            do # verifico che ci siano tutti i paramentri
            Num=`grep $j $i | wc -l`
            if test $Num -eq 0
                then
                trovato=false
                break
            fi
        done
        if test $trovato = true
            then
            echo -n "File che passo alla parte C: "
            echo $i
            # invoco la parte C
        else
            echo file non valido
        fi
    fi
done


for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $G/$i $*
    fi
done
