#!/bin/sh

for i in *
do
    case $i in
    p*)     echo \il file trovato è: "$i"
            if test -f $i
                then 
                echo il contenuto del file è:
                cat $i      
            else
                echo '-->' è una cartella
            fi;;
    esac
done
