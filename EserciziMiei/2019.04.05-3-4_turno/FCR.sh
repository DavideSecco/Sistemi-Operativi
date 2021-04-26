#!/bin/sh

# $1 = directory che stiamo esplorando ora
# $2 = nome directory che stiamo cercando
# $3 = file tmp su cui scrivere i file

cd $1

# per tutti i file nella cartella corrente
case $1 in
*/$2)   echo
        echo Abbiamo trovato una directory valida:
        echo $1
        # entro nella directory
        # cerco fra tutti i file:
        for j in *
            do
            if test -f $j -a -r $j
                then # verifico che abbia almeno 4 linee
                nl=`wc -l < $j`
                if test $nl -gt 4
                    then # file trovato
                    echo -n "trovato file idoneo: " 
                    echo  $j
                    echo $1/$j >> $3
                    echo $nl >> $3
                fi
            fi
        done;;
esac

cd $1

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2 $3
    fi
done
