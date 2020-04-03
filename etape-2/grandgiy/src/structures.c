#include <stdlib.h>
#include "functions.h"
#include "structures.h"

Rule *insert_rule(Rule **head, char *rulename_base, int rulename_length) {
    Rule *new = create_element(sizeof(Rule)),
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

Rule *find_rule(Rule *head, char *rulename_base, int rulename_length) {
    String rulename = { rulename_base, rulename_length };
    Rule *element = head,
         *value = NULL;

    while (element != NULL && !compare_strings(element->rulename, &rulename)) element = element->next;
    if (element != NULL) value = element;
    
    return value;
}

void delete_last_rule(Rule **head) { 
    Rule *element = *head,
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

void delete_rulelist(Rule **head) {
    while (*head != NULL) delete_last_rule(head);
}

Concatenation *insert_concatenation(Concatenation **head) {
    Concatenation *new = create_element(sizeof(Concatenation)),
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

void delete_last_concatenation(Concatenation **head) {
    Concatenation *element = *head,
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

void delete_concatenations(Concatenation **head) {
    while (*head != NULL) delete_last_concatenation(head);
}

Repetition *insert_repetition(Repetition **head) {
    Repetition *new = create_element(sizeof(Repetition)),
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

void delete_last_repetition(Repetition **head) {
    Repetition *element = *head,
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

void delete_repetitions(Repetition **head) {
    while (*head != NULL) delete_last_repetition(head);
}

Num_val *create_num_val() {
    Num_val *new = create_element(sizeof(Num_val));
    new->type = NO_NUM_VAL_TYPE;
    new->value.set = NULL;

    return new;
}

void delete_num_val(Num_val **head) {
    Num_val *element = *head;

    if (element != NULL) {
        if (element->type == SET) delete_set(&(element->value.set));

        *head = NULL;
        free(element);
    }
}

Num_val_set *insert_set_value(Num_val_set **head, int value) {
    Num_val_set *new = create_element(sizeof(Num_val_set)),
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

void delete_last_set_value(Num_val_set **head) {
    Num_val_set *element = *head,
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

void delete_set(Num_val_set **head) {
    while (*head != NULL) delete_last_set_value(head);
}

Node *insert_node(Node **head, String *rulename, String *content) {
    Node *new = create_element(sizeof(Node)),
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

Node *find_node(Node *head, String *rulename, int *index) {
    Node *element = head,
         *value = NULL;

    while (element != NULL && value == NULL) {
        if (!compare_strings(element->rulename, rulename)) {
            if (index == 0) value = element;
            else (*index)--;
        }

        value = find_node(element->children, rulename, index);
        element = element->next;
    }

    return value;
}

void delete_last_node(Node **head) {
    Node *element = *head,
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

void delete_nodes(Node **head) {
    while (*head != NULL) delete_last_node(head);
}

String *create_string(char *base, int length) {
    String *new = create_element(sizeof(String));
    new->base = base;
    new->length = length;

    return new;
}

int compare_strings(String *s1, String *s2) {
    int i = 0;

    if (s1->length == s2->length) {
        while (i < s1->length && compare_insensitive(*(s1->base + i), *(s2->base + i))) i++;
    }

    return i == s1->length;
}

void delete_string(String **head) {
    String *element = *head;

    *head = NULL;
    free(element);
}