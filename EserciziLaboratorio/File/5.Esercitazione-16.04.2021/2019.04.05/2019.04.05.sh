#!/bin/sh

case $# in
0|1|2)  echo Hai inserito pochi paramentri
        exit 1;;
*)      ;;
esac

count=1

for i in $*
    do
    if test $count -eq $#
        then # controllo sull'ultimo parametro
        S=$i
        case $i in 
        */*)    echo Meglio inserire un nome semplice!
                break;;
        *)      ;;
        esac    
    else
        # COntrollo sulle gerarchie assolute
        case $i in
        /*)     if test ! -d $i 
                    then echo Non è una directory
                    exit 3
                fi;;
        *)      ;;
        esac
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
        FCR.sh $i $S /tmp/tmp$$.txt
        
    fi
    count=`expr $count + 1`
done

FileTot=`wc -l < /tmp/tmp$$.txt`

echo
echo Numero file trovati globali: $FileTot 
echo

count=0

for i in `cat /tmp/tmp$$.txt`
    do
    if test `expr $count % 2` -eq 0
        then echo File: $i
        File=$i
        
    else echo Numero linee: $i
         echo
         echo Vuoi Ordinare il file? [si, no]
         read var
         case $var in
         si)   sort -f $File | cat;;
         no)    echo Perfetto, non mostro nulla;;
         *)     echo Hai inserito un risposta non valida;;
         esac
    fi
    count=`expr $count + 1`
done












