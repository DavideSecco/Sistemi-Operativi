#!/bin/bash

d=$1
echo 'd=$1' "--> E stato inserito $d"

case $d in
/*)     echo NOME ASSOLUTO;;
*/*)    echo NOME RELATIVO;;
*[!/]*) echo NOME RELATIVO SEMPLICE;;
*)      echo ERRORE
esac
