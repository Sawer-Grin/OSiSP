//
// Created by Richard on 3/28/22.
//

#ifndef OSISP_2_CALCULATE_WORDS_H
#define OSISP_2_CALCULATE_WORDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "stack.h"

#define MAX_SIZE_WORD 257 // word + '\0'


struct data_word{
    char *word;
    char last_position;
};

int checkChar(char ch){
    char uncorrect_ch [] = " ,.{[}]()!@#$%^&*()_+=-:;~`/\\";
    char *pch = strchr(uncorrect_ch, ch);
    if (pch == NULL)
        return INT_MAX;
    else
        return 0;
}


struct data_word *init_word(){
    struct data_word *temp = (struct data_word*)malloc(sizeof (struct data_word) * 1);
    temp->word = (char *) malloc(sizeof (char) * MAX_SIZE_WORD);
    temp->last_position = -1;
    return temp;
}

void set_word(struct data_word *data, const char *new_data){
    int length = strlen(new_data);
    if (data->word == NULL){
        perror("String in calculate_word.h is empty");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < length; ++i){
        if (checkChar(new_data[i]))
            data->word[i] = new_data[i];
    }
    data->last_position = length - 1;
}

char *copyString(char *str)
{
    char *tmp = (char *) malloc(strlen(str) + 1);
    if (tmp == NULL){
        perror("Problem with memory in copyString\n");
        exit(EXIT_FAILURE);
    }
    strcpy(tmp, str);
    return tmp;
}

void push_char(struct data_word *data, char element){
    char pos_last_elem = data->last_position;
    if ((pos_last_elem + 1 < MAX_SIZE_WORD) && checkChar(element)){
        data->word[pos_last_elem + 1] = element;
        data->word[pos_last_elem + 2] = '\0';
        ++(data->last_position);
    }
}

void delete_last_char(struct data_word *data){
    if (data == NULL){
        perror("Pointer data is NULL");
        exit(EXIT_FAILURE);
    }
    char pos_last_elem = data->last_position;
    if (pos_last_elem > -1){
        data->word[pos_last_elem] = '\0';
        --(data->last_position);
    }
}

char *get_word(struct data_word *data){
    char *copy_word = copyString(data->word);
    return copy_word;
}

void pop_word(struct data_word *data){
    if (data != NULL && (data->last_position > -1)){
        data->word[0] = '\0';
        data->last_position = -1;
    }
}

void free_word(struct data_word *data){
    if (data != NULL) {
        if (data->word != NULL) {
            free(data->word);
            data->word = NULL;
        }
        free(data);
        data = NULL;
    }
}

#endif //OSISP_2_CALCULATE_WORDS_H
