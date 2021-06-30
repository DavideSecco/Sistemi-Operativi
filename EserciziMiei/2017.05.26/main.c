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

typedef struct{
    long int max; 		/* rapprensenta il campo c1 */
	int indice;			/* rapprensenta il campo c2 */
	long int somma; 		/* rapprensenta il campo c3 */
}s_occ;

int main(int argc, char *argv[]){
   	int *pid;							/* array di pid */
	char Cx;
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    	/* variabili per wait*/
	s_occ s;
	int occ;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
	if(strlen(argv[argc-1]) != 1){
		printf("Non hai inserito un singolo carattere\n");
		exit(2);
	}

    N = argc - 2;
	Cx = argv[argc-1][0];
	printf("Sono stati inseriti %d file\n", N);
	printf("Il carattere inserito è: %c \n", Cx);

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
			for (k = 0; k < N; k++) {
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

			// conto le occorrenze del carattere:
			occ = 0;
			while(read(fd, &c, 1)){
				if(c == Cx)
					occ++;
			}

			if(i == 0){
				s.indice = i;
				s.max = occ;
				s.somma = occ;
			}
			else{
				// leggo la struttura che mi arriva dal figlio precendete:

				read(piped[i-1][0], &s, sizeof(s_occ));
				printf("Figlio %d: dal figlio %d ho letto:\n", i, (i-1));
				printf("Max: %ld\n", s.max);
				printf("Indice: %d\n", s.indice);
				printf("Somma: %ld\n", s.somma);
				
				if(s.max < occ){
					s.indice = i;
					s.max = occ;
				}

				s.somma = s.somma + occ;
			}

			// scrivo sul figlio (o padre) successivo
			write(piped[i][1], &s, sizeof(s_occ));

			exit(i);
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

	read(piped[N-1][0], &s, sizeof(s_occ));
	printf("Padre:\n");
	printf("Max: %ld\n", s.max);
	printf("Indice: %d\n", s.indice);
	printf("Somma: %ld\n", s.somma);
	printf("Pid: %d\n", pid[s.indice]);

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
