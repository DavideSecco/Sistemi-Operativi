#!/bin/bash

if test $# -eq 0
    then   
    echo PARAMETRI INSUFFICENTI
    exit 1
fi

for i in $*
do
echo $i
done
