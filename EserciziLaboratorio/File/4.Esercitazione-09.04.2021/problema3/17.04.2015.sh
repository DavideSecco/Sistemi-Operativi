#!/bin/bash

if test $# -lt 3
    then echo Inseriti pochi parametri
    exit 1
fi

expr $1 + 0 > /dev/null 2> /dev/null
var=$?

if test $var -ne 0
    then echo Il primo parametro non è un numero
    exit 2
fi

if test $1 -le 0
    then echo Inserito numero negativo
    exit 3;
fi

for i in $*
    do
    if test $i != $1
        then 
            case $i in
            /*)     ;;
            *)      echo il parametro $i non è un nome assoluto di directory
                    exit 4;;
            esac
    fi
done

PATH=`pwd`:$PATH
export PATH

> /tmp/tmp$$.txt

for i in $*
    do
    if test $i != $1
        then 
            FCR.sh $1 $i /tmp/tmp$$.txt
    fi
done

echo
echo Ho trovato `wc -l < /tmp/tmp$$.txt` file
echo

for i in `cat /tmp/tmp$$.txt`
    do
    echo Ho trovato questo file:
    echo $i
    echo Che numero vuoi inserisci?
    read K
    
    if test $K -le 0 -o $K -ge $1
        then echo Hai inserito un numero non permesso
    else
        head -$K $i | tail -1
    fi
    
done

echo Ho finito

rm /tmp/tmp$$.txt
