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

void lire(char *nomFichier)
{
    FILE *f;
    char c;
    f = fopen(nomFichier, "r");
    while (fscanf(f, "%c", &c) != EOF)
    {
        printf("%c", c);
    }
    printf("\n");
    fclose(f);
}

int lireNbLecteurs(char *nomFichier)
{
    FILE *f;
    int nb;
    f = fopen(nomFichier, "r");
    fscanf(f, "%d", &nb);
    fclose(f);
    printf("nbLecteur = %d\n", nb);
    return nb;
}

void ecrireNbLecteurs(char *nomFichier, int nb)
{
    FILE *f;
    f = fopen(nomFichier, "w");
    fprintf(f, "%d", nb);
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
    int mutex_l = 1;
    int donnee = 0;
    int NbLecteurs;
    char *fichier_nombre = "nombre"; // Fichier contenant le nombre de lecteurs.
    char *fichier = "fichier.txt";
    // Lecture du fichier par les lecteurs.
    P(semid, mutex_l);
    NbLecteurs = lireNbLecteurs("nombre");
    NbLecteurs++;
    ecrireNbLecteurs(fichier_nombre, NbLecteurs);
    if (NbLecteurs == 1)
    {
        P(semid, donnee);
    }
    V(semid, mutex_l);

    sleep(3);
    
    lire(fichier);

    sleep(3);
    
    P(semid, mutex_l);
    NbLecteurs = lireNbLecteurs("nombre");
    NbLecteurs--;
    ecrireNbLecteurs(fichier_nombre, NbLecteurs);
    if (NbLecteurs == 0)
    {
        V(semid, donnee);
    }
    V(semid, mutex_l);

    // On supprime le sémaphore.
    // semctl(semid, 0, IPC_RMID);
    return 0;
}