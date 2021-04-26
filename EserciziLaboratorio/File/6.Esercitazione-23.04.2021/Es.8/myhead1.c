#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char **argv) {
    char c;
    int n;

    if (argc != 2) {
        puts("Hai inserito il numero di paramentri sbagliati");
        exit (1);
    }

    // devo prevedere il "-" davanti al numero? potrei usare abs (per i valori assoluti...)
    if (argv[1][0] != '-'){
        puts("Errore: devi inserire il simbolo di opzione");
        exit (3);
    }
    else{
        n = atoi(&argv[1][1]);
        if (n < 1){
            puts("Opzione non corretta");
            exit (4);
        }
    }

    printf("numero: %d\n", n);

    int i = 1; /* inizializzo il conteggio delle linee a 1 */
    while (read (0, &c, 1) != 0)
    {
        if (c == '\n') 
            i++;                /* se troviamo il terminatore di linea, incrementiamo il conteggio */
        write(1, &c, 1);        /* scriviamo comunque il carattere qualunque sia */
        
        if (i > n) 
            break;       /* se il conteggio supera n, allora usciamo dal ciclo di lettura */
    }  
    exit (0);    
}
