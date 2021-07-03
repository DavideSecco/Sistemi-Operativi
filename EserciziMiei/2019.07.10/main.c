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
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char Cz;
	long int pos;
	int nr;


	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	Cz = argv[argc-1][0];

	if(strlen(argv[argc-1]) != 1){
		printf("Non hai inserito un singolo carattere\n");
		exit(2);
	}

	printf("Sono stati inseriti %d file\n", N);
	printf("Il carattere inserito è: %c", Cz);

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione 2*N pipe figli-figli*/
    if ((piped=(pipe_t *)malloc(2*N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo 2*N pipe figli-figli*/
    for (i=0; i < 2*N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), 2*N);
    for (i=0; i < N; i++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for(j = 0; j < 2*N; j++){
				if(j != i)
					close(piped[j][1]);
				if(j != N-1-i)
					close(piped[j][0]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			//prima metà dei figli, quelli che devono partire
			pos = 0;
			cont = 0;

			if(i > N-1){
				nr = read(piped[N-1-i][0], &pos, sizeof(long int));
				if(nr != 0)
					lseek(fd, pos, SEEK_SET);
			}

			while(read(fd, &c, 1)){
				if(c == Cz){
					cont++;
					pos = pos+1;
					write(piped[i][1], &pos, sizeof(long int));
					nr = read(piped[N-1-i][0], &pos, sizeof(long int));

					if(nr != 0)
						lseek(fd, pos, SEEK_SET);
					else
						break;
				}
				else
					pos++;
			}

			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(j = 0; j < 2*N; j++){
		close(piped[j][1]);
		close(piped[j][0]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/


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
