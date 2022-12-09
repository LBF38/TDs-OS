#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    int cpu = sched_getcpu();
    if (cpu == -1)
    {
        perror("sched_getcpu");
        return -1;
    }

    while (1)
    {
        printf("CPU: %d\n", cpu);
        sleep(1);
        cpu = sched_getcpu();
    }

    return 0;
}