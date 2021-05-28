#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

typedef struct {
	char v1;			// deve contenere il carattere
	long int v2;		// numero di occorrenze
}strut;

void scambia(strut *s1, strut *s2){
	strut temp;

	temp.v1 = s1->v1;
	temp.v2 = s1->v2;
	
	s1->v1 = s2->v1;
	s1->v2 = s2->v2;
	
	s2->v1 = temp.v1;
	s2->v2 = temp.v2;
}

void bubbleSort(strut s[], int dim){
	int i; 
	bool ordinato = false;
	while (dim>1 && !ordinato){
		ordinato = true; /* hp: è ordinato */
		for (i=0; i<dim-1; i++)
		if (s[i].v2 > s[i+1].v2 ){
			scambia(&s[i],&s[i+1]);
			ordinato = false;
		}
		dim--;
	}
}

typedef int pipe_t[2];

int main (int argc, char **argv) {
	int N = 26;				// per il numero di figli effettivi
	int i;
	char carattere;
	int occ;				// per contare il numero di occorrenze nel file del carattere
	pipe_t *piped;
	int status;				// per il ritorno dei figli	
	int pid[26];				// array per i pid dei figli
	int nread;				// numero di caratteri letti da file
	char c;
	int fd;					// file descriptor per ogni figlio
	int ritorno;
	strut s[26];

	if (argc != 2) {			/* controllo sul numero di paramentri */
		puts("ERRORE hai inserito numero errato di parametri");
		exit(1);
	}

	printf("PADRE: %d\n\n", getpid());

	// creo le pipe 
	piped = malloc(N * sizeof(pipe_t));

	if(piped == NULL){
		puts("ERRORE nella malloc per la pipe");
		exit(2);
	}
	
	for (int i = 0; i < N; i++){
		if(pipe(piped[i]) < 0){
			puts("Errore nella creazione della pipe");
			exit(3);
		}
	}
	
	for (i = 0; i < N; i++){

		if ((pid[i] = fork()) < 0){
			puts("ERRORE nella fork padre-figlio");
			exit(2);
		}

		/* FIGLIO */
		if(pid[i] == 0){
			carattere = 97 + i;
			printf("Figlio %d con pid %d associato a carattere %c\n", i, getpid(), carattere);

			// chiudo tutte le pipe che non uso
			for (int j = 0; j < N; j++){
				if(i == 0 || j != (i-1))
					close(piped[i][0]);
				if(j!=i)
					close(piped[j][1]);			// chiudo la scrittura da figlio a padre se la pipe non è di questo figlio
			}

			
			fd = open (argv[1], O_RDONLY);			/* apro il file */
			occ = 0; 

			if(i != 0)
				read(piped[i-1][0], s, N*sizeof(s));

			while ((nread = read(fd, &c, 1))) {/* cerco il carattere nel file */
				if(c == carattere)
					occ++;
			}

			s[i].v1 = carattere;
			s[i].v2 = occ;

			write(piped[i][1], s, N*sizeof(strut));

			printf("Figlio %d carattere %c occ %ld\n", i, s[i].v1, s[i].v2);

			printf("ho finito correttamente il figlio %d\n", i);
			exit(c);

		}
	}

	/* PADRE */
	for (i = 0; i < N; i++){		// chiudo tutte le pipe che non servono nel padre
		close(piped[i][1]);
		if(i != (N-1))
			close(piped[i][0]);
	}

	read(piped[N-1][0], s, N*sizeof(strut));

	puts("PADRE:\n");
	for(int i = 0; i < N; i++){
		printf("FIglio %i carattere %c occ %ld\n", i, s[i].v1, s[i].v2);
	}

	/* ordino s in base al numero di occorrenze */
	bubbleSort(s, 26);

	puts("\nORDINATO:");
	for(int i = 0; i < N; i++){
		printf("FIglio %i carattere %c occ %ld\n", i, s[i].v1, s[i].v2);
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
			printf("il figlio %i con PID: %d ha ritornato %d %c", i, pid[i], ritorno, ritorno);
		}
		printf("\n");
	}

	exit(0);

} 
