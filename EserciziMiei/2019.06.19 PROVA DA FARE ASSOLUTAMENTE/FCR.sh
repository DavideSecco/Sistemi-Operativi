#!/bin/sh

# $1 = cartella da esplorare
# $2 = K = numero max di linee

cd $1

> /tmp/tmp$$.txt

for i in *
    do
    if test -f $i -a -r $i
        then
        nl=`wc -l < $i`
        if test $nl -le $2
            then
            echo $i >> /tmp/tmp$$.txt
        fi
    fi
done

nf=`wc -l < /tmp/tmp$$.txt`

if test $nf -ge 2
    then
    # stampo la directory su stdout
    echo
    echo DIRECTORY: $1
    cat /tmp/tmp$$.txt
    # INVOCO LA PARTE C passando i nomi dei file 
fi

rm /tmp/tmp$$.txt

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2    
    fi
done


