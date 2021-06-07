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
    int pid_nipote;				/* pid per fork */
   	int *pid;							/* array di pid */

    int H;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j;     						/* indici, i per i figli! */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	pipe_t piped_nipote;
	char linea[255];
	int lunghezza;
	int nr;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    H = argc - 1;

	printf("Hai inserito %d file\n", H);


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione H pipe */
    if ((piped=(pipe_t *)malloc(H*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	/* NON È DETTO SERVA: allocazione array per i pid */
    if ((pid=(int *)malloc(H*sizeof(int))) == NULL)    {
    	printf("Errore allocazione pid\n");
    	exit(4);
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo H pipe */
    for (i=0; i < H; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), H);
    for (i=0; i < H; i++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid[i] = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid[i] == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for(j = 0; j < H; j++){
				close(piped[j][0]);		// chiusa la pipe in lettura
				if(j != i)
					close(piped[j][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			if(pipe(piped_nipote) < 0){
				printf("ERRORE creazione pipe nipote-figlio\n");
				exit(5);
			}

			/* eseguo codice figlio e eventuale nipote */
			if ((pid_nipote = fork()) < 0) {
            	printf ("Errore nella fork del nipote \n");
            	exit(1);
        	}
			
			/* NIPOTE */
			if(pid_nipote == 0){
				printf("Creato il nipote %d \n", i);
				
				close(piped[i][1]);		// chiudo la pipe comunicazione figlio-padre

				close(1);						// chiudo lo stdinput
				dup(piped_nipote[1]);			// duplico la pipe sullo stdin
				close(piped_nipote[0]);
				close(piped_nipote[1]);

				execlp("tail", "tail ", "-1", argv[i+1], (char *)0);

				perror("Problemi is wawxuIonw della tail\n");
				exit(-1);
			}
			else{
				close(piped_nipote[1]);
				j = 0;
				while(1){
					nr = read(piped_nipote[0], &linea[j], 1);
					if(linea[j] == '\n' || nr == 0){
						linea[j]='\0';
						lunghezza = strlen(linea);
						write(piped[i][1], &lunghezza, sizeof(int));
						break;
					}
					else 
						j++;
				}
			}


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(i = 0; i < H; i++){
		close(piped[i][1]);
	}

	/* legge dalle pipe i messaggi o manda segnali?*/
	for(i = H-1; i>=0; i--){
		read(piped[i][0], &lunghezza, sizeof(int));
		printf("PID: %d \t lunghezza: %d \t file: %s\n", pid[i], lunghezza, argv[i+1]);
	}


	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(i=0;i < H;i++) {
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
