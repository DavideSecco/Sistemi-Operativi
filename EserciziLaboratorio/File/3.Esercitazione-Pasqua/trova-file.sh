#!/bin/bash

if test $# -ne 1
then
    exit 1
fi

case $1 in
*/*)     echo NON è un nome relativo semplice
            exit 2;;
esac

if test -f $1
then 
echo Esiste il file
else
echo Non esiste il file
fi
