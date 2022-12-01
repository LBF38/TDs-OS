// inclut les librairies pour ce programme
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *calculerFactorielle(void *ptrVoidVal)
{
    int val = *(int *)ptrVoidVal;
    int *res = (int *)malloc(sizeof(int));

    *res = 1;
    while (val > 0)
        ;
    {
        *res = *res * val;
        val--;
    }
    pthread_exit((void *)res);
}

int main(int argc, char **argv)
{
    pthread_t tid1;
    pthread_t tid2;
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    int *retour1;
    int *retour2;
    if (pthread_create(&tid1, NULL, calculerFactorielle, (void *)&x) != 0)
        fprintf(stderr, "Error creating Thread\n");
    if (pthread_create(&tid2, NULL, calculerFactorielle, (void *)&y) != 0)
        fprintf(stderr, "Error creating Thread\n");

    pthread_join(tid1, (void *)&retour1);
    pthread_join(tid2, (void *)&retour2);

    printf("Factorielle de %d = %d et Factorielle de %d = %d \n", x, *retour1, y, *retour2);
    free(retour1);
    free(retour2);
    return 0;

    // à compléter avec code de Moodle.
}