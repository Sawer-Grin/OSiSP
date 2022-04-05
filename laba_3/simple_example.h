//
// Created by Richard on 4/2/22.
//

#ifndef TESTMYARRAU_SIMPLE_EXAMPLE_H
#define TESTMYARRAU_SIMPLE_EXAMPLE_H
#define SIZE_OF_STRUCT 256

#include <stdlib.h>

#include "calculate_words.h"
#include "stack.h"

struct node {
    short int amount_of_repeats;
    struct node *next_elements[SIZE_OF_STRUCT];
};

void create_new_node(struct node **new_head){
    struct node *temp = (struct node*) malloc(sizeof (struct node) * 1);
    if (temp == NULL){
        perror("Problem with crating new node in func initialize_struct");
        //free(temp);
        exit(127);
    }

    for (int i = 0; i < SIZE_OF_STRUCT; ++i){
        temp->next_elements[i] = NULL;
    }
    temp->amount_of_repeats = 0;

    *new_head = temp;
}

void add_element(struct node *head, char* word){
    if (strlen(word) == 0)
        return;
    unsigned long length_of_new_element = strlen(word);
    struct node *temp = head;
    for (int i = 0; i < length_of_new_element; ++i){
        int next_index = (int)word[i];
        if (temp->next_elements[next_index] == NULL){
            create_new_node(&(temp->next_elements[next_index]));
        }
        temp = temp->next_elements[next_index];
    }
    ++temp->amount_of_repeats;
}

struct node *find_grade(struct node *head, char *data_word){
    unsigned short length_word = strlen(data_word);
    struct node *result = head;
    for (int i = 0; i < length_word; ++i){
        int index = (int)data_word[i];
        if (result->next_elements[index] != NULL){
            result = result->next_elements[index];
        }
    }
    return result;
}

int print_struct(struct node *head){
    struct stack_t *stack = newStack();
    struct data_word *temp_word = init_word();
    struct node *temp_head = head;
    int result = 0;
    do {
        char *temp = pop(stack);
        if (temp != NULL){
            temp_head = find_grade(head, temp);
            set_word(temp_word, temp);
        }
        free(temp);

        if (temp_head->amount_of_repeats > 0){
            printf("Word : %s  Amount of repeats: %d\n", temp_word->word, temp_head->amount_of_repeats);
            result += temp_head->amount_of_repeats;
        }

        for (int i = 0; i < SIZE_OF_STRUCT; ++i){
            if (temp_head->next_elements[i] != NULL){
                push_char(temp_word, (char)i);
                push(stack, temp_word->word);
                //print_struct(head->next_elements[i], data_word);
                delete_last_char(temp_word);
            }
        }
    } while (top(stack) != NULL);

    clear(stack);
    free_word(temp_word);
    return result;
}

void free_node(struct node *head){
    for(int i = 0; i < SIZE_OF_STRUCT; ++i){
        if (head->next_elements[i] != NULL)
            free_node(head->next_elements[i]);
    }
    if (head != NULL)
        free(head);
}


#endif //TESTMYARRAU_SIMPLE_EXAMPLE_H
