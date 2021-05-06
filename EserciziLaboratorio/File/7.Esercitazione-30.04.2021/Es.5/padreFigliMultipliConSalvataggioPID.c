#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int mia_random(int n){
    int casuale;
    casuale = rand() % n;
    return casuale;
}

int main (int argc, char **argv){
    int N; // variabile per il secondo paramentro
    int pidFiglio;
    int status;
    int exit_f;

    // controllo su singolo parametro:
    if (argc != 2){
        puts("ERRORE numero di paramentri");
        exit(1);
    }

    N = atoi(argv[1]);

    if (N < 1 || N > 155){
        puts("ERRORE: primo parametro");
        exit(2);
    } 

    int *pid = malloc(N * sizeof(int));

    printf("PID processo padre= %d\n", getpid());
    printf("numero = %d\n", N);

    for (int i=0; i < N; i++) {
        if (((pid[i]) = fork()) < 0){
            printf("All'interazione %d non sono riuscito a creare il processo", i);
            exit (3);
        }
        else if (pid[i] == 0) {
            /* codice figlio */
            int num_rand=mia_random(i + 100);
            printf("PID FIGLIO: %d \t i = %d \t RANDOM: %d \n", getpid(), i, num_rand);
            exit (num_rand);
        }
    }

    for (int i=0; i < N; i++) {
    /* codice padre */
        if ((pidFiglio=wait(&status)) < 0){
            printf("ERRORE nella wait all'iterazione %d\n", i);
            exit (3);
        }

        if((status & 0xFF) != 0)
            printf("Figlio arrestato in modo anomalo\n");
         else {
            exit_f = status >> 8;
            exit_f &= 0xFF;
            for (int j=0; j < N; j++){
                if(pidFiglio == pid[j])
                    printf("2PID FIGLIO: %d \t i: %d \t RANDOM: %d\n", pidFiglio, j, exit_f);
            }
                
        }
    }
}
