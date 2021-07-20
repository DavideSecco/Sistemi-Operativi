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
    int pid, pidnipote;      						/* pid per fork */
   	int N;   							/* numero di caratteri e quindi numero di processi */
    int fd, fdnipote;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *pipedfiglio, *pipednipote;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	long int trasf;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;

	printf("Sono stati inseriti %d file\n", N);

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    if ((pipedfiglio=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	if ((pipednipote=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo N pipe */
    for (i=0; i < N; i++) {
        if (pipe(pipedfiglio[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	for (i=0; i < N; i++) {
        if (pipe(pipednipote[i]) < 0)        {
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
				close(pipedfiglio[k][0]);
				if (k != i)
					close(pipedfiglio[k][1]);
			}

			

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDWR)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			if ((pidnipote = fork()) < 0) {
				printf ("Errore nella fork\n");
				exit(1);
			}

			if(pidnipote == 0){
				// chiude la pipe del figlio lasciata aperta
				close(pipedfiglio[i][1]);

				for (k = 0; k < N; k++)	{
					close(pipednipote[k][0]);
					if (k != i)
						close(pipednipote[k][1]);
				}

				/* OBBLIGATORIO: apre il file */
				if ((fdnipote = open(argv[i+1], O_RDWR)) < 0) { 
					printf("Errore nella apertura del file %s\n", argv[i+1]);
					exit(-1);
				}

				trasf = 0;
				while(read(fdnipote, &c, 1)){
					if(islower(c)){

						char c2 = toupper(c);
						printf("Trovato un carattere minuscolo il nipote: %c lo sostituisco con %c \n", c, c2);

						trasf++;
						lseek(fdnipote, -1, SEEK_CUR);
						write(fdnipote, &c2, 1);
					}
				}

				write(pipednipote[i][1], &trasf, sizeof(long int));

				exit(trasf/256);



			}

			/*NIPOTE SI CONCLUDE */

			for (k = 0; k < N; k++)	{
				close(pipednipote[k][0]);
				close(pipednipote[k][1]);
			}
			
			trasf = 0;
			while(read(fd, &c, 1)){
				if(isdigit(c)){
					char c2 = ' ';
					printf("Trovato un carattere numerico il figlio: %c lo sostituisco con '%c' \n", c, c2);
					trasf++;

					lseek(fd, -1, SEEK_CUR);
					write(fd, &c2, 1);
				}
			}

			write(pipedfiglio[i][1], &trasf, sizeof(long int));

			exit(trasf/256);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(j = 0; j < N; j++){
		close(pipedfiglio[j][1]);
		close(pipednipote[j][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	for(i = 0; i < N; i++){
		read(pipedfiglio[i][0], &trasf, sizeof(long int));
		printf("\nFiglio %d trasformazioni: %ld\n", i, trasf);
		read(pipednipote[i][0], &trasf, sizeof(long int));
		printf("Nipote %d trasformazioni: %ld\n", i, trasf);
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
