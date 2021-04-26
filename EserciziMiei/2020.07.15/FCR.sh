#!/bin/sh

# $1 = directory su cui devo fare il controllo
# $2 = lunghezza che deve avere il file
# $3 = file tmp su cui scrivere

cd $1

for i in *
    do #verifico sia un file
    if test -f $i -a -r $i
        then # conto il numero di linee e di caratteri
        nl=`wc -l < $i`
        nc=`wc -c < $i`
        #verifico il numero di linee sia uguale a L ($2)
        if test $nl -eq $2
            then # conttorllo sulla lunghezza media della linee
            media=`expr $nc / $nl`
            
            if test $media -gt 10
                then # FILE VALIDO TROVATO
                echo `pwd`/$i >> $3       
            fi     
        fi
    fi
done

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2 $3
    fi
done
