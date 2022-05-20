//
// Created by Richard on 5/19/22.
//
#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include <alloca.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/time.h>
#include <time.h>

void *get_info(void *flag)
{
    struct timeval current_time;
    struct tm *today;
    gettimeofday(&current_time, NULL);
    today = localtime(&current_time.tv_sec);
    int local_int = *((int *) flag);

    printf("PID: %d\tParent\tPPID: %d\t ID: %lu\t Time: %d:%0d:%0d:%ld\n",
           getpid(),
           getppid(),
           pthread_self(),
           today->tm_hour,
           today->tm_min,
           today->tm_sec,
           current_time.tv_usec
    );

    if (*((int *) flag) == INT_MAX) {
       printf("First exp. Files are equals.\n");
        pthread_exit(0);
    }
}

int main(int argc, char *argv[])
{
    pthread_t first_thread, second_thread;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    int *flag = alloca(sizeof (*flag)),
        *second_flag = alloca(sizeof (*second_flag)),
        *first_flag = alloca(sizeof (*first_flag));
    *flag = 0, *second_flag = INT_MAX, *first_flag = INT_MAX;

    pthread_create(&first_thread, &attr, get_info, first_flag);
    pthread_create(&second_thread, &attr, get_info, second_flag);
    get_info(flag);

    pthread_join(first_thread,NULL);
    pthread_join(second_thread, NULL);
}