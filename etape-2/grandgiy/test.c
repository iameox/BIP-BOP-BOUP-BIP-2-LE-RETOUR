#include <stdio.h>
#include "functions.h"
#include "abnf.h"
#include "http.h"

void print_rules(rule *r);
void print_concatenations(concatenation *c);
void print_repetitions(repetition *r);
void print_num_val(num_val *n);
void print_num_val_set(num_val_set *s);
void print_nodes(node *n, int i);
void print_string(string *s);

void print_rules(rule *r) {
    while (r != NULL) {
        printf("Name: "); print_string(r->rulename);
        printf("Concatenations:\n"); print_concatenations(r->concatenations);
        printf("\n");
        r = r->next;
    }
}

void print_concatenations(concatenation *c) {
    while (c != NULL) {
        printf("Repetitions: \n"); print_repetitions(c->repetitions);
        c = c->next;
    }
}

void print_repetitions(repetition *r) {
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

void print_num_val(num_val *n) {
    if (n->type == SET) { printf("Set: "); print_num_val_set(n->value.set); }
    else printf("Min: %d / Max: %d\n", n->value.min_max[0], n->value.min_max[1]);
}

void print_num_val_set(num_val_set *s) {
    while (s != NULL) {
        printf("%d ", s->value);
        s = s->next;
    }
    printf("\n");
}

void print_string(string *s) {
    int i;
    for (i = 0; i < s->length; i++) {
        if (is_vchar(*(s->base+i)) || *(s->base+i) == ' ') printf("%c", *(s->base+i));
        else printf("<0x%02X>", (unsigned char) *(s->base+i));
    }

    printf("\n");
}

void print_nodes(node *n, int i) {
    int j;
    while (n != NULL) {
        for (j = 0; j < i; j++) { printf(" "); } printf("name: "); print_string(n->rulename);
        for (j = 0; j < i; j++) { printf(" "); } printf("content: "); print_string(n->content);
        if (n->children != NULL) { for (j = 0; j < i; j++) { printf(" "); } printf("children:\n"); print_nodes(n->children, i + 2); }
        n = n->next;
    }
}

int main (int argc, char *argv[]) {
    if (argc <= 1) exit_on_error("Pas assez d'arguments");

    int size = get_file_size(argv[1]);
    char *input = NULL;
    rule *rulelist = NULL;
    node *tree = NULL;

    read_file(argv[1], &input, size);
    if (!parse_http(&tree, &rulelist, input, size)) printf("Requête invalide.\n");
    else print_nodes(tree, 0);
    printf("\n");

    delete_nodes(&tree);
    delete_rulelist(&rulelist);
}