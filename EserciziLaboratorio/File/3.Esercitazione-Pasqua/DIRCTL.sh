#!/bin/sh
if test $# -ne 1
then	
echo Devi inserire esattamente 1 parametro
exit 1
fi

echo SONO DIRCTL.sh
echo '$0 ==>' $0 
echo '$1 ===>' "$1"     #miglioramento risp. file originale --> 
ls -l "$1"              #aggiungere le virgolette così che prenda anche file con lo spazio
