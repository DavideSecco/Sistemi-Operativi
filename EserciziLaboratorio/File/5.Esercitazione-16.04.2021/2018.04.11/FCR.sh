#!/bin/sh

# $1 = directory da controllare
# $2 = lunghezza  minima del file
# $3 = file tmp su cui scrivere

cd $1

for i in *
    do 
    # controllo che sia un file leggibile
    if test -f $i -a -r $i
        then # controllo la lunghezza
        nl=`wc -l < $i`
        if test $nl -ge $2
            then 
            if test $nl -ge 5
                then 
                echo trovato un file valido: 
                echo $1/$i
                head -5 $i | tail -1 > $i.quinta
                echo $1/$i.quinta >> $3
            else
                > $i.NOquinta
                echo $1/$i.NOquinta >> $3
            fi
        fi
        
    fi
done

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2 $3
    fi
done
