#!/bin/sh

cd $1

DirTrovata=false

# controllo che ci sia una sotto cartella:
# ATTEZIONE: si può fare anche con un ciclo!
 if  ls -l | grep '^d' | wc -l
    then DirTrovata=true
fi

for i in *
    do
    if test -f $i -a -r $i
    then
        # controllo che ci sia un file con quelle carateristica
        nl=`wc -l < $i`
        ris=`expr $nl % $2`
        if test $ris -eq 0
            then 
            if test $DirTrovata = "true"
                then
                echo $1 >> $3
                break
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
