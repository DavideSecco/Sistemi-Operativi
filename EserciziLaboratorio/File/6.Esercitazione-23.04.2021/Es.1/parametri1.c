#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if (argc == 1) {
        puts("Non hai inserito nemmeno un paramentro");
        exit (1);
    }

    printf("Nome dell'eseguibile: %s\n", argv[0]);
   
    for (int i = 1; i<argc; i++){
        printf("Paramentro %d: %s \n", i, argv[i]);
    }

}
