#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include <limits.h>

typedef int pipe_t[2];

typedef struct{
    int indice;			/* campo c1  */
	int occ; 			/* campo c2  */
} s_occ;

int main(int argc, char *argv[]){
   	int *pid;							/* array di pid */

    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int H;
	s_occ s;
	char Cx;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	H = atoi(argv[argc-1]);

	if(H <= 0){
		printf("La lunghezza inserito non è valida\n");
		exit(2);
	}

	printf("Sono stati inseriti %d file\n", N);
	printf("Carattere inserito %c\n", H);

	printf("Inserisci caratere Cx:\n");
	scanf("%c", &Cx);

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

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
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

			/* eseguo codice figlio e eventuale nipote */

			for(j = 0; j < H; j++){
				cont = 0;

				if(i != 0){
					read(piped[i-1][0], &s, sizeof(s_occ));
				}
				else if(i == 0){
					s.indice = -1;
					s.occ = INT_MAX;
				}

				while(read(fd, &c, 1)){
					
					if(c == '\n'){
						
						if(cont < s.occ){
							s.indice = i;
							s.occ = cont;
						}	
						
						write(piped[i][1], &s, sizeof(s_occ));
						break;
					}
					else if(c == Cx){	
						cont++;
					}
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
		if (k != N - 1)	{
			close(piped[k][0]);
		}
	}
	sleep(1);
	/* legge dalle pipe i messaggi o manda segnali?*/
	for(j = 0; j < H; j++){
		int nr = read(piped[N-1][0], &s, sizeof(s_occ));
		printf("Padre: riga %d: figlio %d PID: %d occorrenze: %d\n", j, s.indice, pid[i], s.occ);
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
