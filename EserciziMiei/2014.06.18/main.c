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

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char nomefile[255];
	int X;
	int *fcreato;
	char linea[255];

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	X = atoi(argv[argc-1]);

	printf("Sono stati inseriti %d file e la loro lunghezza è: %d\n", N, X);

	fcreato = malloc(X*sizeof(int));

	// creo tutti i file in /tmp:

	for(i = 0; i < X; i++){
		sprintf(nomefile, "/tmp/%d", i);
		
		if ((fcreato[i] = creat(nomefile, O_WRONLY)) < 0) { 
			printf("Errore nella creazione del file %s\n", nomefile);
			exit(-1);
		}

		printf("Creato il file: %s\n", nomefile);
	}
	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

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
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
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

			for(j = 0; j < X; j++){
				k = 0;
				while(read(fd, &linea[k], 1)){
					if(linea[k] == '\n'){
						k++;
						linea[k]='\0';
						// printf("Figlio %d riga %d: %s\n", i, j, linea);
						// invio al figlio la lunghezza della linea: 
						write(piped[i][1], &k, sizeof(int));

						// invio al figlio la linea:
						write(piped[i][1], &linea, k);
						break;
					}
					else
						k++;
				}
			}


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	for(k = 0; k < X; k++){
		for (i = 0; i < N; i++){
			// leggo dal figlio la lunghezza della linea:
			read(piped[i][0], &cont, sizeof(int));
		
			// leggo dal figlio la linea:
			read(piped[i][0], &linea, cont);

			printf("Padre scrive su file Da figlio %d riga %d: %s\n", i, k, linea);
			write(fcreato[k], &linea, cont);		
		}

		sleep(1);
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
