#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

int token;
typedef int pipe_t[2];

void scrivi(){
	token = 1;
}

void salta(){
	token = 0;
}

int indicemax(char vet[], int N){
	int max=0;
	for (int i = 1; i < N; i++){
		if(vet[max] < vet[i]){
			max = i;
		}
	}
	return max; 
}

int main(int argc, char *argv[]){
    int *pid;							/* array di pid */
	int nr;
	char linea[250];
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char vet[255];
	bool b;
	int max;
	int nlinea;
	int cont;
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
    if ((piped=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	/* allocazione array per i pid */
    if ((pid = (int *)malloc(N * sizeof(int))) == NULL) {
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
			for (j = 0; j < N; j++){
				close(piped[j][0]);
				if(i!=j)
					close(piped[j][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			// leggo il file
			j=0;
			nlinea=0;
			cont=0;
			while(1){
				nr = read(fd, &linea[j], 1);

				if(linea[j] == '\n' || nr == 0){
					nlinea++;
					linea[j]='\0';
					write(piped[i][1], &linea[0], 1);
					pause();

					if(token){
						printf("Il processo %d (PID %d) carattere: %c linea: %d\n", i, getpid(), linea[0], nlinea);	
						cont++;
					}

					if(nr == 0){
						exit(cont);
						break;
					}
					j=0;
				}
				else
					j++;
				

				
			}
			
			exit(cont);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(i = 0; i < N; i++){
		close(piped[i][1]);
	}

	/* legge dalle pipe i messaggi o manda segnali?*/
	j=0;
	b=false;
	nlinea=0;
	while(1){
		nlinea++;
		
		for(i = 0; i < N; i++){
			nr = read(piped[i][0], &c, 1);
			if(nr > 0){
				b = true;
				vet[j] = c;
			}
			else
				vet[j]=0;
			j++;
		}
		if(b == false){
			break;
		}
		max=indicemax(vet, N);
		printf("Linea %d\n", nlinea);
		for(i = 0; i <N; i++){
			if(max != i)
				kill(pid[i], SIGUSR2);		// INVIO ai figli che non devono STAMPARE LA LINEA
			else
				kill(pid[i], SIGUSR1);		// INVIO IL SEGNALE ALL'UNICO FIGLIO CHE DEVE STAMPARE LA LINEA
		}
		sleep(1);
		j = 0;
		b = false;
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
