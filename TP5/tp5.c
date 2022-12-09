#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    int cpu = sched_getcpu();
    if(cpu == -1){
        perror("sched_getcpu");
        return -1;
    }
    printf("CPU: %d", cpu);

    return 0;
}