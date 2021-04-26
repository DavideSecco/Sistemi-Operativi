#!/bin/sh
#Soluzione della Prova del 17 Giugno 2020
#versione che usa un file temporaneo e usa FCR.sh per tutte le fasi

#controllo sul numero di parametri: deve essere maggiore o uguale a 3 
case $# in
0|1|2)	echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 dirass1 dirass2 ... numero
	exit 1;;
*) 	echo OK: da qui in poi proseguiamo con $# parametri ;;
esac
#dobbiamo isolare l'ultimo parametro e intanto facciamo i controlli
num=1 	#la variabile num ci serve per capire quando abbiamo trovato l'ultimo parametro
params=	#la variabile params ci serve per accumulare i parametri a parte l'ultimo
#in $* abbiamo i nomi delle gerarchie e il numero intero 
for i 
do
	if test $num -ne $# #ci serve per non considerare l'ultimo parametro che e' il numero
	then
		#soliti controlli su nome assoluto e directory traversabile
		case $i in
		/*) 	if test ! -d $i -o ! -x $i
	    		then
	    		echo $i non directory o non attraversabile
	    		exit 2
	    		fi;;
		*)  	echo $i non nome assoluto; exit 3;;
		esac
		params="$params $i" #se i controlli sono andati bene memorizziamo il nome nella lista params
	else
	#abbiamo individuato l'ultimo parametro e quindi facciamo il solito controllo su numerico e strettamente positivo
		#Controllo ultimo  parametro (con expr)
		expr $i + 0  > /dev/null 2>&1
		N1=$?
		if test $N1 -ne 2 -a $N1 -ne 3
		then #echo numerico $X siamo sicuri che numerico
     			if test $i -le 0
     			then echo $i non strettamente positivo
          		exit 4
     			fi
		else
  			echo $X non numerico
  			exit 5
		fi
		B=$i #se i controlli sono andati bene salviamo l'ultimo parametro
	fi
	num=`expr $num + 1` #incrementiamo il contatore del ciclo sui parametri
done
#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

for i in $params
do
	echo fase per $i 
	#invochiamo il file comandi ricorsivo con la gerarchia, il numero e il file temporaneo
	FCR.sh $i $B 
done

echo FINITO TUTTO! 
