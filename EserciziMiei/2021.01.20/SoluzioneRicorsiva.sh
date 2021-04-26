#!/bin/sh
#FCR.sh 
#file comandi ricorsivo 
cont=0	#variabile che ci serve per capire se ci sono tutti i file nella directory

cd $1 	#ci posizioniamo nella directory giusta
G=`basename $1`	#salviamo in particolare solo la parte relativa semplice del nome perche' dopo ci serve quella
#basename e' un comando (verificarne il funzionamento con il man) non visto a lezione/esercitazione, ma indispensabile in questo caso!
#nella correzione dei compiti non e' stato penalizzato chi non lo ha utilizzato!
#ora dobbiamo di nuovo usare shift per eliminare il nome della gerarchia e avere solo i nomi dei file
shift

case $G in
*[0-9]*) #se il nome relativo semplice della gerarchia contiene almeno un carattere numerico 
       	    #NOTA BENE: il controllo va fatto solo sul nome relativo semplice e NON su tutto il nome assoluto e quindi per questa ragione e' stato usato il comando basename!
	for F in *
	do
		if test -f $F #solo file 
		then
			#ora dobbiamo fare un for per tutti i nomi dei file perche' dobbiamo verificare che ci siano tutti nella dir corrente!
			for i	#per tutti i nomi dei file 
			do
				if  test $i = $F  #se il file corrente e' presente, incrementiamo il contatore
				then
					cont=`expr $cont + 1`
					break 	#possiamo uscire dal ciclo piu' interno dato che non ci possono essere piu' file con lo stesso nome
				fi
			done
		fi
	done
	#ora controlliamo se il valore di cont e' uguale al numero di file passati come parametri allora abbiamo trovato una dir giusta
	if test $cont = $#
	then
		echo Trovato una directory che soddisfa le specifiche e il suo nome assoluto e\' `pwd` 
		echo chiamiamo la parte in C con $*
		20Gen21 $*
	fi 
	;;
*) #se il nome relativo semplice della gerarchia NON contiene almeno un carattere numerico NON dobbiamo fare nulla e passeremo a fare la sola ricorsion
		;;
esac

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio 
		$0 `pwd`/$i $* 
	fi
done
