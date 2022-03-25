#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>


int print_directory(char const *dir_name){
    DIR *current_dir = opendir(dir_name);
    if(!current_dir){
        fprintf(stderr, "problem with opening directory", strerror(errno));
        return -1;
    }

    struct dirent *d;
    while((d = readdir(current_dir))){
        printf("%s\n", d->d_name);
    }

    if(closedir(current_dir)){
        fprintf(stderr, "problem with closing directory", strerror(errno));
        return -1;
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    printf("Current directory:\n");
    print_directory("./");
    printf("---------------------\n");
    printf("Root directory:\n");
    print_directory("/");
    return 0;
}