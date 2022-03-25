#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int main (int argc, char *argv[]){

    long setting = 0;

    if (argc < 3){
        fprintf(stderr, "usage: %s <file_name> <amount_of_output_strings>\n", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }

    char *file_name = argv[1], *endptr, *char_setting = argv[2];


    errno = 0;
    setting = strtoll(char_setting, &endptr, 10);
    if (errno != 0){
        fprintf(stderr, "Error: number is uncorrect %s\n", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }

    if ((char_setting = endptr) || (endptr - argv[2] < strlen(char_setting)))
    {
        fprintf(stderr, "No digits were found\n");
        return EXIT_FAILURE;
    }

    FILE* file = fopen(file_name, "r");
    if (file == NULL){
        fprintf(stderr, "Error: can't open file %s", argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    char symb;
    int amnt_of_lines = 0;
    while ((symb = fgetc(file)) != EOF){
        if (symb == '\n')
            amnt_of_lines++;
        if (amnt_of_lines == setting && setting != 0){
            getc(stdin);
            amnt_of_lines = 0;
        }
        putc(symb, stdout);
    }

    if (fclose(file) == EOF){
        fprintf(stderr, "Problem with closing file", strerror(errno));
        return EXIT_FAILURE;
    }
}