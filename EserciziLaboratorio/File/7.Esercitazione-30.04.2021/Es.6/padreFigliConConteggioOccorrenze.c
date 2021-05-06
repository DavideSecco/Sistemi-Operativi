#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, char **argv){
    int pid, pidFiglio, status;
    int exit_f;
    char c;                 // per i caratteri letti man mano

    if (argc < 4){
        puts("Errore nel numero di parametri");
        exit(1);
    }

    int N = argc - 2;               // numero file
    char carattere = *argv[N+1];    // salvo il carattere

    printf("Numero di file: %d\n", N);
    printf("Carattere: %c\n", carattere);
    int *Ff = malloc(N*sizeof(int));
    
    /* controllo che i primi N parametri siano file */
    for(int i = 1; i < N; i++){
        if((Ff[i] = open(argv[i], O_RDONLY)) < 0){
            printf("Non sono riuscito ad aprire %d file", i);
            exit(2);
        }
    }

    /* controllo sull'ultimo paramentro */
    int size=sizeof(argv[N]);
    if (size != 8) {
        puts("il terzo paramentro deve essere un carattere");
    }

    for(int i = 1; i < N; i++){
        if ((pid = fork()) < 0){
            puts("Errore nella fork");
            exit(5);
        }
        /* codice figlio*/
        if(pid == 0){
            int tot=0;
            while(read(Ff[i], &c, 1)){
                if (carattere == c)
                    tot++;
            }
            // se fallisce:
            exit(tot);
        }
    }

    for(int i = 1; i < N; i++){
    /* padre deve aspettare che i figli finiscano */
        if ((pidFiglio = wait(&status)) < 0){
            printf("ERRORE nella wait all'iterazione %d\n", i);
            exit (3);
        }

        if((status & 0xFF) != 0)
            printf("Figlio arrestato in modo anomalo\n");
        else {
            exit_f = status >> 8;
            exit_f &= 0xFF;
            printf("PID FIGLIO: %d \t Return: %d\n", pidFiglio, exit_f);
                
        }
    }
}
