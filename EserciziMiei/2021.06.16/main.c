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

typedef char linea[250];

typedef struct{
    			/*  */
	 			/*  */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd, fcreato;   						/* per open */
    int n, j, k;     						/* indici, n per n figli! */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	linea *tutteLinee;
	int nlinea;
	char ok;
	char lin[255];
	int nr;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;

	printf("Sono stati inseriti %d file\n", N);

	/* OBBLIGATORIO: creo il file */
	if ((fcreato = creat("Secco", O_WRONLY)) < 0) { 
		printf("Errore nella creazione del file \n");
		exit(-1);
	}

	// alloco spazio per tutte le tutteLinee:
	if ((tutteLinee=(linea *)malloc(N*sizeof(linea))) == NULL)    {
    	printf("Errore allocazione array per le tutteLinee\n");
    	exit(3); 
    }

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo N pipe */
    for (n=0; n < N; n++) {
        if (pipe(piped[n]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (n=0; n < N; n++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", n, getpid());

			/* chiusura per schema a ring: */
			for (j=0; j < N; j++){
				if(j != n)
					close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
				if(j != ((n+1) % N))
					close(piped[j][1]);			// in scrittura lascio aperta la pipe ...
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[n+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[n+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			nlinea = 0;
			

			for(nlinea = 0; ; nlinea = nlinea + 1 ){
				if(n == 0 && nlinea == 0)
					read(piped[n][0], &ok, 1);								// primo innesco del padre
				else
					read(piped[n][0], tutteLinee, sizeof(tutteLinee)*sizeof(linea));				// dal figlio precendente

				// leggo la linea:
				j = 0;
				while(nr = (read(fd, &lin[j], 1))){
					if(lin[j] == '\n'){
						lin[j+1] = '\0';
						
						strcpy(tutteLinee[n], lin);
						printf("Figlio %d riga %d: '%s'\n", n, nlinea, tutteLinee[n]);
						write(piped[(n+1)%N][1], tutteLinee, sizeof(tutteLinee)*sizeof(linea)); 					// scrivo sulla pipe "successiva" al figlio
						break;
					}
					else 
						j++;
				}

				if(nr == 0)
					break;

				if(n == N - 1){
					printf("\nScrivo su file questa riga di tutti i file che contengono: \n");
					for(j = 0; j < N; j++){
						printf("%s\n", tutteLinee[j]);
					}
					write(fcreato, tutteLinee, sizeof(tutteLinee)*sizeof(linea));
				}
			}
			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa */
	for(n = 1; n < N; n++){
		close(piped[n][0]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		close(piped[n][1]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}

	write(piped[0][1], &ok, sizeof(char));		// lancio l'innesco
	// sleep(1);
	close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura
	
	/* legge dalle pipe n messaggi o manda segnali?*/

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(n=0;n < N;n++) {
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
