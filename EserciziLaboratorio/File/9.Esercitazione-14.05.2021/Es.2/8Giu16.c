#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef int pipe_t[2];

int mia_random(int n) {

	srand(time(NULL));
	int casuale;
	casuale = rand() % n;
	return casuale;
}

int main (int argc, char **argv) { 
	int N;					// per il numero di file
	int H; 					// per l'ultimo intero int passato
	int Fcreato, pid;		// file creato, e pid
	pipe_t *piped_pf;
	pipe_t *piped_fp;
	int nread;
	char str[255];			// ci memorizzo le linee dei file letti e invio al padre la lunghezza
	int indice;				// indice che se possibile, mi dice che carattere stampare sul file
	int Linea;				// salvo le lunghezze delle linee compreso il terminatore
	int lungLinee[255];		// salvo le lunghezze delle linee dei vari file man mano per poi estrarli
	int fd;					// file descripter del file che apro
	int i;

	if (argc < 6) {	/* controllo sul numero di paramentri */
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	N = argc - 2;			// salvo numero di file

	H = atoi(argv[N +1]);			// salvo l'intero passato come parametro

	printf("I file sono: %d\n", N);
	
	/*CONTROLLO SUL FATTO CHE SIA UN INTERO ?*/

	if (H <= 0 || H >=255){
		puts("ERRORE: numero inserito fuori range");
		exit(2);
	}
	
	printf("Intero passato (sono il numero di linee dei file): %d \n", H);

	// questione numeri random

	// creo il file /tmp/creato 

	Fcreato = creat ("/tmp/creato.txt", O_WRONLY);

	/* CREO LE PIPE */

	piped_pf = malloc(N*sizeof(pipe_t));
	piped_fp = malloc(N*sizeof(pipe_t));

	for (int i = 0; i < N; i++){
		if(pipe(piped_pf[i]) != 0){
			puts("ERRORE creazione della pipe padre figlio");
			exit(4);
		}

		if(pipe(piped_fp[i]) != 0){
			puts("ERRORE creazione della pipe figlio padre");
			exit(4);
		}
	}
	
	puts("Creo gli N figli\n");

	for (i = 0; i < N; i++) {						// creo N filgi
		if((pid = fork()) < 0){
			puts("ERRORE nell fork");
			exit(3);
		}

		/* FIGLIO */
		if (pid == 0){
			
			printf("Inzio del %d figlio\n", i);

			if((fd = open(argv[i+1], O_RDONLY)) < 0){		// apro FILE in ogni figlio
				puts("ERRORE nell'apertura del file");
				exit (6);
			}

			for(int j = 0; j < N; j++){						/* CHIUDO LE PIPE CHE NON USO nel figlio */
				
				close(piped_pf[j][1]);				// da padre a figlio --> chiudo TUTTE in scrittura 
				close(piped_fp[j][0]);				// da figlio a padre --> chiudo TUTTE in lettura

				if(j != i){
					close(piped_pf[j][0]);				// da padre a figlio --> chiudo TUTTE in lettura, tranne quella che mi serve 
					close(piped_fp[j][1]);				// da figlio a padre --> chiudo TUTTE in scrittura, tranne quella chhe mi serve
				}
			}

			// LEGGO LINEA DAL FILE  E COMUNICO LA LUNGHEZZA DELLA LINEA
			int k=0;
			int Ncaratteri=0;
			int Nlinea=0;
			sleep(0.1);

			while((nread = read(fd, &str[k], 1)) > 0){
				if(str[k] == '\n'){
					str[k]='\0';
					Linea=strlen(str) + 1;

					printf("FIglio %d \t linea %d \t Invio la lunghezza: \t %d\n", i, Nlinea, Linea);

					write(piped_fp[i][1], &Linea, sizeof(int));			// invio al padre
					read(piped_pf[i][0], &indice, sizeof(int));			// leggo l'indice dal padre

					Nlinea++;
					sleep(0.3);

					if(indice < Linea){						// confronto l'indice del padre con la lunghezza della linea
						/* FUNZIONA? */ 
						write(Fcreato, &str[indice], sizeof(char));
						printf("FIglio %d ha scritto sul file: %c (ha riveuto %d)\n", i, str[indice], indice);
						Ncaratteri++;
					}
					else
						printf("Figlio %d non riesce a scrivere perchè l'indice ritornato è troppo alto\n", i);	
						/* non faccio nulla perchè la linea è troppo corta rispetto all'indice che è stato inviato */

					k=0;				// reinizializzo il contatore della stringa che salva la linea
				}
				else 
					k++;
				
			}
			// RITORNO AL PADRE IL NUMERO DI CARATTERI SCRITTI SUL FILE CREATO 
			exit(Ncaratteri);

		}
	}

	/* PADRE */
	
	/* chiudo le pipe */
	for (int i = 0; i<N; i++){
		close(piped_fp[i][1]);				// da figlio a padre --> leggo da tutti i figli --> NON CHIUDO UN BEL NIENTE!
		close(piped_pf[i][0]);				// da padre a figlio --> chiudo TUTTE in lettura (non leggo mai)
	}
	
	for(int k = 0; k < H; k++){						// dalla riga 0 alla riga H di ogni file
		sleep(1);
		printf("\nPADRE RIGA %d\n", k);
		for (int i = 0; i < N; i++){				// leggo dalle pipe le lunghezza delle linee dagli N file e li salvo nel vettore lungLinee
			read(piped_fp[i][0], &lungLinee[i], sizeof(int));
			printf("Ho ricevuto dal %d file: %d \n", i, lungLinee[i]);
		}
		
		int LungScelta = lungLinee[mia_random(N)];		// estraggo la lunghezza da prendere in considerazione (l'indice non sarà più grande di questa)
		int indiceScelto = mia_random(LungScelta);		// scelgo l'indice da inviare ai figli che al massimo sarà la LungScelta
		printf("Invio indice: %d\n\n", indiceScelto);

		for (int i = 0; i < N; i++){
			write(piped_pf[i][1], &indiceScelto, sizeof(int));						// invio a ogni figlio la lunghezza da leggere
		}
	}

}
