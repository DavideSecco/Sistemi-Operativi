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

void scrivi(){
	;
}

int main(int argc, char *argv[]){
   	int *pid;							/* array di pid */

    int Q;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int q, j, k;     						/* indici, q per q figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char F[255];
	int L;
	int nr;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 5)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }

	
    
	strcpy(F, argv[1]);
	L = atoi(argv[2]);

	if(L <= 0){
		printf("Hai inserito un numero non valido\n");
		exit(2);
	}
	
	Q = argc - 3;
	
	/* NON È DETTO SERVA: allocazione array per i pid */
    if ((pid=(int *)malloc(Q*sizeof(int))) == NULL)    {
    	printf("Errore allocazione pid\n");
    	exit(4);
    }

	printf("Sono stati inseriti %d caratteri\n", Q);

	/* NON È DETTO SERVA:  padre aggancia le due funzioni (scrivi e salta) che useranno q figli alla ricezione dei segnali inviati dal padre */
    bsd_signal(SIGUSR1,scrivi);

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), Q);
    for (q=0; q < Q; q++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid[q] = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid[q] == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			char carattere = argv[q+3][0];
			printf("Figlio %d carattere: %c\n", q, carattere);
			cont = 0;
			for(k = 0; k < L; k++){				
				while((nr = read(fd, &c, 1))){
					if(c == '\n' || nr == 0){
						pause();

						printf("%d occorrenze del carattere '%c'\n", cont, carattere);
						cont = 0;
					}
					else if(c == carattere){
						cont++;
					}
				}
			}


			exit(cont);
		}
	}

	/* padre */

	sleep(1);
	printf("Padre con PID: %d\n", getpid());
    

	/* legge dalle pipe q messaggi o manda segnali?*/

	for(k = 0; k < L; k++){
		printf("Linea %d\n", k+1);

		sleep(1);

		for(j = 0; j < Q; j++){
			kill(pid[j], SIGUSR1);
			sleep(1);
		}
	}

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
