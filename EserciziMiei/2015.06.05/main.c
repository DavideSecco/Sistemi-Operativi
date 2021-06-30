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
    int pid, pidnipote;      			/* pid per fork */
   	int M;   							/*M numero di file e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
	pipe_t pipenipote;
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int lung;
	int nread;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    M = argc - 1;

	printf("Sono stati inseriti %d\n", M);

	//ALLOCAZIONE MEMORIA MALLOC

    /* OBBLIGATORIO: allocazione M pipe */
    if ((piped=(pipe_t *)malloc(M*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo M pipe */
    for (i=0; i < M; i++) {
        if (pipe(piped[i]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), M);


    for (i=0; i < M; i++) {
		/* OBBLIGATORIO: creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", i, getpid());

			/* OBBLIGATORIO: chiude tutte le pipe che non usa (scegli schema chiusura!) */
			for(j = 0; j < M; j++){
				close(piped[j][0]);
				if (j != i)
					close(piped[j][1]);
			}

			/* prima creiamo la pipe "p" di comunicazione fra nipote e figlio */
			if (pipe(pipenipote) < 0) {
				printf("Errore nella creazione della pipe fra figlio e nipote!\n");
				exit(-2);
			}

			if ((pidnipote = fork()) < 0)	{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-3);
			}

			/* codice del nipote */
			if (pidnipote == 0){
				printf("Sono il processo nipote del figlio di indice %d e pid %d e sto per recuperare l'ultima linea del file %s\n", j, getpid(), argv[j + 1]);
				/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(piped[j][1]);
				/* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
				close(0);
				if ((fd = open(argv[i + 1], O_RDONLY)) < 0) {
					printf("Errore nella open del file %s\n", argv[i + 1]);
					exit(-4);
				}
				/* ogni nipote deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
				close(1);
				dup(pipenipote[1]);
				/* ogni nipote adesso puo' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard output */
				close(pipenipote[0]);
				close(pipenipote[1]);
				/* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video) */
				close(2);
				open("/dev/null", O_WRONLY);
	
				/* Il nipote diventa il comando wc -1 */
				execlp("tail", "tail", "-1", (char *)0);
				/* attenzione ai parametri nella esecuzione di wc: aolo -1 e terminatore della lista. */
	
				/* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
				exit(-1);
			}
			
			close(pipenipote[1]);
			
			lung = 0;
			while((nread = read(pipenipote[0], &c, 1))){
				printf("Ho letto: %c\n", c);
				lung++;
			}

			write(piped[i][1], &lung, sizeof(int));

			ritorno=-1;
			pid = wait(&status);
			if (pid < 0) {	
				printf("Errore in wait\n");
				exit(-5);
			}
			if ((status & 0xFF) != 0)
				printf("Nipote con pid %d terminato in modo anomalo\n", pid);
			else{
				printf("Il nipote con pid=%d ha ritornato %d\n", pid, ritorno=(int)((status >> 8) & 0xFF));
				exit(ritorno);
			}	
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < M; k++)
		close(piped[k][1]);

	/* legge dalle pipe i messaggi o manda segnali?*/
	for(j = M-1; j >= 0; j--){
		read(piped[j][0], &lung, sizeof(int));
		printf("Indice: %d file: %s Caratteri ultima riga: %d \n", j, argv[j+1], lung);

	}
	
	

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(i=0;i < M;i++) {
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
