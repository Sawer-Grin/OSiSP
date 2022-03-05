#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<dirent.h>
#include<errno.h>
#include<limits.h>//realpath
#include<libgen.h>//basename

#include "stack.h"

char *AppName;


int SolvingTask(FILE *file_for_saving_r, char* realPath){
    DIR *nowDir;
    struct dirent *info_dir;
    struct stat info_file;

    char *viewed_path_file = malloc(strlen(realPath) + NAME_MAX + 2); // "/" + "\0"
    if (viewed_path_file == NULL){
        fprintf(stderr, ":%s %s", AppName, strerror(errno));
        return -1;
    }

    char *full_name_m_file = malloc(NAME_MAX);
    if (full_name_m_file == NULL){
        fprintf(stderr, ":%s %s", AppName, strerror(errno));
        return -1;
    }

    full_name_m_file[0] = "\0";
    long long size_all_files = 0, max_size_file = -1;
    int count_file = 0;
    struct stack_t* local_stack = newStack();

    push(local_stack, realPath);
    while( top(local_stack) != NULL){

        char *localPath = pop(local_stack);

        // open directory
        nowDir = opendir(localPath);
        if (nowDir == NULL){
            fprintf(stderr, "%s: %s: %s\n", AppName, localPath, strerror(errno));
            return -1;
        }

        while (info_dir = readdir(nowDir)){
            if (strcmp(".", info_dir->d_name) && strcmp("..", info_dir->d_name)){

                strcpy(viewed_path_file, localPath);
                strcat(viewed_path_file, "/");
                strcat(viewed_path_file, info_dir->d_name);

                if (lstat(viewed_path_file, &info_file) == -1){
                    fprintf(stderr, "%s: %s %s", AppName, localPath, strerror(errno));
                    return -1;
                }

                if (!S_ISDIR(info_file.st_mode)){
                    if (info_file.st_size > max_size_file){
                        max_size_file = info_file.st_size;
                        strcpy(full_name_m_file, basename(viewed_path_file));
                    }
                    size_all_files += info_file.st_size;
                    ++count_file;
                }
                if (S_ISDIR(info_file.st_mode)){
                    push(local_stack, viewed_path_file);
                }

            }
        }

        printf("Path: %s Amount of file: %ld Size all files: %d Name the lagest files: %s\n",
                localPath, count_file, size_all_files, full_name_m_file);
        fprintf(file_for_saving_r, "Path: %s Amount of file: %ld Size all files: %d Name the lagest files: %s\n",
                localPath, count_file, size_all_files, full_name_m_file);

        if(closedir(nowDir)==-1){
            fprintf(stderr,"%s: %s: %s\n", AppName, localPath, strerror(errno));
            return -1;
        }

    }
    printf("it's ending.\n");
}

int main(int argc, char** argv) {

    AppName = alloca(strlen(basename(argv[0])));
    strcpy(AppName, basename(argv[0]));
    if (argc < 3) {
        fprintf(stderr, "%s: You must enter three arguments:\n"
                       "1) directory from which to start calculations;\n"
                       "2) name of file for saving.\n", AppName);
        return -1;
    }

    FILE *file;
    if ((file = fopen(argv[2], "w")) == NULL){ // argv[2]
        fprintf(stderr,"%s: %s: %s\n", AppName, argv[2], strerror(errno));
        return -1;
    }

    if (realpath(argv[1], NULL) == NULL){ // argv[1]
        fprintf(stderr, "%s: %s: %s\n", AppName, argv[1], strerror(errno));
        return -1;
    }
    SolvingTask(file, realpath(argv[1], NULL));

    if (fclose(file) != 0){
        printf(stderr, "%s: %s: %s\n", AppName, argv[2], strerror(errno));
        return -1;
    }

    return 0;
}
