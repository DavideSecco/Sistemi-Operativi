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
    			/*  */
	 			/*  */
} s_occ;

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    int cont;     						/* per conteggio */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int fcreato;
	char nlinee[11];
	int linee;
	char linea[200];
	int lung;							// lunghezza delle linee
	int pidspeciale;
	pipe_t p;
	int nr;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;

	printf("Sono stati inseriti %d file\n", N);

	// creaiamo il file in /tmp:
	if ((fcreato = creat("/tmp/DavideSecco", O_WRONLY)) < 0) { 
		printf("Errore nella creazione del file %s\n", argv[1]);
		exit(-1);
	}

	if (pipe(p) < 0)        {
		printf("Errore nella creazione della pipe\n");
		exit(1);
	}

	if ((pidspeciale = fork()) < 0) {
		printf ("Errore nella fork\n");
		exit(1);
	}

	

	if(pidspeciale == 0){
		printf("Sono il processo speciale sto per recuperare il numero di linee linea del file %s (pid: %d)\n", argv[1], getpid());
		
		/* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
		close(0);
		if (open(argv[1], O_RDONLY) < 0) {
			printf("Errore nella open del file %s\n", argv[1]);
			exit(-4);
		}

		printf("Prova\n");
		/* il figlio speciale deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
		close(1);
		dup(p[1]);
		/* il figlio speciale adesso puo' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard output */
		close(p[0]);
		close(p[1]);
		/* Ridirezione dello standard error su /dev/null (per evitare messaggi di errore a video) */
		close(2);
		open("/dev/null", O_WRONLY);

		/* Il nipote diventa il comando wc -1 */
		execlp("wc", "wc", "-l", (char *)0);
		/* attenzione ai parametri nella esecuzione di wc: aolo -1 e terminatore della lista. */

		/* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
		exit(-1);
	}

	/* il padre deve chiudere il lato che non usa della pipe di comunicazione con il figlio */
	close(p[1]);
	/* adesso il padre legge dalla pipe */
	int l=0;
	printf("Prova2\n");
	while ((nr = read(p[0], &nlinee[l], 1)) > 0) {
		//printf("indice l= %d carattere letto da pipe %c\n", l, linea[l]);
		l++;
	}

	close(p[0]);
	
	printf("Numero di linee lette dal primo figlio: %s\n", nlinee);

	linee = atoi(nlinee);


	pidspeciale = wait(&status);
	if (pidspeciale < 0){
		printf("Errore wait\n");
		exit(9);
	}
		
	if ((status & 0xFF) != 0)
		printf("Figlio con pid %d terminato in modo anomalo\n", pidspeciale);
	else{
		ritorno=(int)((status >> 8) & 0xFF);
		printf("Il  figlio  con  pid=%d  ha  ritornato  %d  (se  255 problemi!)\n", pidspeciale, ritorno);
	}
	







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
			for (k = 0; k < N; k++){
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

			for(j = 0; j < linee; j++){
				
				k = 0;
				while(read(fd, &linea[k], 1)){
					if(linea[k] == '\n'){
						linea[k] = '\0';
						// printf("Figlio %d invio al padre la linea %d lunga %d: %s\n", i, j, k, linea);
						write(piped[i][1], &k, sizeof(int));
						write(piped[i][1], linea, strlen(linea));
						k = 0;
						break;
					}
					k++;
				}
			}


			exit(k);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/

	for(k = 0; k < linee; k++){
		for(i = 0; i < N; i++){
			// leggo la lunghezza della linea
			read(piped[i][0], &lung, sizeof(int));
			read(piped[i][0], linea, lung);

			printf("Padre %d ricevuot al padre la linea %d lunga %d: %s\n", i, k, lung, linea);

			write(fcreato, &lung, sizeof(int));
			write(fcreato, linea, lung);

		}
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
