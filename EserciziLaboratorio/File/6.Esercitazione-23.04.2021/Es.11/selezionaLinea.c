#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

int main (int argc, char **argv){
    int n;
    char c;
    int nread;
    int i = 1;      // inizializzo il numero di linee
    bool f = false;

    if ( argc != 3) {    // controllo sul numero di paramentri
        puts("Errore: numero di parametri errato");
        exit (1);
    }   

    int fd = open(argv[1], O_RDONLY);

    if(fd < 0) {
        puts("ERRORE: apertura del file non riuscita");
        exit(2);
    }

    n = atoi(argv[2]);

    if(n<0){
        puts("ERRORE: inserito secondo parametro errato");
        exit(3);
    }


    while((nread = read(fd, &c, 1) > 0)){
        if (i > n)
            exit (0);

        if (i == n) {
            f = true;
            lseek(fd, -1, SEEK_CUR);

            while((nread = read(fd, &c, 1) > 0)){
                write(1, &c, 1);
                
                if (c == '\n') 
                    break;
            }
        }
        if (c == '\n') 
            i++;
    }

    if (f == false)
        printf("Nel file %s non c'è la riga %d\n", argv[1], n);
    
    return (0);
    
}
