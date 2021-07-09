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

int token;

void salta(){
	token = 0;
}

void scrivi(){
	token = 1;
}

int main(int argc, char *argv[]){
   	int *pid;							/* array di pid */

    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int nr;
	bool finito;
	char CZ;
	long int pos, posmax;
	int idmax;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	CZ = argv[1][0];

	if(strlen(argv[1])>1){
		printf("Non hai inserito un carattere\n");
		exit(2);
	}

	printf("Sono stati inseriti %d file\n", N);

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

	/* NON È DETTO SERVA:  padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
    bsd_signal(SIGUSR1,scrivi);
    bsd_signal(SIGUSR2,salta);

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
			for (k = 0; k < N; k++)			{
				close(piped[k][0]);
				if (k != i)
					close(piped[k][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+2], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+2]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			cont = 0;
			while(read(fd, &c, 1)){
				if(c == CZ){
					pos = lseek(fd, 0, SEEK_CUR);
					printf("FIglio %d ha trovato il carattere in pos %ld\n", i, pos);
					write(piped[i][1], &pos, sizeof(long int));
					cont++;

					pause();

					if(token){
						printf("Carattere %c Figlio %d PID %d posizione %ld file %s\n\n", c, i, getpid(), pos, argv[i+2]);
					}
				}
			}


			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}

	finito = false;

	while(finito == false){
		finito = true;
		posmax = 0;
		pos = 0; 

		for(i = 0; i < N; i++){
			nr = read(piped[i][0], &pos, sizeof(long int));

			if(nr != 0)
				finito = false;

			if(pos > posmax){
				posmax=pos;
				idmax = i;
			}
		}

		for(i = 0; i < N; i++){
			if (i == idmax)			
				kill(pid[i], SIGUSR1);
			else
				kill(pid[i], SIGUSR2);
		}
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
