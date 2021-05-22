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

		}
	}

	/* PADRE */



	/* PADRE ASPETTA FIGLIO */

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
