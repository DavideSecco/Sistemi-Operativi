#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

typedef int pipe_t[2];

int main(int argc, char **argv){
	int n;			// numero caratteri
	int num; 		// numero occorrenze
	int pid, fd, nread;
	char c;
	int status;

	if (argc < 3){										// controllo sul numero di parametri
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	n = argc - 2;	// numero dei caratteri passati

	// devo controllare che siano caratteri
	for (int i = 0; i < n; i++){
		if(strlen(argv[i+2]) != 1){
			printf("ERRORE: %s non è un carattere\n", argv[i+2]);
			exit(4);
		}
	}

	pipe_t *piped = malloc(n*sizeof(pipe_t));			// creo il puntatore per le n pipe

	for (int i = 0; i < n; i++){
		if (pipe(piped[i]) < 0){
			puts("Errore nella pipe");
			exit (3);
		}
	}

	for (int i = 0; i < n; i++){							// ciclo per gli n figli
		if ((pid = fork()) < 0){
			printf("Errore nella creazione dell'%d figlio\n", i);
			exit (2);
		}
		
		/* FIGLIO */
		if (pid == 0){
			printf("Sto cercando %c\n", argv[i+2][0]);
			if ((fd = (open(argv[1], O_RDONLY))) < 0){		// apro il file in ogni figlio
				printf("Errore nell'apertura del file");
				exit(3);
			}

			int count = 0;
			while((nread = read(fd, &c, 1)) > 0) {				// ciclo di lettura di ogni figlio
				if ( c == argv[i+2][0]){						// occorrenza del carattere trovata
					count++;
				}		
			}
			
			for (int k = 0; k < n; k++){
				close(piped[k][0]);									// chiudo la pipe nel lato di lettura
				if(k != i)
					close(piped[k][1]);
			}
			
			write(piped[i][1], &count, sizeof(count));			// scrivo sulla pipe il valore da comunicare al padre
			exit (0);
		}
	}

	/* PADRE */
	puts("\nPADRE");
	for(int i = 0; i < n; i++){								// chiudo tutte le pipe in scrittura nel padre
		close(piped[i][1]);
	}

	for (int i = 0; i < n; i++){
		read(piped[i][0], &num, sizeof(num));
		printf("Carattere %s --> %d occorrenze\n", argv[i+2], num);
	}

	for (int i = 0; i < n; i++){
		pid = wait(&status);

		if (pid < 0){
			printf("ERRORE nell wait\n");
			exit (5);
		}

		if (status & 0xFF){
			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
		}
		else{
			int ritorno = (int)((status >> 8) & 0xFF);
			if (ritorno != 0){
				printf("C'è stato qualche problema nel figlio");
			}
			else
				printf("Il figlio con pid=%d ha ritornato %d\n", pid, ritorno);
		}

	}
}