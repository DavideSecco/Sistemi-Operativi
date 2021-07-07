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
    int B;   							/* numero di caratteri e quindi numero di processi */
    int fd, fdc;      						/* per open */
    int q, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char F[255];
	int L;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
	strcpy(F, argv[1]);
	B = atoi(argv[argc-2]);
	L = atoi(argv[argc-1]);

	char* blocco = malloc(L/B*sizeof(char) + 1);

	if(B <= 0 || L <=0 ){
		printf("Non hai inserito dei numeri\n");
		exit(2);
	}
	
	strcat(F, ".Chiara");
	if ((fdc = creat(F, O_WRONLY)) < 0) { 
		printf("Errore nella apertura del file\n");
		exit(-1);
	}


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione B pipe */
    if ((piped=(pipe_t *)malloc(B*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo B pipe */
    for (q=0; q < B; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), B);
    for (q=0; q < B; q++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for (k = 0; k < B; k++)	{
				close(piped[k][0]);
				if (k != q)
					close(piped[k][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			int puntolettura = q*L/B;
			lseek(fd, puntolettura, SEEK_SET);

			read(fd, blocco, sizeof(blocco));
			blocco[L/B] = '\0'; 

			printf("Figlio %d parte a leggere dalla posionze %d letto: %s\n", q, puntolettura, blocco);

			write(piped[q][1], &blocco[L/B-1], 1);


			exit(B);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < B; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/

	for(k = 0; k < B; k++){
		read(piped[k][0], &c, 1);
		write(fdc, &c, 1);
		printf("Padre: scritto %c del figlio %d su file creato\n", c, k);
	}

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(q=0;q < B;q++) {
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
