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

int main(int argc, char *argv[])
{
    int pid;      						/* pid per fork */
   	int Q;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int q, k;     						/* indici, q per q figli! */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int fdCam;
	char linea[255];
	int count=0;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    Q = argc - 1;

	printf("Sono stati inseriti %d file\n", Q);

	/* creo il file camilla --> DEVO CONTROLLARE CHE LA CREAZIONE SIA ANDATA A BUON FINE??? */
	fdCam=creat("Camilla", O_WRONLY);


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione Q pipe */
    if ((piped=(pipe_t *)malloc(Q*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo Q pipe */
    for (q=0; q < Q; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), Q);
    for (q=0; q < Q; q++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for(k = 0; k < Q; k++){
				close(piped[k][0]);
				if(k!=q)
					close(piped[k][1]);
			}
			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[q+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			k=0;
			count =0;
			/* eseguo codice figlio e eventuale nipote */
			while(read(fd, &linea[k], 1)){
				if(linea[k] == '\n'){
					linea[k]='\0';
					if(isdigit(linea[0]) >= 0 && (strlen(linea) + 1) < 10) {
						write(piped[q][1], linea, strlen(linea) + 1);
						count++;
					}
					k = 0;
				}
				else 
					k++;
			}

			exit(count);
		}
	}

	/* padre */
	printf("Padre ha PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(int q=0; q<Q; q++){
		close(piped[q][1]);					// chiudo le pipe in scrittura
	}
	sleep(1);
	/* legge dalle pipe q messaggi o manda segnali?*/
	for (q=0; q<Q; q++){
		k = 0;
		while(read(piped[q][0], &linea[k], 1)){
			if(linea[k]=='\0'){
				printf("Figlio %d file %s linea: %s\n", q, argv[q+1], linea);
				k=0;
			}
			else
				k++;
		}
	}


	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(q=0;q < Q;q++) {
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
