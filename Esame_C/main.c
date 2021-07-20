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

typedef char linea[250];

int main(int argc, char *argv[]){
    int pid;      						/* pid per fork */
    int Y;   							/* numero di caratteri e quindi numero di processi */
    int fd, CreatedFile;      			/* per open e creat*/
    int y, j, k, a;     				/* indici per i figli! per contare */
    char c;       						/* per leggere dal file */
    pipe_t *piped;    					/* array dinamico di pipe --> userò lo schema di comunicazione a pipeline*/
    int pidFiglio, status, ritorno;    /* variabili per wait*/
	int LUNG;							// lunghezza in linee dei file
	linea *tutteL;						// lo userò come array di stringhe
	char buff[250];						// variabile buffer per memorizzare temporaneamente la linea
	int nr;								// utilizzata per contare quanti caratteri effettivamente ho scritto/letto durante le comunicazioni con le pipe

	//CONTROLLI TIPICI

    /* OBBLIGATORIO: numero dei caratteri passati sulla linea di comando */ 
	/* pongo che ci debbano essere almeno 2 file così da rendere la soluzione un po' più interessante.
	Nel caso infatti ci fosse un solo un file passato, il file creato sarebbe una copia del file passato
	*/ 
    if (argc < 4)   {
        printf("Errore nel numero dei parametri\n");
        exit(1);
    }
    
	// salvo il numero di file
    Y = argc - 2;

	// salvo la lunghezza in linee del file
	LUNG = atoi(argv[argc-1]);

	if(LUNG <= 0){
		printf("Il numero inserito non è valido\n");
		exit(2);
	}

	printf("Sono stati inseriti %d file\n", Y);
	printf("Il numero inserito invece è: %d\n", LUNG);

	if ((CreatedFile = creat("SECCO", O_WRONLY)) < 0) { 
		printf("Errore nella creazione del file\n" );
		exit(-1);
	}

	printf("Ho creato il file '%s'\n", "SECCO");

	//ALLOCAZIONE MEMORIA MALLOC

	// alloco spazio per il vettore di linee:
	if ((tutteL=malloc(Y*sizeof(buff))) == NULL)    {
    	printf("Errore allocazione vettore di linee\n");
    	exit(3); 
    }

    /* allocazione Y pipe di comunicazione */
    if ((piped=(pipe_t *)malloc(Y*sizeof(pipe_t))) == NULL)    {
    	printf("Errore allocazione pipe\n");
    	exit(3); 
    }

	//CREAZIONE PIPE 

    /* OBBLIGATORIO: creo Y pipe */
    for (y=0; y < Y; y++) {
        if (pipe(piped[y]) < 0)        {
            printf("Errore nella creazione della pipe\n");
            exit(1);
        }
	}

	printf("Sono il processo padre con pid%d e sto per generare %d figli\n", getpid(), Y);
    for (y=0; y < Y; y++) {
		/* creazione dei figli */
        if ((pid = fork()) < 0) {
            printf ("Errore nella fork\n");
            exit(1);
        }

		if (pid == 0) /* figlio */ {
            printf("Figlio %d con pid %d\n", y, getpid());

			/* figlio chiude tutte le pipe che non usa (schema chiusura a pipeline) */
			for (k = 0; k < Y; k++)	{
				if (k != y)
					close(piped[k][1]);
				if (y == 0 || k != y - 1)
					close(piped[k][0]);
			}

			/* apre il file */
            if ((fd = open(argv[y+1], O_RDONLY)) < 0) { 
                printf("Errore nella apertura del file %s\n", argv[y+1]);
                exit(-1);
            }

			/* eseguo codice figlio */
			for(j = 0; j < LUNG; j++){
				k = 0;
				while(read(fd, &buff[k], 1)){
					if(buff[k] == '\n'){

						// questo non sarebbe strettamente necessario, ma è utile per la stampa di debug
						buff[k+1] = '\0';

						// stampa di debug --> non necessaria
						printf("Figlio %d riga %d: %s", y, j, buff);


						// se non sono il primo figlio, leggo dal precendete
						if(y != 0){

							nr = read(piped[y-1][0], tutteL, sizeof(tutteL)*sizeof(buff));

							if(nr != sizeof(tutteL)*sizeof(buff)){
								printf("Errore lettura per figlio %d\n",y);
								exit(-1);
							}
						}

						strcpy(tutteL[y], buff);

						nr = write(piped[y][1], tutteL, sizeof(tutteL)*sizeof(buff));
						
						if(nr != sizeof(tutteL)*sizeof(buff)){
							printf("Errore scrittura per figlio %d\n",y);
							exit(-1);
						}

						break;
		
					}
					else
						k++;
				}
			}

			exit(k+1);
		}
	}

	/* padre */
	printf("Padre con PID: %d\n", getpid());
    
    /* chiude tutte le pipe che non usa: schema di chiusura a pipeline */
	for (k = 0; k < Y; k++)	{
		close(piped[k][1]);
		if (k != Y - 1)	{
			close(piped[k][0]);
		}
	}
	
	/* legge dalle pipe y messaggi o manda segnali?*/
	for(y = 0; y < LUNG; y++){
		read(piped[Y-1][0], tutteL, sizeof(tutteL)*sizeof(buff));

		/* non posso stampare direttamente su file perchè la write non considera gli \0
		Devo quindi prendere l'array di stringhe e ogni volta che trovo il carattere \0 
		passare alla stringa successiva
		(si poteva usare come carattere anche lo \n, ma visto che prima ho usato \0 
		ho tenunto la stessa logica)
		*/
		for(a = 0; a < Y; a++){
			k = 0;
			
			while(1){
				c = tutteL[a][k];
				k++;
				if(c == '\0')
					break;
				write(CreatedFile, &c, 1);
			}
		}
	}

	//ATTESA TERMINAZIONE DEI FIGLI   
    /* Attesa della terminazione dei figli */
        
    for(y=0;y < Y;y++) {
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
