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
    int pid1;				/* campo c1  */
	char secondo;			/* campo c2  */
	char penultimo			/* campo c3  */
} s_occ;

int main(int argc, char *argv[]) {
    int pid;      						/* pid per fork */
   	int fd;      						/* per open */
    int q, k, j;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char F[255];
	int L;
	s_occ s;
	char linea[255];
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc != 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    strcpy(F,argv[1]);

	L = atoi(argv[2]);
	if(L <= 0 || L > 255){
		printf("Numero inserito non valido\n");
		exit(2);
	}
	
	printf("Nome assoluto di file: %s\n", F);
	printf("Numero inserito: %d\n", L);


	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione L pipe */
    if ((piped=(pipe_t *)malloc(L*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo L pipe */
    for (q=0; q < L; q++) {
        if (pipe(piped[q]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), L);
    for (q=0; q < L; q++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", q, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for(k = 0; k < L; k++){
				close(piped[k][0]);
				if(k != q)
					close(piped[k][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			/* eseguo codice figlio */
			/* scorro fino a trovare la linea del figlio */
			j = 0;
			while(j < q){
				read(fd, &c, 1);
				if(c == '\n')
					j++;
			}

			j=0;				// contatore per i caratteri della linea
			while(read(fd, &linea[j], 1)){
				if(linea[j] == '\n'){
					linea[j]='\0';
					break;
				}
				else
					j++;
			}
			s.pid1 = getpid();
			s.secondo = linea[1];
			s.penultimo = linea[strlen(linea)-1];

			write(piped[q][1], &s, sizeof(s_occ));


			exit(q+1);
		}
	}

	/* padre */

	sleep(1);
	printf("Sono il padre con PID %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for(q=0; q<L; q++){
		close(piped[q][1]);
	}
	
	/* legge dalle pipe i messaggi */
	for(k = 0; k < L; k++){
		read(piped[k][0], &s, sizeof(s_occ));
		if(s.secondo == s.penultimo){
			printf("\nLinea %d\n", k);
			printf("PID figlio: %d\n", s.pid1);
			printf("Carattere %c\n", s.secondo);
		}
	}

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(q=0;q < L;q++) {
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
