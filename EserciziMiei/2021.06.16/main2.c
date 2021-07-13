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
    int fd, fcreato;      						/* per open */
    int i, j;     						/* indici, i per i figli! */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char linea[255];
	char **tutteLinee;
	int nr;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;

	printf("Sono stati inseriti %d file\n", N);

	// creo il file fcreato:
	if ((fcreato = creat("Secco", O_WRONLY)) < 0) { 
		printf("Errore nella apertura del file\n");
		exit(-1);
	}

	// alloco spazio per l'array di linee:
	tutteLinee = malloc(N*sizeof(linea));

	for(i = 0; i < N; i++){
		tutteLinee[i] = malloc(255);
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

			/* chiusura per schema a ring: */
			for (j=0; j < N; j++){
				if(j != i)
					close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
				if(j != ((i+1) % N))
					close(piped[j][1]);			// in scrittura lascio aperta la pipe ...
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			while(1){
				j = 0;
				sleep(1);
				read(piped[i][0], tutteLinee, sizeof(tutteLinee)*sizeof(linea));
				while((nr = read(fd, &linea[j], 1))){
					if(linea[j] == '\n'){
						linea[j+1] = '\0';
						
						strcpy(tutteLinee[i], linea);
						printf("Figlio %d mette la linea nell'array: \n", i);
						write(piped[(i+1)%N][1], tutteLinee, sizeof(tutteLinee)*sizeof(linea));
						write(0, tutteLinee, sizeof(tutteLinee)*sizeof(linea));
						j = 0;

						break;		
					}
					else
						j++;
				}

				if(nr == 0){
					break;
				}

				if(i == (N - 1)){
					printf("\nULTIMO FIGLIO SCRIVE TUTTE LE RIGHE SUL FILE:\n");
					printf("1) %s \n2) %s \n", tutteLinee[0], tutteLinee[1]);
					write(fcreato, tutteLinee, sizeof(tutteLinee)*sizeof(linea));
				}
			}

			exit(j+1);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
	for(i = 1; i < N; i++){
		close(piped[i][0]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		close(piped[i][1]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	write(piped[0][1], tutteLinee, N*255);		// lancio l'innesco
	// sleep(1);
	close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura

	// NON chiudo la pipe in lettura!

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
