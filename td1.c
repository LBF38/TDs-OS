#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void ex1()
{
    int code_retour;
    printf("Debut du test fork()\n");
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Pbm lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        break;
    default:
        printf("Je suis le processus père\n");
        printf("Je viens de créer le processus fils dont le pid est % d \n", code_retour);
    }
    printf("code_retour % d\n", code_retour);
    printf("Fin du test fork()\n");
}

void printProcessusDetails()
{
    int sizePath = 100;
    char directoryPath[sizePath];
    if (getcwd(directoryPath, sizePath) == NULL)
    {
        printf("Error: Couldn't get directory path OR size is too small.\n");
    }
    else
    {

        printf("Répertoire de travail: %s\n", directoryPath);
    };
    printf("Processus ID: %d\n", getpid());
    printf("Processus père ID: %d\n", getppid());
    printf("Propriétaire réel: %d\n", getuid());
    printf("Propriétaire effectif: %d\n", geteuid());
    printf("Groupe propriétaire réel: %d\n", getgid());
    printf("Groupe propriétaire effectif: %d\n", getegid());
    printf("\n");
}

void ex2()
{
    int code_retour;
    printf("Debut du test fork()\n");
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Problème lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        printProcessusDetails();
        break;
    default:
        printf("Je suis le processus père\n");
        printProcessusDetails();
        printf("Je viens de créer le processus fils dont le pid est % d \n", code_retour);
    }
    printf("code_retour % d\n", code_retour);
    printf("Fin du test fork()\n");
}

int main()
{
    // ex1();
    ex2();
    return 0;
}
