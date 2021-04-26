#!/bin/sh
#FCR.sh 
#file comandi ricorsivo 

cd $1
#la variavile B ci serve per il secondo parametro
B=$2
#la variabile L ci serve per la lunghezza de file cioe' il numero di caratteri dei file
L=
#la variabile DIR ci serve per capire se c'e' almeno una directory 
DIR=false
#la variabile trovato ci serve per sapere se abbiamo trovato almeno un file che rispetta la specifica
trovato=false
#la variabile files ci serve per salvare i nomi dei file che rispettano la specifica
files=

for i in *
do
	#controlliamo solo i nomi dei file (se inseriamo anche il controllo se leggibili per cautelarci dato che dopo usiamo il comando wc bisogna commentarlo!)
	if test -f $i 
	then 	
		#calcoliamo il numero di caratteri 
		L=`wc -c < $i`
		#controlliamo se la lunghezza e' multiplo intero di B, avendo cura di controllare che la lunghezza NON sia 0
		if test `expr $L % $2` -eq 0 -a $L -ne 0
			then
			#abbiamo trovato un file che soddisfa le specifiche e quindi lo inseriamo nella variabile files
			files="$files $i"
			trovato=true
		fi
	else
		if test -d $i
		then
		#abbiamo trovato una directory e quindi dobbiamo mettere a true DIR
		DIR=true
		fi
	fi
done
#se DIR e a true e files non e' vuoto allora abbiamo trovato una directory giusta 
if test $DIR = true -a $trovato = true  
then
	echo TROVATO dir `pwd`
	for F in $files
	do
		#creaiamo il file con il nome richiesto
		> $F.Chiara
		L=`wc -c < $F`
        	echo chiamiamo la parte C con file $F, L uguale a $L e $B
		17Giu20 $F $L $B
	done
fi

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory 
		FCR.sh `pwd`/$i $2  
	fi
done

