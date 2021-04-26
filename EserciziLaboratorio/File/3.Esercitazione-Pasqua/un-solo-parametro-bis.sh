#!/bin/bash

d=$1
echo 'Il percorso è: ' $d

case $d in
/*)     echo NOME ASSOLUTO
        if test -d "$d"
        then
            echo E una directory
        fi
        
        if test -f "$d"
        then
            echo E un file
        else
            echo Non è ne un file ne una directory
        fi;;
*/*)    echo NOME RELATIVO;;
*[!/]*) echo NOME RELATIVO SEMPLICE;;
*)      echo ERRORE
esac
