#!/bin/sh

if test $# -le 2
    then echo Hai inserito pochi paramentri
    exit 1
fi

count=1

for i in $*
    do
    if test $count -lt $# # controllo sui primi Q paramentri
        then # controllo che sia un nome assoluto
        if test ! -d $i -o ! -x $i
            then echo Il paramentro $count non è una directory
            exit 2
        fi
        
        case $i in
        /*)     ;;
        *)      echo Non è il nome assoluto
                exit 3;;
        esac
    else # controllo sull'ultimo paramentro
        B=$i
        B=`expr $B + 0` > /dev/null 2>&1
        res=$?
        
        if test $res -ne 0
            then echo "L'ultimo parametro non è un numero"
            exit 4
        fi
        
        if test $B -le 0
            then echo il numero non è strettamente positivo
            exit 5
        fi
    fi
    
    count=`expr $count + 1`
done

PATH=`pwd`:$PATH
export PATH

> /tmp/tmp$$.txt

count=1

for i in $*
    do
    if test $count -lt $#
        then
        FCR.sh $i $B /tmp/tmp$$.txt 
        count=`expr $count + 1`
    fi
done

for j in `cat /tmp/tmp$$.txt`
    do
    cd $j
    for i in *
        do
        if test -f $i
        then
            nl=`wc -l < $i`
            echo nl = $nl
            echo p1
            ris=`expr $nl % $B`
            echo ris = $ris
            echo p2
            if test $ris -eq 0
                then 
                > $j/$i.Chiara
                
                # Parte C
            fi
        fi
    done
done






