#!/bin/sh
echo SONO DIR.sh
echo '$0 ===>' $0 
echo '$1 ===>' "$1" # <-- aggiungere le " " è miglioramento risp.
ls -l "$1"          # file originale, così puoi usare anche file con spazi
