#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv){
    int pidpadre, pidFiglio, pid;
    int casuale;
    int status;
    int exit_f;

    pidpadre = getpid();
    printf("PID PADRE: %d\n", pidpadre);

    if((pid = fork()) < 0){
        /*fork fallita*/
        puts("Non sono riuscito a creare una processo figlio");
        exit(1);
    }

    /*Esecuzione del figlio*/
    if (pid == 0){
        printf("Esecuzione del figlio:\n");
        printf("PID FIGLIO = %d\n", getpid());
        printf("PID PADRE = %d\n", getppid());
        casuale = rand() % 100;
        printf("Numero casuale: %d\n", casuale);
        printf("fine esecuzione figlio\n\n");
        exit (casuale);                     
    }

    /*esecuzione del padre*/
    if((pidFiglio = wait(&status)) < 0){
        puts("ERRORE nella wait");
        exit(2);
    }

    printf("Esecuzione del padre:\n");
    printf("PID del figlio ritornato: %d\n", pidFiglio);

    if((status & 0xFF) != 0)
        puts("Figlio terminato in modo involontario, cioè anomalo\n");
    else {
        // isoliamo il byte alto
        exit_f = status >> 8;
        exit_f = exit_f & 0xFF;
        printf("Valore di uscita del figlio = %d\n", exit_f);
    }

    exit(0);
}
