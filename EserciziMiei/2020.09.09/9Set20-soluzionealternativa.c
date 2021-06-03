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

int main(int argc, char *argv[]) {
    int pid;      						/* pid per fork */
   	int L;								// numero linee
    int Q;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int q, j;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char carattere;						// carattere assegnato a ogni figlio
	char ok;
	int linea;
	int occorrenze;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 5)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    Q = argc - 3;

	L = atoi(&argv[2][0]);

	if(L < 0){
		printf("ERRORE nell'inserimento del numero strettamente positivo\n");
		exit(2);
	}

	printf("Numero inserito: %d \n", L);
	printf("Numero di caratteri inseriti: %d\n", Q);

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione Q+1 pipe */
    if ((piped=(pipe_t *)malloc((Q+1)*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo Q pipe */
    for(q=0; q <= Q; q++) {
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
            carattere = argv[q+3][0];
			printf("Figlio %d con pid %d\n", q, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa */
			/* chiusura per schema a ring: */
			for (j=0; j <= Q; j++){
				if(j != q)
					close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
				if(j != (q+1))
					close(piped[j][1]);			// in scrittura lascio aperta la pipe ...
			}

			
			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }
			
			printf("Il figlio %d ha aperto il file %s e cercherà, riga per riga il carattere %c\n", q, argv[1], carattere);
			
			occorrenze = 0;

			for(linea = 1; linea <= L; linea++){
				occorrenze=0;
				read(piped[q][0], &ok, 1);			// leggo dalla pipe dell'indice del figlio per il via
				while(read(fd, &c, 1)){
					if(c == '\n')
						break;
					else if(c == carattere)
						occorrenze++;
				}
				printf("\t%d occorrenze del carattere '%c'\n", occorrenze, carattere);
				write(piped[q+1][1], &ok, 1); 	// scrivo sulla pipe "successiva" al figlio
			}

			exit(occorrenze);
		}
	}

	/* padre */
    printf("Sono il padre (pid: %d)\n", getpid());
	   
    /* chiude tutte le pipe che non usa */
	for(q = 0; q <= Q; q++){
		if(q != 0)
			close(piped[q][1]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		if(q != Q)
			close(piped[q][0]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}

	sleep(1);
	for(linea = 1; linea <= L; linea++) {
		printf("\nLinea %d\n", linea);
		write(piped[0][1], &ok, sizeof(char));		// lancio l'innesco
		read(piped[Q][0], &ok, 1);
	}


	close(piped[0][1]);							// posso chiudere la pipe iniziale di scrittura
    

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
