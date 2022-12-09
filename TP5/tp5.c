#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void ex1()
{
    int cpu = sched_getcpu();
    if (cpu == -1)
    {
        perror("sched_getcpu");
        return;
    }

    while (1)
    {
        printf("CPU: %d\n", cpu);
        sleep(1);
        cpu = sched_getcpu();
    }
}

void ex2(int cpu)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    if (sched_setaffinity(0, sizeof(set), &set) == -1)
    {
        perror("sched_setaffinity");
        return;
    }
    ex1(); // Pour vérifier que le processus est bien sur le bon CPU
}

int main(int argc, const char *argv[])
{
    // ex1();
    if (argc != 2)
    {
        printf("Usage: %s <cpu>\n", argv[0]);
        return 1;
    }
    int cpu = atoi(argv[1]);
    ex2(cpu);
    return 0;
}