#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char **argv){
    int N; // variabile per il secondo paramentro
    int pid, pidFiglio;
    int status;
    int exit_f;

    // controllo su singolo parametro:
    if (argc != 2){
        puts("ERRORE numero di paramentri");
        exit(1);
    }

    N = atoi(argv[1]);

    if (N < 1 || N > 254){
        puts("ERRORE: primo parametro");
        exit(2);
    } 

    printf("PID processo padre= %d\n", getpid());
    printf("numero = %d\n", N);

    for (int i=0; i < N; i++) {
        if ((pid=fork())<0){
            printf("All'interazione %d non sono riuscito a creare il processo", i);
            exit (3);
        }
        else if (pid == 0) {
            /* codice figlio */
            printf("Figlio: i = %d \t PID FIGLIO: %d \n", i, getpid());
            exit (i);
        }

    }

    for (int i=0; i < N; i++) {
    /* codice padre */
        if ((pidFiglio=wait(&status)) < 0){
            printf("ERRORE nella wait all'iterazione %d", i);
            exit (3);
        }

        if((status & 0xFF) != 0)
            printf("Figlio arrestato in modo anomalo\n");
            else {
                exit_f = status >> 8;
                exit_f &= 0xFF;
                printf("Padre: \t \t PID FIGLIO: %d \t VALORE: %d\n", pidFiglio, exit_f);
            }
        }
    }
}