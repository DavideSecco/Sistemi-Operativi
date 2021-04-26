#!/bin/sh

#test lasco sul numero di paramentri
if test $# -le 2
    then echo Hai inserito pochi paramentri
    exit 1
fi

# controllo sul primo paramentro: singolo carattere
case $1 in
?)      ;;
*)      echo il primo paramentro è sbagliato
        exit 2;;
esac

#salvo il primo paramentro:
C=$1

#scorro i paramentri passati
shift

# controllo su tutti gli altri paramentri --> devono essere nomi di gerarchia
for i in $*
    do
    case $i in 
    /*)     if test ! -d $i -o ! -x $i
                    then echo Non è un cartella oppure non è traversabile
                    exit 4
            fi
            ;;
    *)      echo $i non è un nome assoluto di gerarchia
            exit 3;;
    esac
done


PATH=`pwd`:$PATH
export PATH

echo
echo Inizia la parte ricorsiva
echo

n=1
NumDir=0
> /tmp/tmp$$.txt

# scorro tutte le gerarchie e chiamo il file ricorsivo
for i in $*
    do
    FCR.sh $i $C /tmp/tmp$$.txt
done

NumDir=`wc -l < /tmp/tmp$$.txt`
echo Sono state trovato $NumDir Cartelle

for i in `cat /tmp/tmp$$.txt`
    do
    echo Elena, vuoi visualizzare il contenuto della cartella $i ? [si/no]
    read risposta
    
    case $risposta in
    "si")   ls -la $i;;
    "no")   echo ok;;
    *)      echo hai inserito una risposta non valida;;
    esac
    
done

rm /tmp/tmp$$.txt






