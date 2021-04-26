#!/bin/sh

if test $# -le 2
    then echo Hai inserito pochi paramentri
    exit 100
fi

# controllo primo parametro:
case $1 in 
/*)     if test ! -d $1 -o ! -x $1
            then echo Non è una directory oppure non è traversabile
            exit 2
        fi;;
*)      echo Il 1 paramentro non è un nome assoluto
        exit 1;;
esac

G=$1

shift

> /tmp/tmpFile$$.txt

for F in $*
    do
  
    case $F in
    */*)    echo non è un file con nome semplice
            exit 4;;
    esac
    
    echo $F >> /tmp/tmpFile$$.txt
done

PATH=`pwd`:$PATH
export PATH

> /tmp/tmpDir$$.txt

echo tutti i file da cercare sono:
echo `cat /tmp/tmpFile$$.txt`
echo

FCR.sh $G /tmp/tmpFile$$.txt /tmp/tmpDir$$.txt

echo
echo Tutte le directory trovate sono:
echo
echo `cat /tmp/tmpDir$$.txt`
