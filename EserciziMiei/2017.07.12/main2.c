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

int mia_random(int n){
	int casuale;
	casuale = rand() % n;
	casuale++;
	return casuale;
}

typedef struct{
    int pidnipote;			/* campo c1 */
	int numlinea; 			/* campo c2 */
	char linea[255];		/* campo c3 */
} s_occ;

int main(int argc, char *argv[]){
    int pid, pidnipote;      						/* pid per fork */
    int N;   							/* numero di caratteri e quindi numero di processi */
    int i, j, k;     						/* indici, i per i figli! */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int *X;
	s_occ s;
	char linea[255];
	int a;
	pipe_t p;

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 3)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }

	if((argc % 2) == 0){
		printf("I paramentri passati devono essere pari\n");
		exit(2);
	}

	N = (argc)/2;

	X = malloc(N*sizeof(int));

	a=0;
	for(i=2; i < argc; i=i+2){
		X[a] = atoi(argv[i]);
		a++;

		if(X <= 0){
			printf("Il paramentri è negativo\n");
			exit(2);
		}
	}
    
    printf("Sono stati inseriti %d file e altrettanti numeri interi positivi\n", N);

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

			/* eseguo codice figlio e eventuale nipote */
			/* prima creiamo la pipe "p" di comunicazione fra nipote e figlio */
			if (pipe(p) < 0){
				printf("Errore nella creazione della pipe fra figlio e nipote!\n");
				exit(-2);
			}
	
			if ((pidnipote = fork()) < 0)	{
				printf("Errore nella fork di creazione del nipote\n");
				exit(-3);
			}

			if (pidnipote == 0)	{
				/* codice del nipote */
				printf("Sono il processo nipote del figlio di indice %d e pid %d \n", i, getpid());

				int num = mia_random(X[i]);		// inizializzo il numero di linee che andrò a prendere

				
				/* chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa */
				close(piped[i][1]);
				/* Ridirezione dello standard input (si poteva anche non fare e passare il nome del file come ulteriore parametro della exec):  il file si trova usando l'indice i incrementato di 1 (cioe' per il primo processo i=0 il file e' argv[1]) */
				char numchar[11];

				sprintf(numchar,"-%d", num);

				printf("Preso il numero X[i] = %d è stato estratto: %s\n", X[i], numchar);
				printf("Ho appena aperto il file %s\n", argv[2*i+1]);
				close(0);
				if (open(argv[2*i + 1], O_RDONLY) < 0) {
					printf("Errore nella open del file %s\n", argv[2*i+ 1]);
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
				execlp("head", "head", numchar, (char *)0);
				/* attenzione ai parametri nella esecuzione di wc: aolo -1 e terminatore della lista. */
	
				/* Non si dovrebbe mai tornare qui!!: ATTENZIONE avendo chiuso lo standard output e lo standard error NON si possono fare stampe con indicazioni di errori; nel caso, NON chiudere lo standard error e usare perror o comunque write su 2 */
				exit(-1);
			}

			/* ogni figlio deve chiudere il lato che non usa della pipe di comunicazione con il nipote */
			close(p[1]);
			/* adesso il figlio legge dalla pipe */
			j = 0; // indice per il carattere della linea
			k = 1;	// contatore per il numero di linee
			int nr;
			while ((nr = read(p[0], &linea[j], 1))) {
				if(linea[j] == '\n'){
					linea[j] = '\0';

					s.pidnipote = pidnipote;
					s.numlinea = k;
					strcpy(s.linea, linea);

					printf("Fgilio: PID: %d, numlinea: %d linea: %s\n", s.pidnipote, s.numlinea, s.linea);

					write(piped[i][1], &s, sizeof(s_occ));
					k++;
					j = 0;
				}
				else
					j++;
			}

			printf("nr = %d\n", nr);

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
	bool finito = false;
	
	while(finito == false){
		finito = true;
		for(i = 0; i < N; i++){
			int nr = read(piped[i][0], &s, sizeof(s_occ));

			if(nr != 0){
				finito = false;
				printf("Pid nipore: %d linea: %d linea: %s\n", s.pidnipote, s.numlinea, s.linea);
			}
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
