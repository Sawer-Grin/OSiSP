#ifndef OSISP_2_STACK_H
#define OSISP_2_STACK_H

#include <stdlib.h>
#include <string.h>

/**
 * Type for individual stack entry
 */
struct stack_entry {
    char *data;
    struct stack_entry *next;
};

/**
 * Type for stack instance
 */
struct stack_t
{
    struct stack_entry *head;
    size_t stackSize;  // not strictly necessary, but
    // useful for logging
};

/**
 * Create a new stack instance
 */
struct stack_t *newStack(void)
{
    struct stack_t *stack = malloc(sizeof *stack);
    if (stack)
    {
        stack->head = NULL;
        stack->stackSize = 0;
    }
    return stack;
}

/**
 * Make a copy of the string to be stored (assumes
 * strdup() or similar functionality is not
 * available
 */
char *copy_string(char *str) {
    char *tmp = NULL;
    if (str != NULL) {
        tmp = (char *) malloc(strlen(str) + 1);
        if (tmp == NULL) {
            perror("Problem with memory in copyString\n");
            exit(127);
        }
        strcpy(tmp, str);
    }
    return tmp;
}
/**
 * Push a value onto the stack
 */
void push(struct stack_t *theStack, char *value)
{
    struct stack_entry *entry = malloc(sizeof (struct stack_entry));
    if (entry)
    {
        entry->data = copy_string(value);
        entry->next = theStack->head;
        theStack->head = entry;
        theStack->stackSize++;
    }
    else
    {
        // handle error here
    }
}

/**
 * Get the value at the top of the stack
 */
char *top(struct stack_t *theStack)
{
    if ((theStack != NULL) && (theStack->head != NULL))
        return theStack->head->data;
    else
        return NULL;
}

/**
 * Pop the top element from the stack; this deletes both
 * the stack entry and the string it points to
 */
char* pop(struct stack_t *theStack)
{
    char* res = copy_string(top(theStack));

    if (theStack->head != NULL)
    {
        struct stack_entry *tmp = theStack->head;
        theStack->head = theStack->head->next;
        free(tmp->data);
        free(tmp);
        theStack->stackSize--;
    }
    return res;
}

/**
 * Clear all elements from the stack
 */
void clear (struct stack_t *theStack)
{
    char *temp;
    while (theStack->head != NULL) {
        temp = pop(theStack);
        free(temp);
    }
    free(theStack);
    theStack = NULL;
}

/**
 * Destroy a stack instance
 */
void destroyStack(struct stack_t **theStack)
{
    clear(*theStack);
    free(*theStack);
    *theStack = NULL;
}

#endif //OSISP_2_STACK_H