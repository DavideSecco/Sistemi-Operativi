 
#!/bin/sh
#Soluzione dell'esame del 20 Gennaio 2021

case $# in
0|1|2) 	echo Errore: numero parametri is $# quindi pochi parametri. Usage is $0 L dirass1 dirass2 ...
	exit 1;;
*) 	echo OK: da qui in poi proseguiamo con $# parametri ;;
esac

#Controllo primo parametro sia una directory
case $1 in
/*) 	if test ! -d $1 -o ! -x $1
	then
		echo $1 non directory
		exit 2
	fi;;
*) 	echo $1 non nome assoluto
	exit 3;;
esac

G=$1 #salviamo il primo parametro (N.B. nella variabile il cui nome viene specificato nel testo)
#echo G = $G

#quindi ora possiamo usare il comando shift
shift

#ora in $* abbiamo solo i nomi dei file (relativi semplici) caratteri e quindi possiamo fare i controlli adeguati
for i 
do
	case $i in
	*/*) echo NON e\' un nome relativo semplice
	     exit 4;;
	*)   #echo OK $i e\' un nome relativo semplice  
		;;
	esac
done

#controlli sui parametri finiti 
PATH=`pwd`:$PATH
export PATH

#invochiamo il file comandi ricorsivo con la gerarchia e i nomi dei file 
FCR.sh $G $*
