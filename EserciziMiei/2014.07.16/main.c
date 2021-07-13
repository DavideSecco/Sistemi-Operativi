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
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char *primiChar;
	char linea[255];
	int X;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;

	X = atoi(argv[argc-1]);

	printf("Sono stati inseriti %d file che hanno lunghezza in linee %d\n", N, X);

	// alloco lo spazio per il vettore primiChar
	primiChar = malloc(N*sizeof(char));

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
				if (k != i)
					close(piped[k][1]);
				if (i == 0 || k != i - 1)
					close(piped[k][0]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			for(k = 0; k < X; k++){
				if( i != 0)
					read(piped[i-1][0], primiChar, N*sizeof(primiChar));
				
				j = 0;
				while(read(fd, &linea[j], 1)){
					if(linea[j] == '\n'){
						// invio al figlio successivo l'array aggiornato:
						
						primiChar[i] = linea[0];
						printf("Figlio %d riga %d invia primo carattere: %c\n", i, k, primiChar[i]);
						write(piped[i][1], primiChar, N*sizeof(primiChar));
						break;
					}
					else 
						j++;
				}
			}


			exit(linea[0]);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
		if (k != N - 1)	{
			close(piped[k][0]);
		}
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	for(k = 0; k < X; k++){
		sleep(1);
		int nr = read(piped[N-1][0], primiChar, N*sizeof(primiChar));

		printf("nr = %d\n", nr);

		printf("\nPadre: Linea %d\n", k);

		for(i = 0; i < N; i++){
			printf("indice: %d carattere %c file %s\n", i, primiChar[i], argv[i+1]);
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
