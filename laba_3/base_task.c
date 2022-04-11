#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

void stp_task_for(pid_t temp_pid){
    struct timeval current_time;
    struct tm *today;
    switch (temp_pid) {
        case -1:
            perror("Problem with first fork");
            exit(EXIT_FAILURE);
        case 0:
            gettimeofday(&current_time, NULL);
            today = localtime(&current_time.tv_sec);

            printf("PID: %d Parent PID: %d Time: %d:%0d:%0d:%ld\n",
                   getpid(),
                   getppid(),
                   today->tm_hour,
                   today->tm_min,
                   today->tm_sec,
                   current_time.tv_usec
            );
            exit(EXIT_SUCCESS);
    }
}

int main(){

    pid_t temp_pid;

    for (int i = 0; i < 2; ++i){
        temp_pid = fork();
        stp_task_for(temp_pid);
    }
    if (temp_pid > 0) {
        struct timeval current_time;
        struct tm *today;
        gettimeofday(&current_time, NULL);
        today = localtime(&current_time.tv_sec);

        printf("PID: %d Parent PID: %d Time: %d:%0d:%0d:%ld\n",
               getpid(),
               getppid(),
               today->tm_hour,
               today->tm_min,
               today->tm_sec,
               current_time.tv_usec
        );
        system("ps -x");
        while (wait(NULL) != -1);
        //system("ps -x");
    }
    return 0;
}