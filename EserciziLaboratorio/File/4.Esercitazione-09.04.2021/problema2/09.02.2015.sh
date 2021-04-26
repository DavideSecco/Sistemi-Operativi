#!/bin/sh

case $# in 
	[0-1]) echo Errore pochi paramentri.
	exit 1;;
	2) ;;
	*) echo Errore troppi parametri.
	exit 2;;
esac

if test $2 -le 0
	then echo $2 non numerico o non strettamente positivo
	exit 3
fi


case $1 in 
	/*) if test ! -d $1 -o ! -x $1
	then
		echo $1 non directory o non traversabile 
		exit 4
	fi;;
	*) echo $1 nome non assoluto; exit 5;;
esac

echo Sono rispettati tutti i paramentri per le variabili di ingresso

PATH=`pwd`:$PATH
export PATH

> "/tmp/tmp$$"

FCR2.sh $1 $2 /tmp/tmp$$

echo

for i in `cat /tmp/tmp$$`
    do
    echo "Inserisci il valore per il file $i (minore o uguale a K)"
    read var1
    
    if test $var1 -le 0
        then echo devi inserire un valore strettamente positivo
        exit 6
    fi
    
    if test $var1 -gt $2
        then echo devi inserire un valore minore di K
        exit 7
    fi
done

echo 
echo richiamo il file c

rm /tmp/tmp$$
