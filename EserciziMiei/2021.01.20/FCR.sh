#!/bin/sh

# $1 = cartella da controllare
# $2 = file tmp dove sono memorizzati i file da cercare
# $3 = file dove devi scirvere le cartelle

# ATTENZIONE: la mia soluzione è parzialmente sbagliata: io infatti controllo che tutto il percorso della cartella abbia un numero, mentre era solo il nome semplice da controllare --> la soluzione prevedeva di usare un comando che non abbiamo visto [VEDI SOLUZIONE]
# COMANDO: BASENAME

cd $1

echo Directory:
echo $1
echo

file=true

for F in `cat $2`
    do
    echo File: $F
    if test -f $F
        then echo Cè il file: $F
            echo
    else
        echo Non cè un $F file nella cartella
        echo
        file=false
        break
    fi
done

case $1 in
*[0-9]*)   if test $file = true
                then echo Cartella idonea:
                echo $1
                echo $1 >> $3
            fi;;
esac

for i in *
    do
    if test -d $i -a -x $i
        then FCR.sh $1/$i $2 $3
    fi
done
