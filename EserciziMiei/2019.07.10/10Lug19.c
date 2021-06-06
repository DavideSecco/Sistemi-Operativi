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
    int i, j;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char cz;
	long int pos;
	char file[255];
	int nr;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	cz = argv[argc-1][0];

	printf("Sono stati inseriti %d nomi di file\n", N);
	printf("il carattere inserito è: %c\n\n", cz);


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    if ((piped=(pipe_t *)malloc(2*N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo N pipe */
    for (i=0; i < 2*N; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), N);
    for (i=0; i < 2*N; i++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            
			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for(j=0; j < 2*N; j++){
				if(j != i)				// figlio legge dalla pipe del suo indice
					close(piped[j][0]);

				if(j != (2*N-1-i)){		
					close(piped[j][1]);	// figlio scrive dalla pipe con indice N-1-i
				}
			}
			
			printf("Figlio %d con pid %d\n", i, getpid());
			sleep(1);

			if(i < N) {
				strcpy(file, argv[i+1]);
				printf("Figlio %d cerco di aprire il file %s\n", i, file);
			}
			else{
				strcpy(file, argv[2*N-i]);
				printf("Figlio %d cerco di aprire il file %s\n", i, file);
			}
			sleep(0.5);
			/* OBBLIGATORIO: apre il file */
            if ((fd = open(file, O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s figlio i\n", argv[1]);
                exit(-1);
            }

			/* codice vero del figlio: */
			cont = 0;
			while(read(piped[i][0], &pos, sizeof(long int))){
				printf("Figlio %d leggo dalla posizione %ld\n", i, pos);
				lseek(fd, pos, SEEK_SET);
				
				while(1){
					nr = read(fd, &c, 1);
					if(nr == 0)
						exit(cont);

					if(c == cz){
						cont++;
						pos= lseek(fd, 0, SEEK_CUR);
						printf("Figlio %d trovato un occorrenza in posizione %ld del carattere %c (passo al figlio %d)\n", i, pos, cz, (2*N-1-i));
						write(piped[2*N-1-i][1], &pos, sizeof(long int));
						break;
					}
				}
			}

			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(j = 0; j < 2*N; j++){
		close(piped[j][0]);			// chiudo tutte le pipe in lettura dal padre
		if(j > N-1){
			close(piped[j][1]);		// le pipe in scrittura tengo aperti solo quelle dei primi N-1 figli
		}
	}


	/* padre manda innesco */
	pos = 0; 					// posizione iniziale dalla quale i figli devono cominciare a leggere
	for(j = 0; j < N; j++){
		write(piped[j][1], &pos, sizeof(long int));
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
