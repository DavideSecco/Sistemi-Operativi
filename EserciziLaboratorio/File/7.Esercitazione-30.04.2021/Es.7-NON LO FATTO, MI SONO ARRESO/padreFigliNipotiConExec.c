#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>


int main(int argc, char **argv){
    int pid, pid_nipote, pid_figlio;
    int exit_f, status;

    int N = argc - 1;
    int *Ff = malloc(N*sizeof(int));
    int Fout;

    /* controllo sul numero di parametri: */ 
    if(argc < 4){
        puts("Hai inserito pochi parametri");
        exit (1);
    }

    /* constrollo sui file */
    for (int i = 0; i < N; i++){
        if((Ff[i]=open(argv[i+1], O_RDONLY)) < 0){
            printf("Errore nell'apertura del file %s", argv[i+1]);
            exit(3);
        }
        close(Ff[i]);
    }

    /* ciclo per figli e nipoti */
    for(int i = 0; i < N; i++){
        if((pid = fork()) < 0){
            puts("Errore nella fork");
            exit(2);
        }

        else if (pid == 0){          /* codice del figlio */
            printf("Creato Figlio PID: %d\n", getpid());
            char Fout_name[255];
            strcpy(Fout_name, argv[i+1]);  
            strcat(Fout_name, ".sort");

            Fout=creat(Fout_name, O_WRONLY);
            /* creo il processo nipote */
            if((pid = fork()) < 0){
                puts("Errore nella fork");
                exit(2);
            }

            /* processo nipote */
            if(pid == 0){
                // devo chiudere lo stdout per ridirigere sull'altro file
                close(0);       
                /* creo il nuovo file con il nome giusto */
                puts("Nipote creato\n");

                if (open(argv[i+1], O_RDONLY) < 0){
                    printf("Errore: file %s NON ESISTE", argv[i+1]);
                    exit(-1);
                }

                close(1);

                if(open(FOut, O_WRONLY) < 0){
                    printf("Errore: file %s NOon si riesce ad aprire in lettura\n", Fout);
                    exit(-1);
                }
                
                execlp("sort", "sort", (char *)0 );
                // se qualcosa va storto:
                puts("il nipote ha dei problemi\n");
                exit(-1);
            }
            
            else if((pid_nipote = wait(&status))< 0){
                puts("Errore: problemi con l'attesa del nipote nipote\n");
                exit (5);
            }
        
        }

        for (int i = 0; i < N; i++){
            if((pid_figlio = wait(&status)) < 0){
                puts("Errore: problemi con il figlio\n");
                printf("pid_figlio = %d\n", pid_figlio);
                exit (5);
            }

            if((status & 0xFF) != 0)
            printf("Figlio arrestato in modo anomalo\n");
            else {
                exit_f = status >> 8;
                exit_f &= 0xFF;
                printf("PID FIGLIO: %d \t Return: %d\n", pid_figlio, exit_f);
            }
        }


    }
}
