#!/bin/sh

# $1 = directory da analizzare
# $2 = carattere da cercare
# $3 = file tmp dove memorizzare i file

cd $1

NF=0

for i in *
    do # cerco fra tutti un file leggibile
    if test -d $i -a -r $i
        then # devo controllare quante volte compare il carattere $2
        n=`grep $2 $i | wc -l`
        if test -f $n -ge 2
            then # salvo file e directory
            NF=`expr $NF + 1`
            echo $1/$i >> $3
        fi
    fi
done

if test $NF -ge 1 
then
	#se abbiamo trovato almeno un file che soddisfa le specifiche, andiamo a stampare il nome assoluto della dir
	echo TROVATO direttorio  `pwd`
fi

for i in *
    do
    if test -d $i -a -x $i
        then
        FCR.sh $1/$i $2 $3
    fi
done 
