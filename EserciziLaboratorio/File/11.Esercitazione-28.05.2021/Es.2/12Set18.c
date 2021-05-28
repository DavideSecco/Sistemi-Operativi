#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

#define max 512

typedef int pipe_t[2];

int main (int argc, char **argv) {
	int N;					// per il numero di file effettivi
	int i;
	pipe_t *piped_fp;
	pipe_t *piped_np;	
	int status;				// per il ritorno dei figli	
	int *pid;				// array per i pid dei figli
	int nread;				// numero di caratteri letti da file
	char c;					// memorizzo carattere letto
	int fd;					// file descriptor per ogni figlio
	int ritorno;
	int pidnipote;
	long int trasformazioni, tf, tn;

	if (argc < 3) {			/* controllo sul numero di paramentri */
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	N = argc - 1;

	printf("PADRE: %d\n\n", getpid());

	printf("Numero di file figli inseriti: %d\n\n", N);

	pid = malloc(N*sizeof(int));					// per salvare i pid dei figli
	piped_fp = malloc(N*sizeof(pipe_t));			// N pipe figlio --> padre
	piped_np = malloc(N*sizeof(pipe_t));			// N pipe nipote --> padre

	// creo le pipe figlio padre
	for(i = 0; i < N; i++){
		if((pipe(piped_fp[i])) < 0){
			puts("errore nella creazione della pipe figlio padre\n");
			exit(10);
		}
	}

	// creo le pipe nipote padre
	for(i = 0; i < N; i++){
		if((pipe(piped_np[i])) < 0){
			puts("errore nella creazione della pipe nipote padre\n");
			exit(10);
		}
	}

	for (i = 0; i < N; i++){

		if ((pid[i] = fork()) < 0){
			puts("ERRORE nella fork padre-figlio");
			exit(2);
		}

		/* FIGLIO */
		if(pid[i] == 0){
			printf("Ho creato il figlio con pid: %d\n", getpid());

			
			if ((pidnipote = fork()) < 0){
				puts("ERRORE nella fork padre-figlio");
				exit(2);
			}

			/* NIPOTE */
			if(pidnipote == 0){
				puts("sono un nipote\n");

				// chiudo tutte le pipe che non uso
				for (int j = 0; j < N; j++){
					close(piped_fp[i][0]);				// chiudo la lettura figlio -> padre
					close(piped_fp[j][1]);				// chiudo la lettura figlio -> padre
					close(piped_np[j][0]);				// chiudo la lettura nipote -> padre

					if(j!=i){
						close(piped_np[j][1]);			// chiudo la scrittura nipote -> padre per gli altri nipoti
				}

				if ((fd = open(argv[i+1], O_RDWR)) < 0){
					puts("ERRORE Nell'apertura del file nel nipote");
					exit(100);
				}

				trasformazioni = 0;
				while((nread = read(fd, &c, 1))){
					if(islower(c) != 0){			// cerco caratteri minuscoli
						c = toupper(c);
						lseek(fd, -1, SEEK_CUR);				
						// DEVO TORNARE INDIETRO
						write(fd, &c, 1);
						trasformazioni++;
					}
				}

				write(piped_np[i][1], &trasformazioni, sizeof(long int));

				exit(trasformazioni/256);

			}


			}

			// chiudo tutte le pipe che non uso nel FIGLIO
			for (int j = 0; j < N; j++){
				close(piped_fp[i][0]);				// chiudo la lettura FIGLIO -> PADRE
				close(piped_np[i][1]);				// chiudo la scrittura NIPOTE -> PADRE
				close(piped_np[j][0]);				// chiudo la scrittura NIPOTE --> PADRE

				if(j!=i){
					close(piped_fp[j][1]);			// chiudo la scrittura da PADRE -> FIGLIO di tutti gli altri
				}
			}

			if ((fd = open(argv[i+1], O_RDWR)) < 0){
					puts("ERRORE Nell'apertura del file nel nipote");
					exit(100);
			}

			trasformazioni = 0;
			while((nread = read(fd, &c, 1))){
				if(c >= 48 && c <= 57){			// cerco caratteri minuscoli
					c = ' ';			
					lseek(fd, -1, SEEK_CUR);
					write(fd, &c, 1);
					trasformazioni++;
				}
			}
			write(piped_fp[i][1], &trasformazioni, sizeof(long int));
			exit(trasformazioni/256);
		}
	}

	/* PADRE */
	for (i = 0; i < N; i++){		// chiudo tutte le pipe che non servono nel padre
		close(piped_np[i][1]);		// pipe in scrittura da NIPOTE --> PADRE
		close(piped_fp[i][1]);		// pipe in lettura da FIGLIO --> PADRE
	}
	
	for (i = 0; i < N; i++){
		read(piped_fp[i][0], &tf, sizeof(long int));		// leggo dal figlio
		read(piped_np[i][0], &tn, sizeof(long int));		// leggo dal padre

		printf("FIglio e nipote %d hanno fatto rispettivamente %ld %ld trasformazioni\n", i, tf, tn);

	}

	/* PADRE ASPETTA FIGLIO */

	for(i = 0; i < N; i++){
		pid[i] = wait(&status);
		if(pid[i] < 0){
			puts("ERRORE nell wait");
			exit(6);
		}
		
		if((status & 0xFF) != 0){
			printf("FIglio con pid %d terminato in modo anomalo\n", pid[i]);
		}
		else{
			ritorno=(int)((status>>8) & 0xFF);
			if(ritorno != 0)
				printf("il figlio ha ritornato %d, quindi vuol dice che lui o il nipote hanno riscontrato errori", status);
			else
				printf("il figlio  con PID: %d ha ritornato %d", pid[i], ritorno);
		}
		printf("\n");
	}

	exit(0);

} 
