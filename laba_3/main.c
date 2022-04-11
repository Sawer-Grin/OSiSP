#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include<sys/wait.h>

#include "calculate_words.h"
#include "simple_example.h"

char* execfile, *fullpath;
int max_num_proc, cur_num_proc;

char* getfullname(char* rootdirname, char* dirname)
{
    char* filepath = (char*)malloc(sizeof(char)*PATH_MAX);

    strcpy(filepath,rootdirname);
    strcat(filepath, "/");
    strcat(filepath, dirname);
    return filepath;
}

int getcountofwords(char* filepath)
{
    FILE* file;
    int c;
    int countofwords = 0;
    struct data_word *data_word = init_word();
    struct node *head;
    create_new_node(&head);

    file = fopen(filepath, "r");
    if (file == NULL)
    {
        fprintf(stderr, "%d : %s : %s :%s\n", getpid(), execfile, strerror(errno), filepath);
        return 1;
    }
    do
    {
        c=fgetc(file);
        push_char(data_word, (char)c);
        if (c!='\n' && c!=' ' && c!=EOF && c!='\t')
        {
            c = fgetc(file);
            countofwords++;
            while (c!='\n'&& c!=' ' && c!=EOF && c!='\t'){
                push_char(data_word, (char)c);
                c = fgetc(file);
            }
            char *new_word = get_word(data_word);
            pop_word(data_word);
            add_element(head, new_word);
            free(new_word);
        }
    }
    while (c!=EOF);
    if (fclose(file) == EOF)
        fprintf(stderr, "%d : %s : %s : %s\n", getpid(), execfile, strerror(errno), filepath);

    printf("-----------------File for Analyse: %s-----------------------\n", filepath);
    printf("List of unique word in file\n");
    countofwords = print_struct(head);
    printf("PID: %d Amount of word: %d\n", getpid(), countofwords);
    printf("----------------------------------------------------------------------------\n");
    free_word(data_word);
    free_node(head);
    return 0;
}

int getprocess(char* filepath)
{
    int result;
    pid_t pid;

    if (cur_num_proc >= max_num_proc)
    {
        if ((result = wait(NULL)) > 0)
            cur_num_proc--;
        if (result == -1)
        {
            fprintf(stderr, "%d %s: %s\n", getpid(), execfile, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    if (cur_num_proc < max_num_proc)
    {
        pid = fork();

        if ( pid < 0 )
        {
            fprintf(stderr, "%d : %s : %s\n", getpid(), execfile, strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if ( pid == 0 )
        {
            getcountofwords(filepath);
            exit(0);
        }
        else if ( pid > 0 )
            cur_num_proc++;
    }

    return 0;
}

int open_dir(char* dirname)
{
    DIR* dfd;
    struct dirent *dir;
    struct stat buf;
    char* filepath;
    int temperror;

    if ((dfd=opendir(dirname)) == NULL)
    {
        fprintf(stderr, "%d : %s : %s : %s\n", getpid(), execfile, dirname, strerror(errno));
        return 1;
    }

    while((dir = readdir(dfd)) != NULL) {
        filepath = getfullname(dirname, dir->d_name);
        if (((dir->d_type == DT_DIR))
            && ((strcmp(dir->d_name, ".")) != 0)
            && ((strcmp(dir->d_name, "..")) != 0)) {
            open_dir(filepath);
        } else if (dir->d_type == DT_REG){
            getprocess(filepath);
        }

        free(filepath);
        filepath = NULL;
    }
    if (errno == EBADF)
    {
        fprintf(stderr, "%d %s : %s\n", getpid(), execfile, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (closedir(dfd) != 0)
    {
        fprintf(stderr, "%d %s : %s\n", getpid(), execfile, strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(dir);
    return 0;
}

int main(int argc, char* argv[])
{
    int ret = 0;
    /*argc = 3;
    argv[1] = "/home/Richard/ForTest";
    argv[2] = "15";*/

    if (argc < 3){
        fprintf(stderr, "usage: %s <file_dir> <amount_of_processes>\n", argv[0], strerror(errno));
        exit(EXIT_FAILURE);
    }

    char *endptr, *char_setting = argv[2];
    char *result_real_path = realpath(argv[1], NULL);


    errno = 0;
    max_num_proc = strtoll(char_setting, &endptr, 10);
    if (errno != 0){
        fprintf(stderr, "Error: number is uncorrect %s\n", argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((char_setting == endptr) || (endptr - argv[2] < strlen(char_setting)))
    {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }

    cur_num_proc = 1;
    open_dir(result_real_path);
    while(cur_num_proc != 0) {
        ret = wait(NULL);
        cur_num_proc--;
    }

    free(result_real_path);
    return 0;
}