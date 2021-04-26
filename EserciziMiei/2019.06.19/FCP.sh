#!/bin/sh

# controllo sul numero di paramentri
case $# in
0|1|2)  echo Hai inserito pochi parametri
        exit 1;;
esac

# controllo sul primo paramentro
H=`expr $1 + 0`
res=$?

if test $res -eq 2 -o $res -eq 3
    then echo $1 non è un numero
    exit 2
fi

if test $H -le 1 -o $H -gt 255
    then echo Il primo parametro non è fra 0 e 255
    exit 3
fi

# scorro i paramentri così mi è più comodo per il ciclo dopo
shift

# controllo sulle gerarchie
for i in $*
    do
    case $i in 
    /*)     if test ! -d $i
                then echo $i non è una directory
                exit 3
            fi;;
    *)      echo $i non è un nome assoluto
            exit 4;;
    esac
done

# aggiungo la cartella corrente alle variabile PATH, così che lo script possa essere richiamato da ovunque nel sistema operativo
PATH=`pwd`:$PATH
export PATH

for i in $*
    do
    FCR.sh $i $H
done




