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

//variabile token per far si che il figlio scriva o meno
int token = 0;

void salta(){
    token = 0;
}

void scrivi(){
    token = 1;
}


typedef struct{
    int indice;			/* rappresenta il campo c1 */
	int occ; 			/* rapprenseta il campo c2 */
} s_occ;

int main(int argc, char *argv[]){
    int *pid;							/* array di pid */

    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char Cx;
	int H;								/* numero intero che rappresenta le linee dei file */
	s_occ s;
	char riga[255];

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
	if(strlen(argv[argc-2]) != 1){
		printf("Non hai inserito singolo carattere\n");
		exit(2);
	}

    N = argc - 3;
	Cx = argv[argc-2][0];
	H = atoi(argc[argv-1]);

	if(H <= 0){
		printf("Il numero inserito non è strettamente positivo\n");
		exit(3);
	}
	
	printf("Sono stati inseriti %d nomi di file\n", N);
	printf("Il singolo carattere è: %c\n", Cx);
	printf("Il numero inserito è: %d\n", H);

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
    signal(SIGUSR1,scrivi);
    signal(SIGUSR2,salta);

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

			//schema pipeline: ogni figlio legge dalla pipe i-1 e scrive sulla i
			for (k = 0; k < N; k++)	{
				if (k != i)
					close(piped[k][1]);
				if (i == 0 || k != i - 1)
					close(piped[k][0]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* ciclo di lettura e invio*/
			
			for (k = 0; k<H; k++){
				if (i != 0) {
					read(piped[i-1][0], &s, sizeof(s_occ));
					// printf("indice %d: Ho letto dal figlio prec indice %d occ %d\n", i, s.indice, s.occ);
				}

				cont = 0;
				j=0;
				while(read(fd, &riga[j], 1)){
					if(riga[j] == '\n'){
						riga[j]='\0';
						break;
					}
						
					if(riga[j] == Cx)
						cont++;

					j++;
				}

				// printf("Indice %d, riga %d, occ %d\n", i, k, cont);
				if(cont > s.occ || i == 0){
					s.indice = i;
					s.occ = cont;
				}

				// scrivo sul figlio (o padre) successivo
				write(piped[i][1], &s, sizeof(s_occ));

				pause();

				if(token){
					printf("Riga: %s\n\n", riga);
				}
			}


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	/* chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
		if (k != N - 1)
			close(piped[k][0]);
	}

	
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	for(k = 0; k < H; k++){
		read(piped[N-1][0], &s, sizeof(s_occ));

		printf("Indice %d PID: %d linea: %d occorrenze: %d\n", s.indice, pid[s.indice], k, s.occ);

		for (i = 0; i < N; i++){
			if (i == s.indice)			
				kill(pid[i], SIGUSR1);
			else
				kill(pid[i], SIGUSR2);
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
