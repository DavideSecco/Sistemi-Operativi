#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    
    if (argc != 4) {
        puts("Hai inserito un numero di paramentri errato");
        exit (1);
    }

    // considerato il nome di un file? --> devo controllare che il file esista?

    // controllo sul secondo paramentro
    if (*argv[2] <= 0) {
        puts("il secondo paramentro deve essere strettamente positivo");
        exit (2);
    }

    // controllo sul terzo paramentro sfruttando la dimensioni di un carattere
    int size=sizeof(argv[3]);
    if (size != 8) {
        puts("il terzo paramentro deve essere un carattere");
    }

    printf("il nome dell'eseguibile è %s\n", argv[0]);

    for (int i = 1; i < argc; i++){
        printf("Paramentro %d: %s\n", i, argv[i]);
    }


}
