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

typedef struct{
    int indice;				/* campo c1 del testo */
	int occorenze; 			/* campo c2 del testo  */
} s_occ;

void scrivi(){
	token = 1;
}

void salta (){
	token = 0;
}

int main(int argc, char *argv[]){
    int *pid;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;     /* variabili per wait*/
	char Cx;
	int H;
	s_occ s;
	int nr;
	char linea[255];

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4 )   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 3;
	Cx = argv[argc-2][0];
	H = atoi(argv[argc-1]);

	if(strlen(argv[argc-1]) != 1){
		printf("Non hai inserto un singolo carattere\n");
		exit(2);
	}

	if(H <= 0){
		printf("Numero H non valido\n");
		exit(3);
	}

	printf("Hai inserito %d file\n", N);
	printf("Hai inserito il carattere %c\n", Cx);
	printf("Hai inserito il numero %d\n", H);
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

	/*  padre aggancia le due funzioni (scrivi e salta) che useranno i figli alla ricezione dei segnali inviati dal padre */
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

			//schema pipeline: ogni figlio legge dalla pipe i-1 e scrive sulla i
			for (k = 0; k < N; k++)			{
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

			/* eseguo codice figlio */
			
			for(k = 0; k < H; k++){

				if(i != 0)
					read(piped[i-1][0], &s, sizeof(s_occ));
				else{
					s.occorenze = 0;
					s.indice = 0;
				}

				j = 0;
				cont = 0;
				while((nr = read(fd, &linea[j], 1)) > 0 ){
					if(linea[j] == '\n'){
						linea[j] = '\0';
						break;
					}
					if(linea[j] == Cx)
						cont++;
					else 
						j++;
				}

				// aggiorno i dati della struttura nel caso ce ne sia bisogno 
				if(s.occorenze < cont){
					s.indice = i;
					s.occorenze = cont;
				}

				// invio la struttura
				write(piped[i][1], &s, sizeof(s_occ));	

				// attesa del segnale
				pause();

				if(token){
					printf("Linea %d: %s\n", k, linea);
					printf("Figlio %d PID: %d occorenze: %d	\n\n", i, getpid(), s.occorenze);
				}
			}
			


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
	/* chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);

		if (k != N - 1)		{
			close(piped[k][0]);
		}
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	sleep(1);
	for(i = 0 ; i< H; i++){
		read(piped[N-1][0], &s, sizeof(s_occ));

		for(k=0; k<N; k++){
			if(k == s.indice)
				kill(pid[s.indice], SIGUSR1);
			else
				kill(pid[k], SIGUSR2);
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
