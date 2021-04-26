#!/bin/sh
#file comandi Beginc.sh
d=  #la variabile d serve per memorizzare o il primo parametro oppure la directory corrente
f=  #la variabile f serve per memorizzare il parametro che rappresenta il nome relativo semplice del file (primo o secondo parametro a seconda dei casi)

case $# in
0)      echo "POCHI PARAMETRI - Usage is: $0 [directory] file"
        exit 1;;
1)      d=`pwd`; f=$1;;
2)      d=$1; f=$2;;
*)      echo " TROPPI PARAMETRI - Usage is: $0 [directory] file"
        exit 2;;
esac    # <== se il numero di parametri è corretto, avendo usato le variabili d e f, da qui in poi possiamo trattare il caso di 1 e 2 parametri in modo unitario!

#controllo che d sia dato in forma assoluta
case $d in
/*)     ;;      # <== solo se inizia con / è un nome assoluto!
*)      echo Errore: $d non in forma assoluta
exit 3;;
esac

#controllo che f sia dato in forma relativa semplice
case $f in
/*)    echo Errore: $f non in forma relativa   # !!!!! QUi hai modificato!
        exit 4;;
*)      ;; # <== solo se NON contiene uno / è un nome relativo semplice!
esac

#controllo che d sia una directory e che sia accessibile
if test ! -d $d -o ! -x $d
    then echo Errore: $d non directory o non attraversabile; exit 5
fi

# QUi NON VA!!!!!
#AGGIUNTA MODIFICA PER IL LAB: non devo più cercare un file, ma una directory
if test ! -d $f 
    then echo Errore: $f non è una directory
    exit 6
fi

PATH=`pwd`:$PATH    
# NOTA BENE: nel valore della variabile PATH bisogna inserire la directory dove si trovano i file Beginc.sh e Cercafile.sh: si è usato il risultato di `pwd` nell’ipotesi (altamente plausibile) che il file comandi Beginc.sh venga invocato nella directory che lo contiene; se questo non è l’uso plausibile, bisogna ESPLICITAMENTE inserire il nome completo della directory (nel nostro caso /home/soELab/file-comandi/ricorsione)!

export PATH
# Dopo la modifica della variabile PATH, il Bourne shell richiede di esportare di nuovo la variabile per rendere effettive le modifiche sulla variabile di ambiente PATH 

echo Beginc.sh: Stiamo per esplorare la directory $d
Cercadir.sh $d $f      #<== invocazione del secondo file comandi (quello ricorsivo)!
echo HO FINITO TUTTO
