#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define QUEUE_LENGTH 8

typedef struct Pair_tag{
    int from;
    int to;
} Pair;

Pair queue[] = {
        {0, 1},
        {1, 2},
        {1, 3},
        {1, 4},
        {1, 5},
        {5, 6},
        {5, 7},
        {5, 8}
};

int queue_index;
int node_index;

void print_data_time(){
    struct timeval current_time;
    struct tm *today;
    gettimeofday(&current_time, NULL);
    today = localtime(&current_time.tv_sec);

    printf("\tTime: %d:%0d:%0d:%ld\t",
           today->tm_hour,
           today->tm_min,
           today->tm_sec,
           current_time.tv_usec
    );
}

void action(int sig){
    ++queue_index;

    printf("N= %d  P: %d get SIGUSR1 ", getpid(), getppid());
    print_data_time();
    printf("\n");

    if (queue_index == QUEUE_LENGTH)
        return;
    if (queue[queue_index].from == node_index){
        printf("I'm burned! My pid: %d! My index: %d\n", getpid(), node_index);
        switch(fork()){
            case -1:
                perror("Problem with fork");
                kill(0, SIGKILL);
                exit(EXIT_FAILURE);
            case 0:
                signal(SIGUSR1, action);
                node_index = queue[queue_index].to;
                printf("I'm created. My pid: %d. My index: %d\n ", getpid(), node_index);
                kill(0, SIGUSR1);
                break;
            default:
                break;
        }
    }
}

int main(int argc, char* argv[]){
    queue_index = -1;
    signal(SIGUSR1, action);
    action(SIGUSR1);

    if (node_index == 0){
        getchar();
        kill(0, SIGKILL);
    }
    else{
        while(1) pause();
    }
    return 0;
}