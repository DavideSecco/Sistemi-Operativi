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
    long int c1;			/* long int c1  */
	long int c2; 			/* long int c2 */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, k;     						/* indici, i per i figli! */
    // long int contpari, contdisp;		/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	s_occ s;
	int pos;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3 || (argc % 2) == 0)   {
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
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
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
			s.c1 = 0;
			s.c2 = 0;

			while(read(fd, &c, 1)){
				pos = lseek(fd, 0, SEEK_CUR);
				printf("FIglio %d posizione %d carattere %c (%d)\n", i, pos, c, c);

				// se la posizione è pari, e il figlio è di indice pari
				if((pos % 2) == 0 && (i % 2) == 0){
					if((c % 2) == 0){
						s.c1++;
						printf("s.c1 %ld\n", s.c1);
					}
					else{
						s.c2++;
						printf("s.c2 %ld\n", s.c2);
					}
				}
				else if((pos % 2) == 1 && (i % 2) == 1){
					if((c % 2) == 1){
						printf("s.c1 %ld\n", s.c1);
						s.c1++;
					}
					else{
						s.c2++;
						printf("s.c2 %ld\n", s.c2);
					}
				}
			}

			write(piped[i][1], &s, sizeof(s_occ));


			if(s.c1 > s.c2)
				exit(0);
			else 
				exit(1);

		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/

	for(i = 0; i < N; i = i+2){
		read(piped[i][0], &s, sizeof(s_occ));

		printf("Figlio %d c1: %ld c2: %ld\n", i, s.c1, s.c2);
	}

	for(i = 1; i < N; i = i+2){
		read(piped[i][0], &s, sizeof(s_occ));

		printf("Figlio %d c1: %ld c2: %ld\n", i, s.c1, s.c2);
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
