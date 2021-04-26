#!/bin/sh

cd $1

case $1 in 
    */$2?$2)  echo il nome ha 3 caratteri
            echo $1/$i >> $3
            ;;
    *)      ;;
esac


# cerco se ci sono eventuali sotto directory
for i in *
    do # controllo che sia una directory e esplorabile
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2 $3
    fi
done

