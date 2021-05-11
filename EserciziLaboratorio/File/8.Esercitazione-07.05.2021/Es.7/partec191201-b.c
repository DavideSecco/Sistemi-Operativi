 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

struct messaggio {
	char carattere;
	int num;
};

int main (int argc, char **argv) {
	int N;
	int pid;
	int fd;
	int nread;
	char c;
	int status;
	int piped[2];
	struct messaggio m;				// struct dove salvo i dati

	if (argc < 3){										// controllo sul numero di parametri
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	N = argc - 2;	// numero dei caratteri passati

	// devo controllare che siano caratteri
	for (int i = 0; i < N; i++){
		if(strlen(argv[i+2]) != 1){
			printf("ERRORE: %s non è un carattere\n", argv[i+2]);
			exit(4);
		}
	}

	if(pipe(piped) < 0){
		puts("ERRORE nella creazione della pipe");
		exit(2);
	}

	for (int i = 0; i < N; i++){							// ciclo per gli n figli
		if ((pid = fork()) < 0){
			printf("Errore nella creazione dell'%d figlio\n", i);
			exit (2);
		}
		
		/* FIGLIO */
		if (pid == 0){
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

			close(piped[0]);									// chiudo la pipe nel lato di lettura
			
			m.carattere = argv[i+2][0];
			m.num = count;

			write(piped[1], &m, sizeof(m));			// scrivo sulla pipe il valore da comunicare al padre
			exit (0);
			
		}
	}

	/* PADRE */
	puts("\nPADRE");
	
	close(piped[1]);					// chiudo tutte le pipe in scrittura nel padre
		
	for (int i = 0; i < N; i++){
		read(piped[0], &m, sizeof(m));
		printf("Carattere %c --> %d occorrenze\n", m.carattere, m.num);
	}

	for (int i = 0; i < N; i++){
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
