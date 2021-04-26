#!/bin/bash

#controllo di passare 2 paramentri
if test $# -ne 2
    then
    echo Devi inserire esattamente 2 paramentri
    exit 1
fi

case $1 in
/*)     ;;
*)      exit 2;;
esac

if test ! -d $1 -o ! -x $1
    then
    echo Non hai passato una directory oppure non è una directory traversabile
    exit 3; 
fi

# controllo che il secondo parametro sia un numero

expr $2 + 0 >/dev/null 2>&1
var=$?

if test $var -ne 0
    then    
    echo Il secondo parametro non è un numero
    exit 4
else
    # controllo che il 2° paramentro sia strettamente positivo
    if test $2 -le 0
        then
        echo il secondo paramentro è minore uguale a 0
        exit 5
    fi
fi

# export varibile Path
PATH=`pwd`:$PATH
export PATH

# Passo al file comandi ricorsivo

FCR.sh $1 $2

