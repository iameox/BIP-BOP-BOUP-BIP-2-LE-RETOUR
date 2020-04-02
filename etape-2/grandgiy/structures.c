#include <stdlib.h>
#include "functions.h"
#include "structures.h"

rule *insert_rule(rule **head, char *rulename_base, int rulename_length) {
    rule *new = create_element(sizeof(rule)),
         *element = *head;

    new->rulename = create_string(rulename_base, rulename_length);
    new->concatenations = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

rule *find_rule(rule *head, char *rulename_base, int rulename_length) {
    string rulename = { rulename_base, rulename_length };
    rule *element = head,
         *value = NULL;

    while (element != NULL && !compare_strings(element->rulename, &rulename)) element = element->next;
    if (element != NULL) value = element;
    
    return value;
}

void delete_last_rule(rule **head) { 
    rule *element = *head,
         *previous = *head;

    if (element != NULL) {
        if (element->next == NULL) *head = NULL;
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        delete_string(&(element->rulename));
        delete_concatenations(&(element->concatenations));
        free(element);
    }
}

void delete_rulelist(rule **head) {
    while (*head != NULL) delete_last_rule(head);
}

concatenation *insert_concatenation(concatenation **head) {
    concatenation *new = create_element(sizeof(concatenation)),
                  *element = *head;


    new->repetitions = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_concatenation(concatenation **head) {
    concatenation *element = *head,
                  *previous = *head;

    if (element != NULL) {
        if (element->next == NULL) *head = NULL;
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        delete_repetitions(&(element->repetitions));
        free(element);
    }
}

void delete_concatenations(concatenation **head) {
    while (*head != NULL) delete_last_concatenation(head);
}

repetition *insert_repetition(repetition **head) {
    repetition *new = create_element(sizeof(repetition)),
               *element = *head;

    new->type = NO_REPETITION_TYPE;
    new->content.concatenations = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_repetition(repetition **head) {
    repetition *element = *head,
               *previous = *head;

    if (element != NULL) {
        if (element->next == NULL) *head = NULL;
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        if (element->type == RULENAME) delete_string(&(element->content.rulename));
        else if (element->type == GROUP || element->type == OPTION) delete_concatenations(&(element->content.concatenations));
        else if (element->type == CHAR_VAL) delete_string(&(element->content.char_val));
        else delete_num_val(&(element->content.num_val));
        free(element);
    }
}

void delete_repetitions(repetition **head) {
    while (*head != NULL) delete_last_repetition(head);
}

num_val *create_num_val() {
    num_val *new = create_element(sizeof(num_val));
    new->type = NO_NUM_VAL_TYPE;
    new->value.set = NULL;

    return new;
}

void delete_num_val(num_val **head) {
    num_val *element = *head;

    if (element != NULL) {
        if (element->type == SET) delete_set(&(element->value.set));

        *head = NULL;
        free(element);
    }
}

num_val_set *insert_set_value(num_val_set **head, int value) {
    num_val_set *new = create_element(sizeof(num_val_set)),
                *element = *head;

    new->value = value;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_set_value(num_val_set **head) {
    num_val_set *element = *head,
                *previous = *head;

    if (element != NULL) {
        if (element->next == NULL) *head = NULL;
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        free(element);
    }
}

void delete_set(num_val_set **head) {
    while (*head != NULL) delete_last_set_value(head);
}

node *insert_node(node **head, string *rulename, string *content) {
    node *new = create_element(sizeof(node)),
         *element = *head;

    new->rulename = rulename;
    new->content = content;
    new->children = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_node(node **head) {
    node *element = *head,
         *previous = *head;

    if (element != NULL) {
        if (element->next == NULL) *head = NULL;
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        delete_string(&(element->content));
        delete_nodes(&(element->children));
        free(element);
    }
}

void delete_nodes(node **head) {
    while (*head != NULL) delete_last_node(head);
}

string *create_string(char *base, int length) {
    string *new = create_element(sizeof(string));
    new->base = base;
    new->length = length;

    return new;
}

int compare_strings(string *s1, string *s2) {
    int i = 0;

    if (s1->length == s2->length) {
        while (i < s1->length && compare_insensitive(*(s1->base + i), *(s2->base + i))) i++;
    }

    return i == s1->length;
}

void delete_string(string **head) {
    string *element = *head;

    *head = NULL;
    free(element);
}