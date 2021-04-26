#!/bin/sh

# controllo sul numero di parametri
case $# in 
0|1|2)  echo Hai inserito pochi parametri
        exit 1;;
esac

Y=`expr $1 + 0`
res=$?

if test $res -eq 2 -o $res -eq 3
    then echo il primo paramentro non è un numero
    exit 2
fi

if test $Y -le 0
    then echo Il primo paramentro non è strettamente positivo
    exit 3
fi

shift 

for i in $*
    do # controllo gli altri parametri
    case $i in 
    /*)     if test ! -d 
                then echo $i
                echo Non è una directory
            fi;;
    *)      echo $i 
            echo Non è un nome assoluto
            exit 4;;
    esac

done

PATH=`pwd`:$PATH
export PATH

> /tmp/tmp$$.txt

for i in $*
    do
    FCR.sh $i $Y /tmp/tmp$$.txt
done

echo
echo Il numero di file trovati è: `wc -l < /tmp/tmp$$.txt`

for i in `cat /tmp/tmp$$.txt`
    do
    echo
    echo Stai visualizzando il file:
    echo $i
    echo il suo contennuto è:
    cat $i
done





    
