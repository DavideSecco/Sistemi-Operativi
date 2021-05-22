#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

typedef struct {
	int indice; 			// equivale a c1
	int caratteriLetti;		// equivale a c2
} s_occ;

/* VARIBILI GLOBALI */
char riga[256];
s_occ strut;				// strut dati per passare i dati attuali
int NumeroStampe = 0;		// numero di stampe

void stampa(int sign){
	write(1, riga, strlen(riga));
	printf("\n");
	NumeroStampe = NumeroStampe + 1 ;
}

void nulla(int sign){
	/* NON DEVI FARE NULLA*/
}

typedef int pipe_t[2];

int main (int argc, char **argv) {
	int N;						// per il numero di file effettivi
	int H; 						// numero di linee dei file
	int *pid;
	pipe_t *piped;
	int i, j, k;				// contatori
	int nread;					// per capire quanti caratteri ho letto dal file
	s_occ strutprima;			// strut per leggere i dati passati
	
	int fd;
	int status = 0;
	int ritorno = 0;

	
	if (argc < 3) {	/* controllo sul numero di paramentri */
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	N = argc - 2;

	H = atoi(argv[N+1]);

	printf("PADRE: %d\n\n", getpid());

	printf("Numero di file inseriti: %d\n\n", N);

	printf("Numero Linee dei file %d\n", H);

	if(((strlen(argv[N+1])) != 1) || H < 1 || H > 254){			// controllo sul valore del numero di linee
		printf("ERRORE: il numero di linee non è valido\n");
		exit(2);
	}	

	pid = malloc(N*sizeof(int));			// per memorizzare i pid dei figli
	
	piped = malloc(N*sizeof(pipe_t));

	// creo le pipe
	for(i = 0; i < N; i++){
		if((pipe(piped[i])) < 0){
			puts("errore nella creazione della pipe padre\n");
			exit(10);
		}
	}

	for (i = 0; i < N; i++){

		if ((pid[i] = fork()) < 0){
			puts("ERRORE nella fork padre-figlio");
			exit(3);
		}

		/* FIGLIO */
		if(pid[i] == 0){
			
			for (int j = 0; j < N; j++){		// chiudo tutte le pipe che non mi servono
				if(i == 0){
					close(piped[i][0]);
				}
				
				if(j != i-1) 
					close(piped[j][0]);					// il primo figlio non legge da nessuna parte e si legge dal figlio precendete

				if (j != i)
					close(piped[j][1]);					// non vanno chiuse le pipe in scrittura dove j == i
			}

			fd = open(argv[i+1], O_RDONLY);		// aperto il file di ogni figlio

			printf("Sono il figlio %d PID = %d\n", i, getpid());

			for (k = 0; k < H; k++) {			// per ogni linea del file devo contare i numero di caratteri
				signal(SIGUSR1, stampa);
				signal(SIGUSR2, nulla);
				
				j = 0;
				while((nread = read(fd, &riga[j], 1)) > 0){
					if(riga[j] == '\n')
						break;
					else
						j++;
				}
				
				riga[j]='\0';

				strut.indice = i;
				strut.caratteriLetti = strlen(riga) + 1;

				
				if(i != 0){				// a meno di essere il primo nodo, devo leggere i dati della struct che mi invia il nodo precendete
					read(piped[i-1][0], &strutprima, sizeof(s_occ));
				}
				
				if(i == 0 || strut.caratteriLetti > strutprima.caratteriLetti){		// se questo è il primo nodo, oppure la struct attuale a più caratteri di quella arrivata
					write(piped[i][1], &strut, sizeof(s_occ));						// invio i dati della struct attuale al nodo (può essere padre o figlio) successivo
				} 						
				else{
					write(piped[i][1], &strutprima, sizeof(strutprima));	// la struct precendente è ancora la più lunga, allora passo quella
				}

				// una volta inviata dovrei ricevere dal padre l'indicazione se stampare o meno una delle mie righe
				pause();

				strutprima.indice=0;
				strutprima.caratteriLetti = 0;
				strut.indice=0;
				strut.caratteriLetti = 0;
			}

			printf("Numero di stampe figlio %d: %d\n", i, NumeroStampe);
			exit(NumeroStampe);			
		}
	}

	/* PADRE */
	printf("Padre INIZIO\n");

	for (i = 0; i < N; i++){		// chiudo le pipe che non usa il padre
		close(piped[i][1]);			// chiudo TUTTE le pipe in scrittura

		if(i != (N-1))
			close(piped[i][0]);		// chiudo tutte le pipe il lettura tranne l'ultima
	}

	for(i = 0; i < H; i++){

		read(piped[N-1][0], &strut, sizeof(strut));		// leggo la struct che mi è arrivata per ogni linea
		printf("PADRE: Riga %d ho ricevuto indice = %d Numero caratteri = %d\n", i, strut.indice, strut.caratteriLetti);

		for(j = 0; j < N; j++){
			sleep(0.5); /* per sicurezza */
			if(j == strut.indice) {
				if(kill(pid[strut.indice], SIGUSR1) == -1)
					printf("ERRORE NELLA KILL\n");
			}
			else {
				if(kill(pid[j], SIGUSR2) == -1)
					printf("ERRORE NELLA KILL\n");
			}
		}
	}


	/* PADRE ASPETTA FIGLIO */

	for(int f= 0; f < N; f++){
		int pidfiglio = wait(&status);
		if(pidfiglio < 0){
			puts("ERRORE nell wait");
			exit(6);
		}
		
		if((status & 0xFF) != 0){
			printf("FIglio con pid %d terminato in modo anomalo\n", pidfiglio);
		}
		else{
			ritorno=(int)((status>>8) & 0xFF);
			if(ritorno != 0)
				printf("il figlio ha ritornato %d, quindi vuol dice che lui o il nipote hanno riscontrato errori", status);
			else
				printf("il figlio  con PID: %d ha ritornato %d", pidfiglio, ritorno);
		}
		printf("\n");
	}

	exit(0);

} 
