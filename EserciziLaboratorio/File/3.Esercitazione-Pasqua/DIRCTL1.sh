#!/bin/sh
if test $# -ne 1
then	
echo Devi inserire esattamente 1 parametro
exit 1
fi

echo SONO DIRCTL1.sh
echo '$0 ==>' $0
#miglioramento risp. file originale -->  #aggiungere le virgolette così che prenda anche file con lo spazio
echo '$1 ===>' "$1"

#controllo se è una directory
if test -d $1
then
echo E una directory!
ls -lR "$1" 
fi

#controllo se è un file
if test -f $1
then
echo E un file
ls -l "$1" 
fi



     
            
