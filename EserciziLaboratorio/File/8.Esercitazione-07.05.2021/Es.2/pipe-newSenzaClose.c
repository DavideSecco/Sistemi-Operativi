#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MSGSIZE 5

int main (int argc, char **argv) {
    int pid, j, piped[2];               /* pid per fork, j per indice, piped per pipe */
    char mess[MSGSIZE];                 /* array usato dal figlio per inviare stringa al padre */ 
    char inpbuf [MSGSIZE];              /* array usato dal padre per ricevere stringa inviata dal figlio: N.B: si poteva usare sempre mess,
                                        tanto il padre e il figlio agiscono sulla loro copia delle variabili! */
    int pidFiglio, status, ritorno;     /* per wait padre */

    if (argc != 2) {
        printf("Numero dei parametri errato %d: ci vuole un singolo parametro\n", argc);
        exit(1);
    }
    
    if (pipe (piped) < 0 ) {            /* si crea una pipe */
        printf("Errore creazione pipe\n");
        exit (2);
    }
    if ((pid = fork()) < 0)     {
        printf("Errore creazione figlio\n");
        exit (3);
    }

     /* figlio */
    if (pid == 0) {
        int fd;
        /*close (piped [0]); */        /* il figlio CHIUDE il lato di lettura */

        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            printf("Errore in apertura file %s\n", argv[1]);
            exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindiidentificato come errore */
        }
    
        printf("Figlio %d sta per iniziare a scrivere una serie di messaggi, ognuno di lunghezza %d, sulla pipe dopo averli letti dal file passato come parametro\n", getpid(), MSGSIZE);
        j=0; /* il figlio inizializza la sua variabile j per contare i messaggi che ha mandato al padre */

        while (read(fd, mess, MSGSIZE)){ /* il contenuto del file e' tale che in mess ci saranno 4 caratteri e il terminatore di linea */
            mess[MSGSIZE-1]='\0';			 /* il padre ha concordato con il figlio che gli mandera' solo stringhe e quindi dobbiamo sostituire il terminatore di linea con il terminatore di stringa */
            write (piped[1], mess, MSGSIZE);
            j++;
		}
        printf("Figlio %d scritto %d messaggi sulla pipe\n", getpid(), j);
        exit(0);
    }

    /* PADRE */
    /* close (piped [1]);*/ /* il padre CHIUDE il lato di scrittura */
    printf("Padre %d sta per iniziare a leggere i messaggi dalla pipe\n", getpid());
    
    j=0;                /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio */
    while (read ( piped[0], inpbuf, MSGSIZE)) {
        printf ("%d: %s\n", j, inpbuf);			/* dato che il figlio gli ha inviato delle stringhe, il padre le puo' scrivere direttamente con una printf */
        j++;
    }

    printf("Padre %d letto %d messaggi dalla pipe\n", getpid(), j);
    
    pidFiglio = wait(&status);      /* padre aspetta il figlio */
    
    if (pidFiglio < 0) {
        printf("Errore wait\n");
        exit(5);
    }
    
    if ((status & 0xFF) != 0)
        printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
    else {
        ritorno=(int)((status >> 8) & 0xFF);
        printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
    }

    exit (0);
}
