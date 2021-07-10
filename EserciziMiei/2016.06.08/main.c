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

int mia_random(int n){
	int casuale;
	casuale = rand() % n;
	return casuale;
}

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd, Fcreato;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t *pipePadreFiglio, *pipeFiglioPadre;				/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int H;
	int *lunghezze;
	char linea[255];
	int index;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 6)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	H = atoi(argv[argc-1]);

	if(H <= 0 || H >= 255){
		printf("Problemi con il carattere numerico %d\n", H);
		exit(2);
	}

	printf("Sono stati inseriti %d file e il carattere %d\n", N, H);

	/* OBBLIGATORIO: apre il file */
	if ((Fcreato = creat("/tmp/creato", O_WRONLY)) < 0) { 
		printf("Errore nella CREAZIONE del file %s\n", "/tmp/creato");
		exit(-1);
	}


	//ALLOCAZIONE MEMORIA MALLOC

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
			/* chiusura pipes inutilizzate */
			for (j = 0; j < N; j++)	{
				close(pipeFiglioPadre[j][0]);
				close(pipePadreFiglio[j][1]);
				if (j != i)	{
					close(pipeFiglioPadre[j][1]);
					close(pipePadreFiglio[j][0]);
				}
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			cont=0;
			k = 0;
			for(j = 0; j < H; j++){
				while(read(fd, &linea[k], 1)){
					if(linea[k] == '\n'){
						linea[k+1] = '\0';

						// invio la lunghezza della linea:
						write(pipeFiglioPadre[i][1], &k, sizeof(int));

						// leggo l'indice da considerare:
						read(pipePadreFiglio[i][0], &index, sizeof(int));
						
						// se l'indice è ammissibile:
						if(index <= k){
							write(Fcreato, &linea[index], 1);
							cont++;
						}
						k = 0;
					}
					else
						k++;
				}
			}


			exit(cont);
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
	
	lunghezze = malloc(N*sizeof(int));

	/* legge dalle pipe i messaggi o manda segnali?*/
	for(j = 0; j < H; j++){
		for(i = 0; i < N; i++){
			read(pipeFiglioPadre[i][0], &lunghezze[i], sizeof(int));
			printf("Padre: Figlio %d riga %d lunghezza %d\n", i, j, lunghezze[i]);
		}

		int lunghezza_considerata = lunghezze[mia_random(N-1)];
		printf("\nFra le lunghezze inviate considero: %d\n", lunghezza_considerata);

		int index = mia_random(lunghezza_considerata);
		printf("L'indice che invio ai figli è: %d\n", index);

		for(i = 0; i < N; i++){
			write(pipePadreFiglio[i][1], &index, sizeof(int));
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
