#!/bin/sh
#file comandi leggi e mostra
#CONTROLLO SU PRESENZA DI UN SINGOLO PARAMETRO DIVERSO DA PRIMA

if test $# -eq $0
then
echo Errore: voglio almeno un parametro
exit 1
fi

for i in $* 
    do
    if test ! -f $i -o ! -r $i
        then
        echo Errore: $i non file oppure non leggibile
        exit 2
    fi

    echo "\nvuoi visualizzare il file $i (si/no)?" #> /dev/tty
    read var1

    if test $var1 = si
        then
        ls -la $i; cat $i
        else echo niente stampa di $i
    fi
done
