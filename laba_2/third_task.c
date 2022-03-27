#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<errno.h>

int main(int argc, char *argv[]){

    if (argc < 3){
        fprintf(stderr, "usage: %s <src_file> <dist_file>", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }

    FILE* file_1 = fopen(argv[1], "r");

    if (file_1 == NULL){
        fprintf(stderr, "Error: can't open file %s", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    FILE* file_2 = fopen(argv[2], "w");

    if (file_2 == NULL){
        fprintf(stderr, "Error: can't open/create file %s", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }

    struct stat copy_stat;
    if (stat(argv[1], &copy_stat)){
        fprintf(stderr, "Promblem with stat%\n", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }
    if (chmod(argv[2], copy_stat.st_mode)){
        fprintf(stderr, "Problem with coping st_mode%\n", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }

    char symb;
    while ((symb = fgetc(file_1)) != EOF){
        fputc(symb, file_2);
    }

    if (fclose(file_1)){
        fprintf(stderr, "Problem with closing first file.\n", strerror(errno));
        return EXIT_FAILURE;
    };
    if (fclose(file_2)){
        fprintf(stderr, "Problem with closing second file,\n", strerror(errno));
        return EXIT_FAILURE;
    }
}