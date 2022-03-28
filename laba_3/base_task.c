#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>

void stp_task_for(pid_t temp_pid){
    struct timeval current_time;
    struct tm *today;
    switch (temp_pid = fork()) {
        case -1:
            perror("Problem with first fork");
            exit(1);
        case 0:
            gettimeofday(&current_time, NULL);
            today = localtime(&current_time.tv_sec);

            printf("PID: %d Time: %d:%0d:%0d.%d\n",
                   getpid(),
                   today->tm_hour,
                   today->tm_min,
                   today->tm_sec,
                   current_time.tv_usec
            );
            exit(0);
    }
}

int main(){

    pid_t first_pid = fork();
    if (first_pid == 0){
        stp_task_for(first_pid);
    }
    else{
        pid_t second_pid = fork();
        if (second_pid == 0){
            stp_task_for(second_pid);
        }
        else{
            stp_task_for(second_pid);
            system("ps -x");
        }
    }
    return 0;
}