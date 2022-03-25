#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int main(int argc, char *argv[]){

    if (argc < 2){
        fprintf(stderr, "usaging: %s <file_name>\n", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }

    char* file_name = argv[1];

    FILE* file = fopen(file_name, "w");

    if (file == NULL){
        fprintf(stderr, "Error: can't open file %s\n", file_name, strerror(errno));
        return EXIT_FAILURE;
    }

    char symb;
    while ((symb = getc(stdin)) != 6){
        fputc(symb, file);
    }

    fclose(file);
}