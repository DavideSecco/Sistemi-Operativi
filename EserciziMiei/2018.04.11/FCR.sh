#!/bin/sh

cd $1

echo
echo Directory
echo $1
echo

for i in *
    do # controllo che sia un file e leggibile
    if test -f $i -a -r $i
        then # controllo la lunghezza
        echo trovato file:
        echo $i
        nl=`wc -l < $i`
        if test $nl -ge $2
            then # vado avanti nei due casi
            if test $nl -ge 5
            then
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
