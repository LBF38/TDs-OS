#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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
    printf("Répertoire de travail: %s\n", getcwd(NULL, 0));
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

void ex3()
{
    int code_retour;
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Problème lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        printProcessusDetails();
        sleep(5);
        break;
    default:
        printf("Je suis le processus père\n");
        printProcessusDetails();
        printf("Je viens de créer le processus fils dont le pid est % d \n", code_retour);
    }
    printf("code_retour % d\n", code_retour);
    printf("Fin du test fork()\n");
}

void ex4()
{
    int code_retour;

    int file_descriptor = open("toto", O_RDWR);
    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error opening file : %s\n", strerror(errno));
        exit(-1);
    }
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Problème lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        printf("Fils: écriture fichier toto\n");
        char writeFils[] = "Fils";
        write(file_descriptor, writeFils, strlen(writeFils));
        sleep(2);
        printf("Fils: lecture fichier toto\n");
        char readTotoFils[100];
        int size_fils = read(file_descriptor, readTotoFils, 4);
        if (size_fils == -1)
        {
            fprintf(stderr, "Error reading file : %s\n", strerror(errno));
            exit(-1);
        }
        printf("Fils: %s\n", readTotoFils);
        break;
    default:
        printf("Je suis le processus père\n");
        sleep(1);
        printf("Père: lecture fichier toto\n");
        char readTotoPere[100];
        int size_pere = read(file_descriptor, readTotoPere, 4);
        if (size_pere == -1)
        {
            fprintf(stderr, "Error reading file : %s\n", strerror(errno));
            exit(-1);
        }
        readTotoPere[size_pere] = '\0';
        printf("Père: %s\n", readTotoPere);
        printf("Père: écriture fichier toto\n");
        char writePere[] = "Père";
        write(file_descriptor, writePere, strlen(writePere));
    }
}

void ex5()
{
    int code_retour;
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Problème lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        printProcessusDetails();
        sleep(5);
        break;
    default:
        printf("Je suis le processus père\n");
        printProcessusDetails();
        printf("Je viens de créer le processus fils dont le pid est % d \n", code_retour);
        wait(NULL);
    }
    printf("code_retour % d\n", code_retour);
    printf("Fin du test fork()\n");
}

void ex6a()
{
    int code_retour;
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Problème lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        printProcessusDetails();
        sleep(5);
        exit(3);
        break;
    default:
        printf("Je suis le processus père\n");
        printProcessusDetails();
        printf("Je viens de créer le processus fils dont le pid est % d \n", code_retour);
        int terminaison;
        pid_t child_pid = waitpid(code_retour, &terminaison, WCONTINUED);
        printf("Père: fin processus PID : %d\n", child_pid);
        printf("Père: terminaison fils : %d\n", terminaison);
        printf("Père: exit status : %d\n", WEXITSTATUS(terminaison));
    }
    printf("code_retour %d\n", code_retour);
    printf("Fin du test fork()\n");
}

void ex6b()
{
    int code_retour;
    code_retour = fork();
    switch (code_retour)
    {
    case -1:
        printf("Problème lors de la creation du processus\n");
        break;
    case 0:
        printf("Je suis le processus fils \n");
        printProcessusDetails();
        sleep(20);
        exit(3);
        break;
    default:
        printf("Je suis le processus père\n");
        printProcessusDetails();
        printf("Je viens de créer le processus fils dont le pid est % d \n", code_retour);
        int terminaison;
        pid_t child_pid = waitpid(code_retour, &terminaison, WCONTINUED);
        printf("Père: fin processus PID : %d\n", child_pid);
        printf("Père: terminaison fils : %d\n", terminaison);
        printf("Père: exit status : %d\n", WEXITSTATUS(terminaison));
    }
    printf("code_retour %d\n", code_retour);
    printf("Fin du test fork()\n");
}

void ex7()
{
    int processus, processus2;
    processus = fork();
    processus2 = fork();
    if (processus == -1)
    {
        printf("Problème lors de la création du processus fils 1\n");
    }
    else if (processus2 == -1)
    {
        printf("Problème lors de la création du processus fils 2\n");
    }
    else if (processus == 0)
    {
        printf("Je suis le processus fils 1\n");
        printProcessusDetails();
        printf("Fils 1: s'endort\n");
        sleep(5);
        printf("Fils 1: se termine\n");
        exit(1);
    }
    else if (processus2 == 0)
    {
        printf("Je suis le processus fils 2\n");
        printProcessusDetails();
        printf("Fils 2: s'endort\n");
        sleep(10);
        printf("Fils 2: se termine\n");
        exit(2);
    }
    else
    {
        printf("Je suis le processus père\n");
        printProcessusDetails();
        int terminaison;
        pid_t child_pid = waitpid(processus2, &terminaison, WCONTINUED);
        printf("Père: fin processus PID : %d\n", child_pid);
        printf("Père: terminaison fils : %d\n", terminaison);
        printf("Père: exit status : %d\n", WEXITSTATUS(terminaison));
    }
    printf("Fin du test fork()\n");
}

void ex9()
{
    char buf[80];

    printf("[%d]-- Début du test exec()\n", getpid());
    sprintf(buf, "--pid=[%d]", getpid());
    execl("/bin/echo", "echo", "Execution", "d'un test exec", buf, NULL);
    printf("[%d]-- Echec de la fonction execl()\n", getpid());
    printf("[%d]-- Fin du test exec()\n", getpid());
    exit(0);
}

int main()
{
    // ex1();
    // ex2();
    // ex3();
    // ex4();
    // ex5();
    // ex6a();
    // ex6b();
    // ex7();
    ex9();
    return 0;
}
