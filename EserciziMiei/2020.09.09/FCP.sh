 #!/bin/sh
 
 case $# in 
 0|1|2)     echo Hai inserito pochi paramentri
            exit 1;;
esac

# controllo sul 1 parametro
case $1 in 
/*)     ;;
*)      echo il primo paramentro non è un nome assoluto
        exit 2;;
esac

G=$1

shift

echo controllo gli altri Q paramentri

for i in $* 
    do
    case $i in 
    ?)     ;;
    *)      echo $i non è un carattere
            exit 3;;
    esac
done

PATH=`pwd`:$PATH
export PATH


FCR.sh $G $*
