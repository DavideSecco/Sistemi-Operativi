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
    long int max;			/* c1  */
	int indice; 			/* c2 */
	long int somma;			/* c3 */
} s_occ;

int main(int argc, char *argv[]){
    //int pid;      						/* pid per fork */
   	int *pid;							/* array di pid */
	int occ;
    int N;   							/* numero di caratteri e iuindi numero di processi */
    int fd;      						/* per open */
    int i, j;	     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    	/* variabili per wait*/
	char Cz;
	s_occ s;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	Cz = argv[argc-1][0];

	printf("Numero di file inseriti: %d\n", N);
	printf("Carattere inserito: %c\n", Cz);

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
			for (j=0; j < N; j++){
				if(j != i)
					close(piped[j][0]);			// in lettura lascio aperta la pipe con lo stesso indice del figlio
				if(j != ((i+1) % N))
					close(piped[j][1]);			// in scrittura lascio aperta la pipe ...
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[i+1]);
                exit(-1);
            }

			/* leggo dal file */
			occ=0;
			while(read(fd, &c, 1)){
				if(c == Cz)
					occ++;
			}

			// se è il primo figlio:
			if(i == 0){
				s.max = occ;
				s.indice = 0;
				s.somma = occ; 
			}
			// se non è il primo figlio:
			else{
				read(piped[i][0], &s, sizeof(s_occ));
				s.somma = s.somma + occ;
				if(occ > s.max){
					s.indice = i;
					s.max = occ;
				}
			}

			// invio tutto al figlio (o padre) successivo
			write(piped[(i+1)%N][1], &s, sizeof(s_occ));

			exit(i);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(i = 1; i < N; i++){
		close(piped[i][0]);			// tengo aperto la pipe[0] in scrittura per l'innesco 
		close(piped[i][1]);			// e pipe[0] in lettura per evitare il sigpipe dell'ultimo figlio nell'ultima scrittura
	}

	close(piped[0][1]);		// posso chiudere la pipe 0 di scrittura

	// NON chiudo la pipe in lettura!

	/* legge dalle pipe i messaggi o manda segnali?*/
	read(piped[0][0], &s, sizeof(s_occ));
	printf("\n\nI DATI RITORNATI SONO:\n");
	printf("c1 (max): %ld\n", s.max);
	printf("c2 (indice): %d\n", s.indice);
	printf("c3 (somma): %ld\n", s.somma);

	printf("\nPID: %d\n", pid[s.indice]);
	printf("Nome del file: %s\n\n", argv[s.indice + 1]);
	
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
