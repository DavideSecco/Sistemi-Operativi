#!/bin/sh

# $1 = directory da esplorare
# $2 = Stringa
# $3 = file tmp

cd $1

for i in *
    do
    if test -f $i -a -r $i
        then
        case $i in
        $2.txt)     echo File trovato: $i
                    echo $1/$i >> $3
                    nl=`wc -l < $i`
                    echo $nl >> $3
                    ;;
        *)          ;;
        esac
    fi
done

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2 $3
    fi
done
