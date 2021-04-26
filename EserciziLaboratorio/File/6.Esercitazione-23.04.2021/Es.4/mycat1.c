#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char **argv) {
    char buffer [BUFSIZ];
    int nread, fd = 0; // <== Inizializzato a 0: se non si passa un parametro, allora si legge da standard input!

    // non viene passato nessun parametro --> leggiamo da stdin
    if (argc == 1)  {
        while ((nread = read (fd, buffer, BUFSIZ)) > 0 )    /* lettura dal file o dallo standard input fino a che ci sono caratteri */
        write(1, buffer, nread);                        /* scrittura sullo standard output dei caratteri letti */
    }
    // viene passato uno o più file
   else {
        for(int i = 1; i<argc; i++){
            fd = open(argv[i], O_RDONLY);
            if ( fd < 0)
                printf("Errore in apertura file %s", argv[i]);
            else {
                printf("Visualizzio il file %s:\n", argv[i]);
                while ((nread = read (fd, buffer, BUFSIZ)) > 0 )
                    write(1, buffer, nread);
                printf("\n\n:::::::::::::::::::::::::::::\n\n");
            }
        }
   }
    /* se non abbiamo un parametro, allora fd rimane uguale a 0 */
    
    return 0;
}