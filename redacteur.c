#include <sys/types.h> /* key_t  */
#include <sys/ipc.h>   /* ftok   */
#include <sys/sem.h>   /* semget, semctl, semop */
#include <sys/stat.h>  /* stat, struct stat  */
#include <stdlib.h>    /* malloc */
#include <stdio.h>     /* perror, printf */
#include <errno.h>     /* errno */
#include <unistd.h>    /*sleep*/

/* retourne -1 en cas d'erreur */
int P(int semid, int noSem)
{
    struct sembuf Ops[1];
    int ok;

    // Opération P sur le sémaphore noSem
    Ops[0].sem_num = noSem;
    Ops[0].sem_op = -1;
    Ops[0].sem_flg = 0;

    // Appel à la fonction semop pour réaliser l'opération P.
    ok = semop(semid, Ops, 1);
    if (ok == -1)
    {
        perror("P");
        return ok;
    }

    return ok;
}

/* retourne -1 en cas d'erreur */
int V(int semid, int noSem)
{
    struct sembuf Ops[1];
    int ok;

    // Opération V sur le sémaphore noSem.
    Ops[0].sem_num = noSem;
    Ops[0].sem_op = 1;
    Ops[0].sem_flg = 0;

    // Appel à la fonction semop pour réaliser l'opération V.
    ok = semop(semid, Ops, 1);
    if (ok == -1)
    {
        perror("V");
        return ok;
    }

    return ok;
}

void ecrire(char *nomFichier)
{
    FILE *f;
    f = fopen(nomFichier, "a");
    fprintf(f, "Bonjour");
    fclose(f);
}

int main(int argc, char *argv[])
{
    // On récupère la clé du sémaphore.
    // PRJVAL = 1.
    key_t cle;
    cle = ftok("redacteur.c", 1);

    // Récupération des sémaphores.
    int semid;
    semid = semget(cle, 2, 0666);

    // Programme principal :
    int donnee = 0;
    char *fichier="fichier.txt";
    // Le Rédacteur écrit dans le fichier.
    P(semid, donnee);
    ecrire(fichier);
    sleep(3);
    V(semid, donnee);

    // On supprime le sémaphore.
    // semctl(semid, 0, IPC_RMID);
    return 0;
}