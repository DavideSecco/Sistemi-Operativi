#!/bin/sh

for i in `ls -a`
    do
    case $i in
    #caso in cui è un file directory nascosta
    .*)     echo $i è un file nascosto!!!!!!;;
    esac
done
