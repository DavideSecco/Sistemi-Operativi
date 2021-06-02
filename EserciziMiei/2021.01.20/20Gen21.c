#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

typedef int pipe_t[2];

typedef struct{
    			/*  */
	 			/*  */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	int Q;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int q, j, k;     						/* indici, q per q figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char ok = 'y';
	int pos;
	int nread;

	/* numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    Q = argc - 1;		// numero file


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione Q pipe */
    if ((piped=(pipe_t *)malloc(Q*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo Q pipe */
    for (q=0; q < Q; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), Q);
   
    for (q=0; q < Q; q++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for (j=0; j < Q; j++){
				if(j != q)
					close(piped[j][0]);
				if(j != ((q+1) % Q))
					close(piped[j][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[q+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			
			k=0;
			pos = q + k * Q;		// setto la posizione iniziale
			lseek(fd, pos, SEEK_SET);
			
			while(read(fd, &c, sizeof(char))){
				read(piped[q][0], &ok, 1);							/* aspetto il segnale dal figlio precedente (o padre) che da il via */
				printf("Figlio %d (pid = %d) posizione %d carattere: %c \n", q, getpid(), pos, c);
				k++;
				pos = q + k * Q;
				
				lseek(fd, pos, SEEK_SET);				
				write(piped[(q+1)%Q][1], &ok, 1); 									// invio segnale a figlio successivo/padre
			}
			
			exit(q);
		}
	}

	/* padre */
	printf("Sono il padre (pid: %d)\n", getpid());
	   
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(q = 1; q < Q; q++){
		close(piped[q][0]);
		close(piped[q][1]);
	}

	write(piped[0][1], &ok, sizeof(char));
	sleep(1);
	close(piped[0][1]);

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(q=0;q < Q;q++) {
    	pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore wait\n");
	        exit(9);
    	}
            
        if ((status & 0xFF) != 0)
            printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        else{
            ritorno=(int)((status >> 8) & 0xFF);
            printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidFiglio, ritorno);
        }
    }
    exit(0);
}
