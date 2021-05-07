#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define Max_SIZE 512

int main (int argc, char **argv) {
    int pid, j, piped[2];               /* pid per fork, j per indice, piped per pipe */
    char mess[Max_SIZE];				/* ATTENZIONE: ora il messaggio non ha lunghezza fissa faccio una malloc dopo */
    int pidFiglio, status, ritorno;     /* per wait padre */
	int size;
	int nread;

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
        close (piped [0]);        /* il figlio CHIUDE il lato di lettura */

        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            printf("Errore in apertura file %s\n", argv[1]);
            exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindiidentificato come errore */
        }
    
        printf("Figlio %d sta per iniziare a scrivere una serie di messaggi, \n", getpid());
        j=0; 			/* il figlio inizializza la sua variabile j per contare i messaggi che ha mandato al padre */
		int i=0;

        while ((nread = read(fd, &mess[i], 1)) > 0){ /* il contenuto del file e' tale che in mess ci saranno 4 caratteri e il terminatore di linea */
			// riga finita
			if (mess[i] == '\n'){
				mess[i]='\0';
				int size = strlen(mess) + 1;

				// invio la lunghezza della linea che ho letto
            	write (piped[1], &size, sizeof(size));

				// invio il messaggio al padre
				write(piped[1], mess, size);
				j++;	// aumento il valore delle linee
				i=0;	// facico ripartire il vettore
			}
			else
				i++;           
		}
        printf("Figlio %d scritto %d messaggi sulla pipe\n", getpid(), j);
        exit(0);
    }

    /* PADRE */
    close (piped [1]); /* il padre CHIUDE il lato di scrittura */
    printf("Padre %d sta per iniziare a leggere i messaggi dalla pipe\n", getpid());
    
    j=0;                /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio */
    while (read ( piped[0], &size, sizeof(int))) {
        read(piped[0], mess, size);
		printf ("%d: %s\n", j, mess);			/* dato che il figlio gli ha inviato delle stringhe, il padre le puo' scrivere direttamente con una printf */
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
