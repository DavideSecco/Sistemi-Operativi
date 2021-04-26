#!/bin/sh
#file comandi con altro esempio di for
#supponiamo che nel file t siano stati memorizzati i nomi di alcuni file insieme con la loro lunghezza in linee

# contrllo che i paramentri passati siano 1 e 1 solo
echo $1

if test $# -ne 1
    then
    echo hai inserito il numero sbagliato di paramentri
    exit 1
fi

#controllo che non sia un file
if test ! -f $1 
    then
    echo quello insirito non è un file
    exit 2
fi

# contrllo che il file sia stato inserito con un nome relativo semplice
case $1 in
*/*)    echo devi inserire il nome semplice di un file;;
esac



c=0 #variabile che ci serve all'interno del for per capire se siamo su un elemento dispari (nome file) o su un elemento pari (numero di linee)

for i in `cat $1`
do
    c=`expr $c + 1`
    if test `expr $c % 2` -eq 1 # <== se il resto dell’operazione modulo 2 è 1 vuol dire che l’elemento è dispari
        then
        echo 
        echo elemento dispari, quindi nome di file $i 
        if test -f $i
            then cat $i
        fi
        else #<== se il resto dell’operazione modulo 2 è 0 vuol dire che l’elemento è pari
            echo elemento pari, quindi lunghezza in linee del file appena visualizzato = $i
    fi
done
