 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <fcntl.h>
 #include <unistd.h>

 int main(int argc, char **argv) {
    int fd=0;
    char c;     // memorizzo il carattere letto

    if (argc != 3){
        puts("hai inserito un numero di parametri sbagliato");
        exit (1);
    }

    // controllo sul primo paramentro:
    if ((fd = open(argv[1], O_RDONLY)) < 0 ) {
        puts("errore apertura del file");
        exit(2);
    }

    // controllo del secondo parametro
    int n=atoi(argv[2]);
    if(n<1) {
        puts("hai inserito un numero non strettamente positivo oppure un carattere non valido");
        exit (3);
    }
    
    int lung = lseek(fd, 0, SEEK_END);     // numero caratteri del file

    // ciclo
    int salto = atoi(&argv[2][0]);
    int pos = lseek(fd, salto, SEEK_SET);

    while(pos < lung) {
        read(fd, &c, 1);
        lseek(fd, -1, SEEK_CUR);                                // Torno indietro di una posizione, perchè la lettura ha sposta di 1 il pointer
        printf("Posizione %d --> carattere: %c\n", pos, c);     // stampo
        pos = lseek(fd, salto, SEEK_CUR);                       // porto avanti il file pointer
    }
 }
