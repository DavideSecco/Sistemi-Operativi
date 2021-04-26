#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char **argv){
    char buffer;

    if(argc != 3) {
        puts("hai inserito un numero di parametri errato");
        exit (1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd <0 ){
        puts("Errore nell'apertura del file");
        exit (3);
    }
    
    // controllo che il secondo parametro sia un singolo carattere
    if (strlen(argv[2]) != 1){
        puts("il secondo parametro non è un singolo carattere");
        exit (2);
    }

    long int conta = 0;
    while(read(fd, &buffer, 1) > 0){
        if (buffer == *argv[2])
            conta++;
    }

    printf("Numero di occorrenze trovate (%c) nel file %s: %ld\n", *argv[2], argv[1], conta);

    close(fd);
    return 0;
}
