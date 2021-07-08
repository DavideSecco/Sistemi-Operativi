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
    int pid;			/* sostituisce campo c1  */
	int lung; 				/* sostituisce campo c2  */
} s_occ;

void scambia(s_occ *s1, s_occ *s2){
	s_occ temp;

	temp.pid = s1->pid;
	temp.lung = s1->lung;

	s1->pid = s2->pid;
	s1->lung = s2->lung;

	s2->pid = temp.pid;
	s2->lung = temp.lung;
}

void bubbleSort(s_occ strut[], int dim){
	int i; 
	bool ordinato = false;
	while (dim>1 && !ordinato) {
		ordinato = true; /* hp: è ordinato */
		for (i=0; i<dim-1; i++)
			if (strut[i].lung > strut[i+1].lung) { /* ordinamento crescente; per l’ordinamento decrescente scrivere if (v[i] < v[i+1]) */	
				scambia(&strut[i],&strut[i+1]);
				ordinato = false;
			}
	dim--;
	}
}

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
   	int N;   							/* numero di caratteri e quindi numero di processi */
    int fd;      						/* per open */
    int i, j, k;     						/* indici, i per i figli! */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe */
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int Y;								// lunghezza in linee del file
	s_occ *strut;
	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
    N = argc - 2;
	Y = atoi(argv[argc-1]);

	if(Y <= 0){
		printf("Hai inserito un numero non valido\n");
		exit(2);
	}

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

			// alloco il posto per la struttura:
			strut = malloc((i+1)*sizeof(s_occ));

			strut[i].pid = getpid();

			// leggo tutte le linee in ogni figlio:
			for(j = 0; j < Y; j++){

				if(i != 0){
					read(piped[i-1][0], strut, sizeof(strut)*i);
				}
				
				k = 0;
				while(read(fd, &c, 1)){
					if(c == '\n'){
						strut[i].lung = k;
						printf("Figlio %d pid: %d linea %d lunghezza %d\n", i,  strut[i].pid, j+1, strut[i].lung);
						break;
					}
					else 
						k++;
				}

				write(piped[i][1], strut, sizeof(strut)*(i+1));
			}


			exit(0);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* OBBLIGATORIO: chiude tutte le pipe che non usa */
	for (k = 0; k < N; k++)	{
		close(piped[k][1]);
		if (k != N - 1)		{
			close(piped[k][0]);
		}
	}
	
	strut = malloc(N*sizeof(s_occ));

	sleep(1);

	/* legge dalle pipe i messaggi o manda segnali?*/
	for(j = 0; j < Y; j++){
		read(piped[N-1][0], strut, sizeof(strut)*N);

		bubbleSort(strut, N);

		printf("Linea %d\n", j+1);
		for(i = 0; i < N; i++){
			printf("Figlio %d PID: %d  linea %d Numero caratteri: %d\n", i, strut[i].pid,  j+1, strut[i].lung);
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
