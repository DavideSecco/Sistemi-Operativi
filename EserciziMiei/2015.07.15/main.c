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

int token;

void fine(){
	token = 0;
}

void scrivi(){
	token = 1;
}

int main(int argc, char *argv[]){
   	int *pid;							/* array di pid */

    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd, fcreato;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;

	printf("Sono stati inseriti %d file\n", N);

	// creo file di nome merge:
	/* OBBLIGATORIO: apre il file */
	if ((fcreato = creat("Merge", O_WRONLY)) < 0) { 
		printf("Errore nella apertura del file %s\n", "Merge");
		exit(-1);
	}

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	/* NON È DETTO SERVA: allocazione array per i pid */
    if ((pid=(int *)malloc(N*sizeof(int))) == NULL)    {
    	printf("Errore allocazione pid\n");
    	exit(4);
    }

	/* NON È DETTO SERVA:  padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
    bsd_signal(SIGUSR1,scrivi);
    bsd_signal(SIGUSR2,fine);

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < N; i++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid[i] = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid[i] == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for (k = 0; k < N; k++)	{
				close(piped[k][0]);
				if (k != i)
					close(piped[k][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			while(read(fd, &c, 1)){
				pause();

				if(token == 1){
					printf("Figlio %d ha letto e invia al padre '%c'\n", i, c);
					write(piped[i][1], &c, 1);
				}
				else
					exit(-1);
			}


			exit(c);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	bool finito = false;
	while(finito == false){
		for(i = 0; i< N; i++){
			sleep(1);
			kill(pid[i], SIGUSR1);
			int nr = read(piped[i][0], &c, 1);
			
			// se non ho letto nulla, devo far terminare tutti i figli
			if(nr == 0){
				printf("Arrivato alla fine del file: termino tutti i figli\n");
				for(i = 0; i < N; i++){
					kill(pid[i], SIGUSR2);
				}
				finito = true;
				break;
			}
			else if(nr != 0){
				printf("Padre scrive da figlio %d carattere %c\n", i, c);
				write(fcreato, &c, 1);
			}
		}
	}

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(i=0;i < N;i++) {
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
