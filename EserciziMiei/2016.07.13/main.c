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
    //int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *pipeFiglioPadre, *pipePadreFiglio;  					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int divisore, dividendo, num;
	int pos;
	int *posizione;
	int *numeri;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
	if((argc % 2) == 0){
		printf("Hai inserito un numero di paramentri dispari\n");
		exit(2);
	}

    N = argc/2;

	printf("Sono stati inseriti %d file\n", N);

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione N pipe */
    /* allocazione pipe figli-padre */
	if ((pipeFiglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL){
		printf("Errore allocazione pipe padre\n");
		exit(3);
	}

	/* allocazione pipe padre-figli */
	if ((pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t))) == NULL)	{
		printf("Errore allocazione pipe padre\n");
		exit(4);
	}

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo N pipe */
    /* creazione pipe */
	for (i = 0; i < N; i++){
		if (pipe(pipeFiglioPadre[i]) < 0) {
			printf("Errore creazione pipe\n");
			exit(5);
		}
	}

	/* creazione di altre N pipe di comunicazione/sincronizzazione con il padre */
	for (i = 0; i < N; i++){
		if (pipe(pipePadreFiglio[i]) < 0){
			printf("Errore creazione pipe\n");
			exit(6);
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
			for (j = 0; j < N; j++)	{
				close(pipeFiglioPadre[j][0]);
				close(pipePadreFiglio[j][1]);
				if (j != i)	{
					close(pipeFiglioPadre[j][1]);
					close(pipePadreFiglio[j][0]);
				}
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[2*i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[2*i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			// 1° FASE:

			read(pipePadreFiglio[i][0], &divisore, sizeof(int));
			sleep(1);
			dividendo = atoi(argv[2*i+2]);

			num = dividendo / divisore;

			for(j = 0; j < divisore; j++){
				read(fd, &c, 1);
				//pos = lseek(fd, 0, SEEK_CUR);
				//printf("Figlio %d posizione %d carattere %c\n", i, pos, c);
				pos = lseek(fd, num-1, SEEK_CUR);
				write(pipeFiglioPadre[i][1], &c, 1);
			}


			exit(divisore);
		}
	}

	/* padre */

	sleep(1);
	printf("Padre con PID: %d\n", getpid());

	
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (i = 0; i < N; i++)	{
		close(pipePadreFiglio[i][0]);
		close(pipeFiglioPadre[i][1]);
	}

	// 1 FASE: chiedo all'utente, per ogni file il divisore e poi invio il divisore
	numeri = malloc(N*sizeof(int));

	for(i = 0; i < N; i++){
		printf("Inserisci il divisore per il file %d: %s\n", i, argv[2*i+1]);
		scanf("%d", &divisore);

		dividendo = atoi(argv[2*i+2]);
		numeri[i] = dividendo / divisore;

		if((dividendo % divisore) != 0){
			printf("Hai inserito un divisore non valido per questo file\n");
			exit(2);
		}

		write(pipePadreFiglio[i][1], &divisore, sizeof(int));
	}
	
	// 2° FASE:
	posizione = malloc(N*sizeof(int));

	for(i = 0; i < N; i++){
		posizione[i] = 1;
	}

	bool finito = false;
	while(finito == false){
		finito = true;
		for(i = 0; i < N; i++){
			int nr = read(pipeFiglioPadre[i][0], &c, 1);
			if(nr != 0){
				finito = false;
				printf("PADRE: Figlio %d file %s posizione %d carattere %c\n", i, argv[2*i+1], posizione[i], c);
				posizione[i] = numeri[i] + posizione[i];
			}
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
