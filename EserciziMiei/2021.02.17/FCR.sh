#!/bin/sh

# $1 = directory da esplorare
# $2 = numero di file da trovare
# $3 = numero di righe che devono avere i file
# $4 = file tmp

cd $1
echo 
echo Directory:
echo $1

cont=0

trovato=true

for i in *
    do #controllo che sia un file
    if test -f $i -a -r $i 
        then #contorllo il numero di linee
        nl=`wc -l < $i`
        if test $nl -eq $3
            then #File trovato!
            echo file trovato: $i
            cont=`expr $cont + 1`
        fi
    fi
done

if test $cont -ge $2
    then
    echo Directory che rispetta le condizioni!
    echo $1 >> $4
else 
    echo Non salvo la directory, non rispetta le condizioni
fi

for i in *
    do
    if test -d $i -a -x $i
        then 
        FCR.sh $1/$i $2 $3 $4
    fi
done
