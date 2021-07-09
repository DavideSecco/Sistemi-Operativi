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
    int i, j;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *pipeFiglioPadre;			/* array dinamico di pipe */
    pipe_t *pipePadreFiglio;
	int pidFiglio, status, ritorno;    /* variabili per wait*/
	char Cx;
	long int pos;
	char cs;
	int nr;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	 Cx = argv[argc-1][0];

	if(strlen(argv[argc-1]) > 1){
		printf("Non hai inserito un singolo carattere\n");
		exit(2);
	}

	printf("Sono stati inseriti %d file \n" , N);

	//ALLOCAZIONE MEMORIA MALLOC

   /* allocazione pipe figli-padre */
	if ((pipeFiglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)	{
		printf("Errore allocazione pipe padre\n");
		exit(3);
	}

	/* allocazione pipe padre-figli */
	if ((pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)	{
		printf("Errore allocazione pipe padre\n");
		exit(4);
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
    /* creazione pipe */
	for (i = 0; i < N; i++)
		if (pipe(pipeFiglioPadre[i]) < 0){
			printf("Errore creazione pipe\n");
			exit(5);
		}

	/* creazione di altre N pipe di comunicazione/sincronizzazione con il padre */
	for (i = 0; i < N; i++)
		if (pipe(pipePadreFiglio[i]) < 0){
			printf("Errore creazione pipe\n");
			exit(6);
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
			/* chiusura pipes inutilizzate */
			for (j = 0; j < N; j++) {
				close(pipeFiglioPadre[j][0]);
				close(pipePadreFiglio[j][1]);
				if (j != i)	{
					close(pipeFiglioPadre[j][1]);
					close(pipePadreFiglio[j][0]);
				}
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDWR)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */

			while(read(fd, &c, 1)){
				if(c == Cx){
					pos = lseek(fd, 0, SEEK_CUR);

					// scrivo al padre la pos del carattere trovato
					write(pipeFiglioPadre[i][1], &pos, sizeof(long int));
					
					pause();

					// se il carattere è valido, leggo dalla pipe e poi sostituisco il valore
					if(token){
						// leggo con che carattere devo sostiutire
						read(pipePadreFiglio[i][0], &cs, sizeof(char));

						// sostituisco il carattere
						lseek(fd, -1, SEEK_CUR);
						write(fd, &cs, 1);
					}

					
				}
			}


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	/* chiusura pipe */
	for (i = 0; i < N; i++)	{
		close(pipePadreFiglio[i][0]);
		close(pipeFiglioPadre[i][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	bool finito = false;

	while(finito == false){
		finito = true;
		for(i = 0; i < N; i++){
			nr = read(pipeFiglioPadre[i][0], &pos, sizeof(long int));
			if(nr != 0){
				finito = false;
				printf("Figlio %d file: %s posizione: %ld\n", i, argv[i+1], pos);
				printf("Con che carattere vuoi sostituire?\n");
				scanf("%c", &cs);

				if(cs != '\n'){
					kill(pid[i], SIGUSR1);
					write(pipePadreFiglio[i][1], &cs, 1);
				}
				else
					kill(pid[i], SIGUSR2);
			}
			sleep(1);
			
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
