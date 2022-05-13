#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include <sys/time.h>
#include <time.h>

#define PROCESS_COUNT 9
#define AMOUNT_PAIRS 9
#define SIGDEF -2
#define NOSIG -1
#define AMOUNT_OF_SIGNAL 2

typedef struct {
    int flag;
    int signal;
    int from;
    int to;
} Pair;

Pair queue[] = {
        {1, NOSIG, 0, 1},
        {1, SIGUSR1, 1, 2},
        {1, SIGUSR1, 1, 3},
        {1, SIGUSR1, 1, 4},
        {1, SIGUSR1, 1, 5},
        {1, SIGUSR1, 5, 6},
        {1, SIGUSR1, 5, 7},
        {1, SIGUSR1, 5, 8},
        {0, SIGUSR2, 8, 1}
};

int queue_index;
int node_index;
pid_t array_of_pid[PROCESS_COUNT];

int amount_of_sent_signals[AMOUNT_OF_SIGNAL];

int find_children_PGID(int index, pid_t *pgid, int *sig){
    for (int i = 0; i < PROCESS_COUNT; ++i){
        if (queue[i].from == index){
            *pgid = getpgid(array_of_pid[queue[i].to]);
            if (sig != NULL)
                *sig = queue[i].signal;
            return 1;
        }
    }
    return 0;
}

void print_data_time(char *status, int index_signal){
    struct timeval current_time;
    struct tm *today;
    gettimeofday(&current_time, NULL);
    today = localtime(&current_time.tv_sec);

    printf("%-5d %-5d %-5d %6s   USR%1d Time: %d:%0d:%0d:%ld\n",
           node_index,
           getpid(),
           getppid(),
           status,
           index_signal,
           today->tm_hour,
           today->tm_min,
           today->tm_sec,
           current_time.tv_usec
    );
    fflush(stdout);
}

int signal_children(int index, int sig){
    int s;
    pid_t pgid;
    if (find_children_PGID(index, &pgid, &s)){
        int signal = (sig == SIGDEF ? s : sig);
        kill(-pgid, signal);
        return signal;
    }
    return NOSIG;
}

void sig_for_user(int sig){
    print_data_time("get", sig == SIGUSR1 ? 1 : 2);

    if((node_index == 1) && (amount_of_sent_signals[0] + amount_of_sent_signals[1]) == 110) {
        kill(getpid(), SIGTERM);
        return;
    }

    int signal = signal_children(node_index, SIGDEF);
    if(signal != NOSIG) {
        int sigNum = signal == SIGUSR1 ? 1 : 2;
        print_data_time("put", sigNum);
        amount_of_sent_signals[sigNum - 1]++;
    }
}

void sigterm(int) {
    printf("%-5d %-5d %-5d has terminated. Sended: %4d SIGUSR1 and %4d SIGUSR2\n",
           node_index, getpid(), getppid(), amount_of_sent_signals[0], amount_of_sent_signals[1]);


    pid_t pgid;
    if(find_children_PGID(node_index, &pgid, NULL)) {
        kill(-pgid, SIGTERM);
        waitpid(-pgid, NULL, 0);
    }

    exit(0);
}

int main(int argc, char* argv[])
{
    array_of_pid[0] = getpid();
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO; // i don't understand, for what

    int pgid = 0;
    for (int i = 0; i < AMOUNT_PAIRS; ++i){
        if (i == 0){
            pgid = 0; // problem
        }

        if (node_index == queue[i].to && queue[i].signal != NOSIG){
            signal(queue[i].signal, sig_for_user);
        }

        if (node_index == queue[i].from && queue[i].flag) {
            pid_t pid;
            switch (pid = fork()) {
                case -1:
                    perror("Problem with fork");
                    kill(0, SIGKILL);
                    exit(EXIT_FAILURE);
                case 0:
                    node_index = queue[i].to;
                    array_of_pid[node_index] = getpid();
                    i = -1;

                    signal(SIGTERM, sigterm);
                    break;
                default:
                    if (!pgid)
                        pgid = pid;

                    setpgid(pid, pgid);
                    array_of_pid[queue[i].to] = pid;
                    break;
            }
        }
    }
    if (node_index == 0)
        printf("INDEX PID   PPID  STATUS SIGNAL TIME\n");

    sleep(1);

    if (node_index == 1){
        int sig = signal_children(node_index, SIGDEF);
        if (sig != NOSIG){
            int type_sig = (sig == SIGUSR1 ? 1 : 2);
            ++amount_of_sent_signals[type_sig - 1];
        }
    }

    if(node_index == 0){
        getchar();
        while(wait(NULL) != -1);
        return 0;
    }
    else {
        while(1)
            pause();
    }

}