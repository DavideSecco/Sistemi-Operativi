#!/bin/bash

if test $# -le 2
    then echo Hai inserito pochi paramentri
    exit 1
fi

# primo paramentro:
case $1 in
*/*)    echo Doveva essere un nome relativo semplice
        exit 2;;
esac

F=$1

shift

for i in $*
    do
    case $i in 
    /*)     ;;
    *)      echo Dal secondo paramentro in poi devono essere nomi assoluti
            exit 3;;
    esac
done

PATH=`pwd`:$PATH
export Path

# echo Inizia la 1 PARTE RICORSIVA

> /tmp/tmp$$.txt

for i in $*
    do
    FCR.sh $i $F /tmp/tmp$$.txt
done

# echo Inizia la 2 PARTE RICORSIVA

nftot=`wc -l < /tmp/tmp$$.txt`

echo
echo Sono stati trovati in totale $nftot file
echo

echo Visualizziamo file per file:
echo

count=0

for i in `cat /tmp/tmp$$.txt`
    do
    num=`expr $count % 2`
    if test $num -eq 0
        then
        File=$i
        echo File: $File
    else
        echo Lunghezza: $i
        echo
        echo Fino a che linea vuoi visualizzare? [0-$i]
        read X
        if test $X -le 0 -o $X -gt $i
            then echo Hai inserito un numero di linee non corretto
        else
            head -$X $File 
        fi
    fi
    echo
    count=`expr $count + 1`
done

rm /tmp/tmp$$.txt
