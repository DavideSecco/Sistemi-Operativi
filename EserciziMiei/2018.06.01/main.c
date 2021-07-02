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
   	int *pid;							/* array di pid */

    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int K, X;
	int L; 								// per la lung della linea
	char linea[255];

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	K = atoi(argv[argc-1]);
	
	if(K <= 0){
		printf("Non è stato inserito un numero intero positivo\n");
		exit(2);
	}

	printf("Sono stati inseriti %d file\n", N);
	printf("Numero inserito: %d\n", K);

	printf("Inserisci un numero intero positivo minore di K\n");
	scanf("%d", &X);
	if(X > K || X < 1){
		printf("Numero inserito non valido\n");
		exit(3);
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

			/* eseguo codice figlio */
			 
			// scorro le linee:
			cont = 1;
			c = '0';
			bool trovata = false;
			do {
				if(cont == X){
					printf("TROVATA\n");
					trovata = true;
					break;
				}
					
				if(c == '\n'){
					cont++;
				}

				if(cont == X){
					trovata = true;
					break;
				}
				//printf("Letto %c\n", c);
			}while(read(fd, &c, 1));

			// sono arrivato alla linea giusta: leggo la linea
			L = 0;
			if(trovata == true){
				while(read(fd, &linea[L], 1)){
					if(linea[L] == '\n'){
						linea[L] = '\0';
						break;
					}
					L++;
				}
			}

			if(trovata == true){
				printf("Linea trovata: %s", linea);
			}
			write(piped[i][1], &L, sizeof(int));
			write(piped[i][1], linea, L);

			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi*/
	for (i = 0; i<N; i++){
		read(piped[i][0], &L, sizeof(int));
		read(piped[i][0], &linea, L);

		printf("\nPID: %d, file: %s \n", pid[i], argv[i+1]);
		if(L != 0){
			printf("Lunghezza: %d, Linea: %s\n\n", L, linea);
		}
		else
			printf("Linea non esistente\n\n");

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
