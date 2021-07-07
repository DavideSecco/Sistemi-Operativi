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
    int pid;				/* campo c1  */
	char secondo; 			/* campo c2  */
	char ultimo;			/* campo c3  */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int L;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int q, j, k;     						/* indici, q per q figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	char F[255];
	char linea[250];
	s_occ s;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc != 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
	strcpy(F, argv[1]);
	L = atoi(argv[2]);

	if(L <= 0 || L >= 255){
		printf("Inserisci un numero positivo\n");
		exit(2);
	}    

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
			for (k = 0; k < L; k++)	{
				close(piped[k][0]);
				if (k != q)
					close(piped[k][1]);
			}

			/* OBBLIGATORIO: apre il file */
            if ((fd = open(argv[1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[1]);
                exit(-1);
            }

			/* eseguo codice figlio e eventuale nipote */
			cont = 1;

			// arrivo alla linea che devo leggere
			while((q + 1) > cont && read(fd, &c, 1)){
				//printf("Letto %c\n", c);
				if(c == '\n')
					cont++;
			}

			// leggo la linea che devo leggere
			j = 0;
			while(read(fd, &linea[j], 1)){
				if(linea[j] == '\n'){
					linea[j] = '\0';	
					break;
				}
				else 
					j++;
			}

			s.pid = getpid();
			s.secondo = linea[1];
			s.ultimo = linea[j-1];

			// printf("figlio %d secondo: %c ultimo: %c\n", q, s.secondo, s.ultimo);

			write(piped[q][1], &s, sizeof(s_occ));
			

			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < L; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe q messaggi o manda segnali?*/

	for (k = 0; k < L; k++)	{
		read(piped[k][0], &s, sizeof(s_occ));

		if(s.secondo == s.ultimo){
			printf("Figlio %d riga %d\n", k, k+1);
			printf("Pid del figlio: %d\n", s.pid);
			printf("Secondo e ultimo carattere: %c\n\n", s.secondo);
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
