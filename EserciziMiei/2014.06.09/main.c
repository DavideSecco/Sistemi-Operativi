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
    pipe_t *piped, p;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int nlinee;
	char linee[11];

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 1;

	printf("Sono stati inseriti %d\n", N);

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

			if (pipe(p) < 0){
				printf("Errore nella creazione della pipe fra figlio e nipote!\n");
				exit(-2);
			}
	
			if ((pid = fork()) < 0)	{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-3);
			}

			if (pid == 0)	{
				/* codice del nipote */
				printf("Sono il processo nipote del figlio di indice %d e pid %d e sto per contare le linee del file %s\n", i, getpid(), argv[i+ 1]);
				/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(piped[i][1]);
				/* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
				close(0);

				if (open(argv[i + 1], O_RDONLY) < 0) {
					printf("Errore nella open del file %s\n", argv[i + 1]);
					exit(-4);
				}
				/* ogni nipote deve simulare il piping dei comandi nei confronti del figlio e quindi deve chiudere lo standard output e quindi usare la dup sul lato di scrittura della propria pipe */
				close(1);
				dup(p[1]);
				/* ogni nipote adesso puo' chiudere entrambi i lati della pipe: il lato 0 non viene usato e il lato 1 viene usato tramite lo standard output */
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

			/* ogni figlio deve chiudere il lato che non usa della pipe di comunicazione con il nipote */
			close(p[1]);
			/* adesso il figlio legge dalla pipe */
			j=0;
			while (read(p[0], &linee[j], 1)) {
				printf("Figlio %d carattere letto da pipe %c\n", i, linee[j]);
				j++;
			}

			linee[j+1] = '\0';
			int nlinee = atoi(linee);

			/* il figlio comunica al padre */
			write(piped[i][1], &nlinee, sizeof(int));

			/* il figlio deve aspettare il nipote per restituire il valore al padre */
			/* se il nipote e' terminato in modo anomalo decidiamo di tornare -1 che verra' interpretato come 255 e quindi segnalando questo problema al padre */
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

			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
	}
	
	/* legge dalle pipe i messaggi o manda segnali?*/
	long int somma = 0;
	for(i = 0; i < N; i++){
		read(piped[i][0], &nlinee, sizeof(int));
		somma = somma + nlinee;
	}

	printf("SOmma delle linee: %ld\n", somma);
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
