#!/bin/sh
#file comandi Cercafile.sh
#ricerca in breadth-first

cd $1       # <== Ci spostiamo nella directory dove dobbiamo, nella invocazione corrente, cercare il file

if test -d $2   # <== Cerchiamo se esiste un file che ha nome relativo semplice come il secondo parametro
    then
    echo la directory $2 si trova in `pwd` # <== Se lo troviamo, riportiamo sullo standard output il nome della directory dove lo abbiamo trovato (quella corrente)!
fi

for i in *
    do
    if test -d $i -a -x $i      # <== Nella directory corrente, controlliamo se ci sono delle sotto-directory traversabili!
        then
        echo Stiamo per esplorare la directory `pwd`/$i
        Cercadir.sh `pwd`/$i $2    # <== Se ci sono dobbiamo, invocare ricorsivamente il file comandi passando come primo parametro il nome ASSOLUTO della sotto-directory e come secondo parametro il nome relativo semplice del file da cercare!
    fi
done
