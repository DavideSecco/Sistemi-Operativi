#!/bin/bash

#19) Con un editor, scrivere due file comandi; il primo file comandi si deve chiamare FCP.sh e deve fare i seguenti controlli: di essere invocato esattamente con 1 parametro e che tale parametro rappresenti un nome assoluto di directory esistente; il secondo file comandi si deve chiamare file-dir.sh e deve scorrere l'intero contenuto della directory il cui nome è passato come parametro e deve riportare una [F] prima del nome di ogni file regolare ed una [D] prima del nome di ogni directory e, da ultimo, deve anche riportare su standard output il numero totale di nomi di file e directory stampati. Verificarne il funzionamento. NOTA BENE: il file file-dir.sh NON deve essere ricorsivo.

#controllo che ci sia un solo parametro

if test $# -ne 1
    then
    echo Devi inserire un solo paramtro
    exit 1
fi

#controllo che il parametro sia il nome assoluto di una directory:

if test ! -d $1
    then 
    echo Devi passare una directory
    exit 2
fi

case $1 in
[!/]*)  echo Devi inserire il nome assoluto della directory
        exit 3;;
esac

#Non ho idea se questa parte serve o meno, lo si verifica gà al punto 2?
#cd $1
if test $? -eq 0
    then 
    echo Bravo! La directory esiste
else
    echo La directory non esiste!
fi

sh file-dir.sh $1
