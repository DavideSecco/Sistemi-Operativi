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
    int i, k, j;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int nr;
	bool b;
	char linea[255];
	int value;
	int K, X, L;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;

	K = atoi(argv[argc-1]);

	if(K <= 0 || K >= 255){
		printf("ERRORE: problemi con il numero inserito\n");
		exit(2);
	}

	printf("Hai inserito %d file\n", N);
	printf("Il numero inserito è: %d\n", K);

	printf("Inserisci il numero X\n");
	scanf("%d", &X);

	if(X <= 0 || X > K){
		printf("X = %d --> Numero X non corretto\n", X);
		exit(5);
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
			for(j = 0; j < N; j++){
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			/* scorro fino alla linea giusta */
			b = true;
			for(k = 1; k < X; ){
				nr = read(fd, &c, 1);

				if(nr == 0){
					b = false;
					break;
				}
				if(c == '\n')
					k++;
			}

			/* se non sono arrivato alla linea*/
			if(b == false){
				value = -1;
				write(piped[i][1], &value, sizeof(int));
				write(piped[i][1], linea, 1);
				exit(0);
			}

			/* leggo la linea */
			j=0;
			while(1){
				nr = read(fd, &linea[j], 1);
				if(linea[j] == '\n' || nr == 0){
					linea[j] = '\0';
					L = strlen(linea);
					write(piped[i][1], &L, sizeof(int));
					write(piped[i][1], &linea, L*sizeof(char));
					exit(L);
				}
				j++;
			}
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(i = 0; i < N; i++){
		close(piped[i][1]);
	}

	/* lettura dai figli: */
	for(i = 0; i < N; i++){
		read(piped[i][0], &value, sizeof(int));
		if(value == -1){						// se il figlio non è arrivato alla linea
			read(piped[i][0], linea, 1);					// lettura inutile della linea
			printf("\nFiglio %i (PID: %d) \t file: %s\n", i, pid, argv[i+1]);
			printf("Linea non presente\n");
		}
		else{									// se il figlio è arrivato alla linea che voleva leggere				
			read(piped[i][0], linea, value);
			printf("\nFiglio %i (PID: %d) \t file: %s \n", i, pid, argv[i+1]);
			printf("Lunghezza: %d \t Linea: %s\n", value, linea);
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
