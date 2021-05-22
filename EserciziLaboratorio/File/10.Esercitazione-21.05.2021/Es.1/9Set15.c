#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#define max 512

typedef int pipe_t[2];

int main (int argc, char **argv) {
	int N;					// per il numero di file effettivi
	int i;
	pipe_t *piped_fp;
	pipe_t *piped_pf;	
	int status;				// per il ritorno dei figli	
	bool *statoFigli;		// array per ricordarsi quali figli sono ancora giusti
	int *pid;				// array per i pid dei figli
	int nread;				// numero di caratteri letti da file
	char c, cf;					// memorizzo carattere letto
	int fd;					// file descriptor per ogni figlio
	int ritorno;

	if (argc < 4) {			/* controllo sul numero di paramentri */
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	N = argc - 2;

	printf("PADRE: %d\n\n", getpid());

	printf("Numero di file figli inseriti: %d\n\n", N);

	printf("File padre da cui legge: %s\n", argv[N+1]);

	statoFigli = malloc(N*sizeof(bool));			// stato dei figli, per capire se li devo far leggere o meno

	for (i = 0; i < N; i++){						// setto a true tutti i figli
		statoFigli[i]=true;
	}

	pid = malloc(N*sizeof(int));					// per salvare i pid dei figli
	piped_fp = malloc(N*sizeof(pipe_t));			// N pipe figlio -> padre
	piped_pf = malloc(N*sizeof(pipe_t));			// N pipe padre --> figlio

	// creo le pipe figlio padre
	for(i = 0; i < N; i++){
		if((pipe(piped_fp[i])) < 0){
			puts("errore nella creazione della pipe figlio padre\n");
			exit(10);
		}
	}

	// creo le pipe padre figlio
	for(i = 0; i < N; i++){
		if((pipe(piped_pf[i])) < 0){
			puts("errore nella creazione della pipe figlio padre\n");
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

			// chiudo tutte le pipe che non uso
			for (int j = 0; j < N; j++){
				close(piped_fp[i][0]);				// chiudo la lettura figlio -> padre
				close(piped_pf[i][1]);				// chiudo la scrittura padre -> figlio

				if(j!=i){
					close(piped_pf[j][0]);			// chiudo la lettura dal padre se la pipe non è di questo figlio
					close(piped_fp[j][1]);			// chiudo la scrittura da figlio a padre se la pipe non è di questo figlio
				}
			}

			printf("Nel figlio %d (pid = %d) apro il file %s\n", i, getpid(), argv[i+1]);
			fd = open(argv[i + 1], O_RDONLY);		// apro il file del figlio

			while ((nread = read(piped_pf[i][0], &c, 1)) > 0) {			// leggo finche il padre mi invia caratteri (se è tutto ok mi inverà 'x' altrimenti nulla e sto fermo)
				if (c == 't'){										// se il carattere è t, vuol dire che il padre ha finito
					break;										
				}
				
				nread = read(fd, &c, 1);				// leggo il carattere dal file
				write(piped_fp[i][1], &c, 1);			// scrivo sulla pipe del figlio --> padre il carattere che ho letto

			}

			printf("ho finito correttamente il figlio %d\n", i);
			exit(0);

		}
	}

	/* PADRE */
	for (i = 0; i < N; i++){		// chiudo tutte le pipe che non servono nel padre
		close(piped_fp[i][1]);		// pipe in scrittura da figlio a padre
		close(piped_pf[i][0]);		// pipe in lettura da padre a figlio
	}
	sleep(1);
	// apro il file che il padre deve leggere
	printf("Apro file padre %s\n", argv[N+1]);
	int AF = open(argv[N+1], O_RDONLY);

	while((nread = read(AF, &c, 1)) > 0){			// finchè leggo caratteri dal file del padre
		printf("Dal file padre ho letto \t %c\n", c);
		for(i = 0; i < N; i++){						// per ogni figlio 
			if (statoFigli[i] == true){				// se il figlio è ancora valido
				write(piped_pf[i][1], "x", 1);		// invio al figlio il segnale che gli dice di leggere

				read(piped_fp[i][0], &cf, 1);		// leggo quello che il figlio mi hai inviato
				printf("Da figlio %d ho ricevuto \t %c\n", i, cf);
				// se il carattere letto dal padre nel suo file e quello inviato da figlio sono diversi, 
				// allora non dovrò più inviare nulla al figlio --> cambio il flag nel vettore
				if(c != cf){						
					statoFigli[i] = false;
				}
			}
		}
	}

	for (i = 0; i < N; i++) {				// ha tutti i figli ok, invio il carattere 't' così sanno che ho finito
		if(statoFigli[i] == true)
			write(piped_pf[i][1], "t", 1);
	}

	printf("Il padre ha finito i leggere i caratteri dal suo file\n");

	for(i = 0; i < N; i++){
		if (statoFigli[i] == false){				// TUTTI i file che non sono uguali a file AF vengono interrotti forzatamente
			printf("kill del figlio %d\n", i);
			if((kill(pid[i], SIGKILL)) == -1)
				printf("Errore nell kill del figlio %d\n", i);			
		}
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
