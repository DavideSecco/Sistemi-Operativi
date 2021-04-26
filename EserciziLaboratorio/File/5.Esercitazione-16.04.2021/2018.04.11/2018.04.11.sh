#!/bin/sh

case $# in
0|1|2)  echo hai inserito pochi paramentri
        exit 1;;
*)      ;;
esac

# controllo che sia un numero:
Y=`expr $1 + 0`
res=$?

if test $res -eq 2 -o $res -eq 3
    then echo Il primo paramentro è un numero
    exit 2
fi

# controllo che il numero sia positivo
if test $Y -le 0
    then echo Il numero non è strettamente positivo
    exit 3
fi

shift

# controllo sugli altri parametri
for i in $*
    do
    if test ! -d $i -a ! -x $i
        then echo $i non è una directory
        exit 4
    fi
    
    case $i in 
    /*)     ;;
    *)      echo $i
            echo Non è non assoluto di directory
            exit 5;;
    esac
done

PATH=`pwd`:$PATH
export PATH

> /tmp/tmp$$.txt

for i in $*
    do
    FCR.sh $i $Y /tmp/tmp$$.txt
done

NumFile=`wc -l < /tmp/tmp$$.txt`

echo
echo sono stati creati $NumFile
echo


for i in `cat /tmp/tmp$$.txt`
    do
    echo
    echo Visualizzi il file:
    echo $i
    echo
    echo Il suo contenuto è:
    cat $i
done

rm /tmp/tmp$$.txt


















