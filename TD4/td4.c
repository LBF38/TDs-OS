// inclut les librairies pour ce programme
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function(void *arg)
{
    int val = *(int *)arg;
    int n = 0;
    int *ret = malloc(sizeof(int));
    *ret = val;
    while (1)
    {
        fprintf(stderr, "New thread %d\n", &val);
        sleep(1);
    }
}

int main(void)
{
    pthread_t thr[n]; // pour plusieurs threads
    int args[n];
    int *val_ret;
    for (int j = 0; j < N; j++)
        args[j] = j;
    for (int i = 0; i < 5; i++)
    {
        if (pthread_create(&(thr[i]), NULL, thread_function, (void *)&args[i]) != 0)
        {
            fprintf(stderr, "Error during pthread_create()\n");
            exit(EXIT_FAILURE);
        }
    }
    while (1)
    {
        fprintf(stderr, "Main thread\n");
        sleep(1);
    }
}