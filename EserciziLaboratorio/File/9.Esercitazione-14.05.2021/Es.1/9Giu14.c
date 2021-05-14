#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#define max 512

typedef int pipe_t[2];

int main (int argc, char **argv) {
	int N;					// per il numero di file effettivi
	int pid, pid_nipote;	// per le fork 
	pipe_t piped_nf;		// pipe nipote-figlio (nipote scrive, figlio legge)
	pipe_t *piped_fp;		// pipe figlio-padre (figlio scrive, padre legge)
	char righe_c[11];			// numero per  le righe in char
	int i;
	int righe_i;			// numero delle righe ma in int
	long int somma = 0;		// somma per le righe
	int r;
	int status;
	int ritorno;

	if (argc < 3) {	/* controllo sul numero di paramentri */
		puts("ERRORE hai inserito pochi parametri");
		exit(1);
	}

	N = argc - 1;

	printf("PADRE: %d\n\n", getpid());

	printf("Numero di file inseriti: %d\n\n", N);

	piped_fp = malloc(N*sizeof(pipe_t));

	// creo le pipe padre figlio
	for(i = 0; i <N; i++){
		if((pipe(piped_fp[i])) < 0){
			puts("errore nella creazione della pipe figlio padre\n");
			exit(10);
		}
	}

	for (i = 0; i < N; i++){

		if ((pid = fork()) < 0){
			puts("ERRORE nella fork padre-figlio");
			exit(2);
		}

		/* FIGLIO */
		if(pid == 0){

			printf("FIGLIO %d con PID: %d\n", i, getpid());
				
			for (int k = 0; k <N; k++){	
				close(piped_fp[k][0]);						// chiudo la pipe figlio padre in lettura nel figlio per tutti
				if(k!=i)
					close(piped_fp[k][1]);					// chiudo in scrittura tutto tranne quella che mi serve
			}
			
			if((pipe(piped_nf)) < 0){				// apro la pipe figlio-nipote
				puts("ERRORE nell creazione della pipe");
				exit(4);
			}

			if((pid_nipote = fork()) < 0 ){			// creo il nipote
				puts("ERRORE nella fork figlio-nipote");
				exit(3);
			}

			/* NIPOTE */
			if(pid_nipote == 0){
				/*
				for (int k = 0; k <N; k++){	
					close(piped_fp[k][0]);				// chiudo la pipe figlio padre nel nipote
					close(piped_fp[k][1]);
				}
				*/
				close(piped_nf[0]);						// chiudi la lettura nel nipote
			
				printf("Nipote del figlio %d\n", i);
				close(1);				// chiudo lo stdout per il nipote perchè va messo sulla pipe
				dup(piped_nf[1]);		// duplico la pipe in scrittura al posto dello stdout

				close(piped_nf[1]);		// chiudo la pipe completamente tanto nel nipote non mi serve più

				execlp("wc", "wc", "-l", argv[i+1], (char *)0);
				puts("ERRORE nell'esecuzione del nipote\n");
				exit(-1);
			}

			close(piped_nf[1]);
			close(0);						// chiuso lo stdin
			dup(piped_nf[0]);				// al posto dello stdin metto il lato della pipe che legge

			close(piped_nf[0]);				// chiudo la pipe completamente tanto nel figlio non mi serve più

			int j =0;

			while((read(0, &righe_c[j], sizeof(char))) > 0){			// Leggo dallo stdin
				if(righe_c[j] == ' '){
					break;
				}
				else
					j++;
			}	
			righe_c[j]='\0';
			printf("Nel file %d --> righe: %s \n", i, righe_c);
		
			righe_i = atoi(righe_c);
			printf("invio il numero di linee: %d\n", righe_i);
			write(piped_fp[i][1], &righe_i, sizeof(int));


			pid_nipote = wait(&status);
			if(pid_nipote < 0){
				puts("ERRORE nell wait");
				exit(6);
			}

			if((status & 0xFF) != 0){
				printf("FIglio con pid %d terminato in modo anomalo\n", pid);
			}
			else{
				ritorno=(int)((status>>8) & 0xFF);
				if(ritorno != 0)
					printf("il figlio ha ritornato %d, quindi vuol dice che lui o il nipote hanno riscontrato errori", status);
				else
					printf("il figlio ha ritornato %d\n", ritorno);
			}

			exit(ritorno);	// uscita figlio --> che valore di ritorno devo mettere?
		}
	}

	/* PADRE */

	for (int k = 0; k <N; k++){
		close(piped_fp[k][1]);
	}	
	sleep(1);

	for (i = 0; i < N; i++) {
		read(piped_fp[i][0], &r, sizeof(int));
		printf("Ho ricevuto %d dal file %d\n", r, i);
		somma = r + somma;
	}

	printf("la somma totale delle righe è: %ld\n", somma);	

	printf("\nControllo i valori di ritorno:\n");

	for(int i = 0; i < N; i++){
		pid = wait(&status);
		if(pid < 0){
			puts("ERRORE nell wait");
			exit(6);
		}
		
		if((status & 0xFF) != 0){
			printf("FIglio con pid %d terminato in modo anomalo\n", pid);
		}
		else{
			ritorno=(int)((status>>8) & 0xFF);
			if(ritorno != 0)
				printf("il figlio ha ritornato %d, quindi vuol dice che lui o il nipote hanno riscontrato errori", status);
			else
				printf("il figlio  con PID: %d ha ritornato %d", pid, ritorno);
		}
		printf("\n");
	}

	exit(0);

} 
