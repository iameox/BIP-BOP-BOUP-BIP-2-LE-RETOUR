#include <stdio.h>
#include <stdlib.h>
#include "src/functions.h"
#include "src/http.h"

/*void print_rules(Rule *r);
void print_concatenations(Concatenation *c);
void print_repetitions(Repetition *r);
void print_num_val(Num_val *n);
void print_num_val_set(Num_val_set *s);
void print_nodes(Node *n, int i);
void print_string(String *s);

void print_rules(Rule *r) {
    while (r != NULL) {
        printf("Name: "); print_string(r->rulename);
        printf("Concatenations:\n"); print_concatenations(r->concatenations);
        printf("\n");
        r = r->next;
    }
}

void print_concatenations(Concatenation *c) {
    while (c != NULL) {
        printf("Repetitions: \n"); print_repetitions(c->repetitions);
        c = c->next;
    }
}

void print_repetitions(Repetition *r) {
    while (r != NULL) {
        printf("Min: %d / Max: %d\n", r->min, r->max);
        if (r->type == RULENAME) { printf("Rulename: "); print_string(r->content.rulename); }
        else if (r->type == GROUP) { printf("Group:\n"); print_concatenations(r->content.concatenations); }
        else if (r->type == OPTION) { printf("Option:\n"); print_concatenations(r->content.concatenations); }
        else if (r->type == CHAR_VAL) { printf("Char var: "); print_string(r->content.char_val); }
        else { printf("Num var:\n"); print_num_val(r->content.num_val); }
        r = r->next;
    }
}

void print_num_val(Num_val *n) {
    if (n->type == SET) { printf("Set: "); print_num_val_set(n->value.set); }
    else printf("Min: %d / Max: %d\n", n->value.min_max[0], n->value.min_max[1]);
}

void print_num_val_set(Num_val_set *s) {
    while (s != NULL) {
        printf("%d ", s->value);
        s = s->next;
    }
    printf("\n");
}

void print_string(String *s) {
    int i;
    for (i = 0; i < s->length; i++) {
        if (is_vchar(*(s->base+i)) || *(s->base+i) == ' ') printf("%c", *(s->base+i));
        else printf("<0x%02X>", (unsigned char) *(s->base+i));
    }

    printf("\n");
}

void print_nodes(Node *n, int i) {
    int j;
    while (n != NULL) {
        for (j = 0; j < i; j++) { printf(" "); } printf("name: "); print_string(n->rulename);
        for (j = 0; j < i; j++) { printf(" "); } printf("content: "); print_string(n->content);
        if (n->children != NULL) { for (j = 0; j < i; j++) { printf(" "); } printf("children:\n"); print_nodes(n->children, i + 2); }
        n = n->next;
    }
}*/

int main (int argc, char *argv[]) {
    if (argc <= 1) exit_on_error("Pas assez d'arguments");

    int size = get_file_size(argv[1]);
    char *input = NULL;
    Node *tree = NULL;

    read_file(argv[1], &input, size);
    if (!parse_http(&tree, input, size)) ;//printf("Requête invalide.\n");
    //else print_nodes(tree, 0);
    //print_rules(rulelist);
    //printf("\n");

    delete_nodes(&tree);
    delete_rulelist(&rulelist);
    free(input);
    free(abnf_buffer);
}