#/bin/sh

#venuto molto diverso ma perchè era un totale e ho capito male

if test $# -le 3
    then echo Hai inserito pochi paramentri
    exit 1
fi

H=`expr $1 + 0` > /dev/null 2>&1
res=$?

if test $res -eq 2 -o $res -eq 3
    then echo il primo parametro è non numerico
    exit 2
fi

K=`expr $2 + 0` > /dev/null 2>&1
res=$?

if test $res -eq 2 -o $res -eq 3
    then echo il secondo parametro è non numerico
    exit 3
fi

if test $H -le 0
    then echo il primo parametro non è strettamente positivo
    exit 4
fi

if test $K -le 0
    then echo il secondo parametro non è strettamente positivo
    exit 5
fi

shift
shift

for i in $*
    do
    if test ! -d $i -a ! -x $i
        then echo Non è una directory oppure non è traversabile
        exit 7
    fi
    
    case $i in 
    /*)     ;;
    *)      echo $i
            echo Non è un nome assoluto di directory
            exit 6;;
    esac
done

PATH=`pwd`:$PATH
export PATH

> /tmp/tmp$$.txt

for i in $*
    do
    FCR.sh $i $H $K /tmp/tmp$$.txt
done

# Per ognuna delle directory trovato devo passare i file giusti alla parte C

for i in `cat /tmp/tmp$$.txt`
    do
    cd $i
    F=
    for j in *
        do
        if test -f $j -a -r $j 
            then
            if test `wc -l < $j` -eq $K
                then
                F="$F $J"
            fi
        fi
    done
    
    # passo al file C la variabile F
done

