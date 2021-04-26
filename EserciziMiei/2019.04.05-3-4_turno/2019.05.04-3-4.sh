#!/bin/sh

# ATTENZIONE soluzione giusto ma probabilmente non come vuol lei sbaglaita

case $# in 
0|1|2)  echo Hai inserito pochi paramentri
        exit 1;;
esac

# controllo sul primo paramentro
case $1 in
*/*)    echo Il primo paramentro non è un nome semplice
        exit 2;;
esac

D=$1

#controllo sugli altri N paramentri
shift

for i in $*
    do
    case $i in
    /*)     if test ! -d $i -o ! -x $i
                then echo E nome assoluto, ma non è un directory oppure non è traversabile
                exit 3
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
    FCR.sh $i $D /tmp/tmp$$.txt 
done

echo
echo Uscita dalla ricorsione
nfile=`wc -l < /tmp/tmp$$.txt`
nfile=`expr $nfile / 2`

echo Il numero di file trovati è: $nfile

cont=1

for i in `cat /tmp/tmp$$.txt`
    do
    if test `expr $cont % 2` -eq 1
        then
        echo File
        echo $i
        file=$i
        
    else
        echo Lunghezza file: $i
        echo Che linea vuoi visualizzare [1-4]?
        read K
           
        case $K in
        [1-4])   head -$K < $file | tail -1
                 ;;
        *)       echo Hai inserito un valore errato;;
        esac
    
    
    fi
    cont=`expr $cont + 1`
done










