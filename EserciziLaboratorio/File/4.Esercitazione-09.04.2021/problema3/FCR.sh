#!/bin/bash

cd $2
echo
echo Directoy: $2

# scorro tutti i file nella directory attuale
for i in *
    do #controllo che sia un file e che sia leggibile
    if test -f $i -a -r $i
        then # setto le righe a 0 e poi per ogni linea guardo se se il carattere finale è t
        echo
        echo file: $i
        numero_righe=0
        for j in `cat $i`
                do
                case "$j" in
                *t)     numero_righe=`expr $numero_righe + 1`
                esac
                
                # se il numero di righe ha raggiunto quello sufficente mi fermo:
                # 1. incremento ul numero di file
                # 2. salvo il nome del file in un file temporaneo
                if test "$numero_righe" = "$1"
                    then
                        echo preso
                        echo `pwd`/$i >> $3
                        break
                fi
        done
    fi
done

for u in *
    do if test -d $u -a -x $u
        then
            echo Cè una sottodirectory!
            FCR.sh $1 $2/$u $3
        fi
    
done

