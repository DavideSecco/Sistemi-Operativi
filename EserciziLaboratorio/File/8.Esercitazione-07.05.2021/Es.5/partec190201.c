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
	int N;
	int pid;
	int fd;
	int nread;
	char c;
	pipe_t numeri;
	pipe_t caratteri;
	bool numero=false;
	char str_char[max];
	char str_num[max];
	int j;

	/* controllo sul numero di parametri */
	if (argc < 3){
		puts("Errore sul numerdo di paramentri\n");
		exit(1);
	}

	N = argc - 1;					// salvo numero dei file
	printf("Numero di file: %d \n", N);
	
	if(pipe(numeri) < 0){
		puts("Errore nella creazione della pipe numeri\n");
		exit(2);
	}
	if(pipe(caratteri)<0) {
		puts("Errore nella creazione della pipe caratteri\n");
		exit(3);
	}

	/* generazione figli */ 
	for (int i = 0; i < N ; i++){
		if((pid = fork()) < 0){			// creo gli N figli
			puts("Errore nella fork\n");
			exit (2);
		}

		/* FIGLIO */
		if(pid == 0){
			
			if ((fd = open(argv[i+1], O_RDONLY)) < 0){ 	/* apro il file associato a questo figlio */
				puts("Errore apertura file\n");
				exit(3);
			}
			else
				printf("Aperto correttamente dal figlio il file: %s\n", argv[i+1]);

			
			/* chiudo pipe per ogni figlio */
			close(numeri[0]);			// chiudo in lettura
			close(caratteri[0]);		// chiudo in scrittura

			while((nread = read(fd, &c, 1)) > 0){		/* leggo dal file che ho aperto e scrivo sulla pipe giusta*/
				if(c >= 48 && c <= 57)		/* E' un numero */
					numero=true;
				else						/* E' una lettera */
					numero=false;

				if((i % 2 == 0) && (numero == true)) {					// file dispari (processo pari) --> caso giusto
					write(numeri[1], &c, sizeof(char));
				}	
				else if	((i % 2 != 0) && (numero == false))				// file dispari (processo pari) --> caso giusto
					write(caratteri[1], &c, sizeof(char));
			}
			printf("Ho finito di leggere dal file %s\n", argv[i+1]);
			exit(0);		// figlio terminato con successo

		}
	}

	/* PADRE */
	sleep(1);
	printf("Inizia codice padre\n");
	
	close(numeri[1]);									/* chiudo la pipe in scrittura NUMERI*/
	close(caratteri[1]);								/* chiudo la pipe in scrittura CARATTERI*/

	j=0;
	
	while(read(caratteri[0], &str_char[j], 1))			// leggo dalla pipe dei caratteri:
		j++;											// scrivo il carattere letto
	
	str_char[j]='\0';

	j=0;
	
	while(read(numeri[0], &str_num[j], 1))				// leggo dalla pipe dei numeri:
		j++;											// scrivo il carattere letto


	str_num[j]='\0';
	
	
	int size;
	if (strlen(str_char) <= strlen(str_num))
		size = strlen(str_char);
	else
		size=strlen(str_num);

	printf("\nSize = %d\n", size);

	for (int i = 0; i< size; i++){						/* il padre scrive su stdout 1 carattere e 1 numero alternandoli */
		printf("%c\n", str_num[i]);
		printf("%c\n", str_char[i]);
	}

}
