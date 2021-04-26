#!/bin/sh

case $# in
0|1|2)  echo Hai inserito pochi paramentri
        exit 1;;
esac

# controllo primo paramentro:
L=`expr $1 + 0`
res=$?

if test $res -eq 2 -o $res -eq 3
    then echo Il primo paramentro non è un numero
    exit 2
fi

if test $L -le 0   
    then echo Il primo paramentro non è stressamente positivo
    exit 3
fi

shift

for i in $*
    do
    if test ! -d $i
        then echo $i
        echo Non è una directory
        exit 4
    fi
    
    case $i in 
    /*)     ;;
    *)      echo $i 
            echo Non è un nome assoluto di directory
            exit 5;;
    esac         
done 

PATH=`pwd`:$PATH
export PATH



for i in $*
    do
    > /tmp/nomiAssoluti$$
    FCR.sh $i $L /tmp/nomiAssoluti$$
    
    nl=`wc -l < /tmp/nomiAssoluti$$`
    echo
    echo Gerarchia $i
    echo il numero di file trovati: $nl
    
    for file in `cat /tmp/nomiAssoluti$$`	#il nome della variabile del ciclo viene specificato nel testo!
        do
        echo Chiamiamo la parte in C con $file e $L
        #15Lug20 $file $L
    done
done

rm /tmp/nomiAssoluti$$







