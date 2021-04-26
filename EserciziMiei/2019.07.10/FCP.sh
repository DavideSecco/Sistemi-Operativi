#!/bin/sh

case $# in 
0|1|2)  echo Hai inserito pochi parametri
        exit 1;;
esac

# controllo sul primo carattere
case $1 in
?)  ;;
*)  echo il primo paramentro non è un singolo carattere
    exit 2;;
esac

W=$1

shift

for i in $*
    do
    case $i in 
    /*)     if test ! -d $1
                echo $i non è una directory
                exit 3
            fi;;
    *)      echo Non hai inserito un nome assoluto di directory
            exit 4;;
    esac
done

PATH=`pwd`:$PATH
export PATH

for i in $*
    do
    FCR.sh $1 $W /tmp/tmp$$.txt
done

echo Numero totale di file trovati: `wc -l < /tmp/tmp$$.txt`

# invoco la parte in C (file memorizzati in /tmp/tmp$$.txt

rm /tmp/tmp$$.txt
