#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define PERM 0644   /* in UNIX */

int appendfile(char *f1) {
    int outfile, nread;
    char buffer [BUFSIZ];

    if ((outfile = open( f1, O_APPEND | O_WRONLY)) < 0) {  /* apertura in scrittura con opzione di posizionarsi alla fine */
        if ((outfile = creat(f1, PERM)) <0)     /* se il file non esiste, viene creato */
            return 1;
    }
    
    while ((nread = read(0, buffer, BUFSIZ)) > 0) {    /* si legge dallo standard input */
        if (write (outfile, buffer, nread ) < nread) { 
            close(outfile); 
            return 2;                                   /* errore */ 
        }
    }
    /* fine del file di input */
    close(outfile); return 0;
}

int main (int argc, char ** argv) { 
    int integi;
    if (argc != 2) { /* controllo sul numero di argomenti */
        printf ("ERRORE: ci vuole un argomento \n");
        exit (3);
    }

    integi = appendfile(argv[1]);
    exit(integi);
}