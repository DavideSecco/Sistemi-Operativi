#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int fd1, fd2;
    int piped[2];

    if (argc != 2) { 
        puts("Errore nel numero di parametri"); 
        exit(1); 
    }

    if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
        puts("Errore in apertura file");
        exit(2); }
    else
        printf("Valore di fd1 = %d\n", fd1);
    
    if ((fd2 = open(argv[1], O_RDONLY)) < 0) { 
        puts("Errore in apertura file");
        exit(2); 
    }
    else
        printf("Valore di fd2 = %d\n", fd2);
    
    close(fd1);
    /* close(fd2); */

    if((pipe(piped)) < 0){
        puts("Errore nella creazione della pipe\n");
        exit (3);
    }

    printf("Valore della pipe %d\n", piped[0]);
    printf("Valore della pipe %d\n", piped[1]);
    
    return 0; 
}