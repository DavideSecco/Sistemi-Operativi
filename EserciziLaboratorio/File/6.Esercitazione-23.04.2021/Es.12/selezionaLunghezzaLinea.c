#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main (int argc, char **argv){
    int fd;
    int n;
    char c;                     // dove metto i caratteri
    int lung=0;
    char buff [BUFSIZ];
    bool f = false;
    int nread;

    if ((fd = open(argv[1], O_RDONLY)) < 0){
        puts ("ERRORE: apertura file non riuscita");
        exit (1);
    }

    n = atoi(argv[2]);

    if (n <=.0){
        puts("ERRORE:secondo paramentro non corretto");
        exit (2);
    }

    while((nread = read(fd, &c, 1)) > 0){
        buff[lung] = c;    // salvo il carattere appena letto nel caso la riga vada bene
        lung++;     // letto un carattere --> incremento la lunghezza
     
        // riga finita
        if (c == '\n') {
            // lunghezza corrisponde
            if (lung == n){
                buff[lung] = '\0';
                write(1, buff, n);
                f = true;
            }
            lung = 0;
        }
    }

    if (f == false){
        puts("Non c'è nessuna linea con quel numero di caratteri");
    }

    return 0;
}
