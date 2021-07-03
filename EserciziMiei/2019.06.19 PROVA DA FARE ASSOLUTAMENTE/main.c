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
    int i, j, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char linea[250];
	bool b;
	char max;
	int id_max;
	int nr;

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

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for (k = 0; k < N; k++)	{
				close(piped[k][0]);
				if (k != i)
					close(piped[k][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			j = 0;
			while(read(fd, &linea[j], 1)){
				if(linea[j] == '\n'){
					linea[j]='\0';
					printf("F%d linea: %s\n", i, linea);
					write(piped[i][1], &linea[0], 1);
					j=0;

					pause();

					if(token){
						printf("Riga: %s\n\n", linea);
					}
				}
				else{
					j++;
				}
			}


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	b = true;
	int riga = 1;
	while(b == true){
		b = false;
		max = 0;
		for(k = 0; k < N; k++){
			nr = read(piped[k][0], &c, 1);
			printf("%c", c);
			
			// se ho letto, mi devo ricordare che ci sono ancora figli vivi
			if(nr != 0){
				b = true;		// metto la variabile b a true

				if(c >= max){	// se il valore che ho letto è maggiore del valore che avevo prima, mi salvo l'indice e il nuovo valore max
					id_max=k;
					max = c;
				}
			}
		}

		if(b == false)
			break;
		else{
			for(k = 0; k < N; k++){
				if (k == id_max){			
					printf("\nIndice: %d PID: %d carattere: %c riga %d\n", id_max, pid[id_max], max, riga);
					kill(pid[k], SIGUSR1);
					riga++;
				}
				else
					kill(pid[k], SIGUSR2);
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
