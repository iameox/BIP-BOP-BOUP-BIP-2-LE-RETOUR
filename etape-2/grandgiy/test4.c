// ADD SUPPORT FOR COMMENTS AND PROSE-VAL

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define min(a, b) (a <= b ? a : b)

typedef enum repetition_type repetition_type;
typedef enum num_val_type num_val_type;
typedef enum opt_or_grp opt_or_grp;
typedef enum bin_or_dec_or_hex bin_or_dec_or_hex;

typedef struct string string;

typedef struct rule rule;
typedef struct concatenation concatenation;
typedef struct repetition repetition;
typedef struct num_val num_val;
typedef struct num_val_set num_val_set;

int is_crlf(char *s);
int is_wsp(char c);
int is_alpha(char c);
int is_digit(char c, int base);
int is_vchar(char c);
int char_to_int(char c);
int compare_strings(string *s1, string *s2);
string *create_string(char *base, int length);
void insert_in_num_val_set(num_val_set **head, int value);
num_val *create_num_val(num_val **head);
void delete_last_repetition(repetition **head);
repetition *create_repetition(repetition **head);
void delete_last_concatenation(concatenation **head);
concatenation *create_concatenation(concatenation **head);
void delete_last_rule(rule **head);
rule *find_rule(rule *head, string *rulename);
rule *create_rule(rule **head, string *rulename);
int get_bin_or_dec_or_hex_val(bin_or_dec_or_hex type, num_val *num_val, char *abnf, int *index);
int get_num_val(num_val **head, char *abnf, int *index);
int get_char_val(string **char_val, char *abnf, int *index);
int get_group_or_option(opt_or_grp type, concatenation **head, char *abnf, int *index);
int get_element(repetition *r, char *abnf, int *index);
int get_repeat(repetition *r, char *abnf, int *index);
int get_repetition(repetition **head, char *abnf, int *index);
int get_concatenation(concatenation **head, char *abnf, int *index);
int get_alternation(concatenation **head, char *abnf, int *index);
int get_c_nl(char *abnf, int *index);
int get_c_wsp(char *abnf, int *index);
int get_elements(concatenation **head, char *abnf, int *index);
int get_defined_as(char *abnf, int *index);
int get_rulename(string **rulename, char *abnf, int *index);
int get_rule(rule **head, char *abnf, int *index);
int get_rulelist(rule **head, char *file_name);

enum repetition_type {
    RULENAME,
    GROUP,
    OPTION,
    CHAR_VAL,
    NUM_VAL
};

enum num_val_type {
    SET,
    MIN_MAX
};

enum opt_or_grp {
    GRP,
    OPT
};

enum bin_or_dec_or_hex {
    BIN,
    DEC,
    HEX
};

struct string {
    char *base;
    int length;
};

struct rule {
    string *rulename;
    concatenation *concatenations;

    rule *next;
};

struct concatenation {
    repetition *repetitions;

    concatenation *next;
};

struct repetition {
    repetition_type type;
    int min;
    int max;
    union {
        string *rulename;
        concatenation *concatenations;
        string *char_val;
        num_val *num_val;
    } content;

    repetition *next;
};

struct num_val {
    num_val_type type;
    union {
        num_val_set *set;
        int min_max[2];
    } value;
};

struct num_val_set {
    int value;

    num_val_set *next;
};

rule *rulelist = NULL;

void print_num_val_set(num_val_set *s);
void print_num_val(num_val *n);
void print_repetitions(repetition *r);
void print_concatenations(concatenation *c);
void print_string(string *s);
void print_rules(rule *r);

void print_num_val_set(num_val_set *s) {
    while (s != NULL) {
        printf("%d ", s->value);
        s = s->next;
    }
    printf("\n");
}

void print_num_val(num_val *n) {
    if (n->type == SET) { printf("Set: "); print_num_val_set(n->value.set); }
    else printf("Min: %d / Max: %d\n", n->value.min_max[0], n->value.min_max[1]);
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

void print_concatenations(concatenation *c) {
    while (c != NULL) {
        printf("Repetitions: \n"); print_repetitions(c->repetitions);
        c = c->next;
    }
}

void print_string(string *s) {
    int i;
    for (i = 0; i < s->length; i++) printf("%c", *(s->base+i));
    printf("\n");
}

void print_rules(rule *r) {
    while (r != NULL) {
        printf("Name: "); print_string(r->rulename);
        printf("Concatenations:\n"); print_concatenations(r->concatenations);
        printf("\n");
        r = r->next;
    }
}

int is_crlf(char *s) {
    return *s == '\r' && *(s + 1) == '\n';
}

int is_wsp(char c) {
    return c == ' ' || c == '\t';
}

int is_alpha(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

int is_digit(char c, int base) {
    int return_value, max;

    if (base <= 10) return_value = '0' <= c && c <= '0' + base;
    else if (base <= 36) {
        max = base - 10;
        return_value = ('0' <= c && c <= '9')
            || ('A' <= c && c <= 'A' + max)
            || ('a' <= c && c <= 'a' + max);
    }

    return return_value;
}

int is_vchar(char c) {
    return 0x21 <= c && c <= 0xFE;
}

int char_to_int(char c) {
    int value = 0;

    if (is_digit(c, 10)) value = c - '0';
    else if (is_alpha(c)) {
        if (c <= 'Z') value = c - 'A' + 10;
        else value = c - 'a' + 10;
    }

    return value;
}

int compare_insensitive(char c1, char c2) {
    int return_value = 0,
        value1, value2;

    if (c1 == c2) return_value = 1;
    else if (is_alpha(c1)) {
        if (c1 <= 'Z') value1 = c1 - 'A';
        else value1 = c1 - 'a';

        if (c2 <= 'Z') value2 = c2 - 'A';
        else value2 = c2 - 'a';

        return_value = value1 == value2;
    }

    return return_value;
}

int compare_strings(string *s1, string *s2) {
    char c1, c2;
    int return_value = 1,
        i = 0;

    if (s1->length != s2->length) return_value = 0;
    else {
        while (i < s1->length && return_value) {
            c1 = *(s1->base + i);
            c2 = *(s2->base + i);

            return_value = compare_insensitive(c1, c2);
            i++;
        }
    }

    return return_value;
}

void delete_string(string **head) {
    free(*head);
    *head = NULL;
}

string *create_string(char *base, int length) {
    string *new = malloc(sizeof(string));
    if (new == NULL) {
        fprintf(stderr, "Impossible d'allouer la mémoire.\n");
        exit(EXIT_FAILURE);
    }
    
    new->base = base;
    new->length = length;

    return new;
}

void delete_last_num_val_in_set(num_val_set **head) {
    num_val_set *element = *head, *previous = *head;

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

void insert_in_num_val_set(num_val_set **head, int value) {
    num_val_set *new = malloc(sizeof(num_val_set)), *element = *head;
    if (new != NULL) {
        new->value = value;
        new->next = NULL;

        if (element == NULL) *head = new;
        else {
            while (element->next != NULL) element = element->next;
            element->next = new;
        }
    }
}

void delete_num_val(num_val **head) {
    num_val *element = *head;

    if (element->type == SET) delete_last_num_val_in_set(&(element->value.set));
    free(element);
}

num_val *create_num_val(num_val **head) {
    num_val *new = malloc(sizeof(num_val));
    if (new == NULL) {
        fprintf(stderr, "Impossible d'allouer la mémoire.\n");
        exit(EXIT_FAILURE);
    }

    new->value.set = NULL;

    *head = new;
    return new;
}

void delete_last_repetition(repetition **head) { // There is at least a repetition
    repetition *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }

    if (element->type == RULENAME) delete_string(&(element->content.rulename));
    else if (element->type == GROUP || element->type == OPTION) delete_last_concatenation(&(element->content.concatenations));
    else if (element->type == CHAR_VAL) delete_string(&(element->content.char_val));
    else free(element->content.num_val);
    free(element);
}

repetition *create_repetition(repetition **head) {
    repetition *new = malloc(sizeof(repetition)), *element = *head;
    if (new == NULL) {
        fprintf(stderr, "Impossible d'allouer la mémoire.\n");
        exit(EXIT_FAILURE);
    }

    new->content.concatenations = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_concatenation(concatenation **head) { // There is at least a concatenation
    concatenation *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }

    while (element->repetitions != NULL) delete_last_repetition(&(element->repetitions));
    free(element);
}

concatenation *create_concatenation(concatenation **head) {
    concatenation *new = malloc(sizeof(concatenation)), *element = *head;
    if (new == NULL) {
        fprintf(stderr, "Impossible d'allouer la mémoire.\n");
        exit(EXIT_FAILURE);
    }

    new->repetitions = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_rule(rule **head) { // There is at least a rule
    rule *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }

    delete_string(&(element->rulename));
    while (element->concatenations != NULL) delete_last_concatenation(&(element->concatenations));
    free(element);
}

rule *find_rule(rule *head, string *rulename) {
    rule *r = head, *element = NULL;

    while (r != NULL && !compare_strings(r->rulename, rulename)) r = r->next;
    if (r != NULL) element = r;

    return element;
}

rule *create_rule(rule **head, string *rulename) {
    rule *new = malloc(sizeof(rule)), *element = *head;
    if (new == NULL) {
        fprintf(stderr, "Impossible d'allouer la mémoire.\n");
        exit(EXIT_FAILURE);
    }

    new->rulename = rulename;
    new->concatenations = NULL;
    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

int get_bin_or_dec_or_hex_val(bin_or_dec_or_hex type, num_val *num_val, char *abnf, int *index) {
    char c;
    int previous_index = *index,
        value = 0,
        return_value = 0,
        i = 0,
        base;

    if (type == BIN) {
        c = 'B';
        base = 2;

    } else if (type == DEC) {
        c = 'D';
        base = 10;

    } else {
        c = 'X';
        base = 16;
    }

    if (!compare_insensitive(*(abnf + *index), c)) return_value = -1;
    else {
        (*index)++;

        while (is_digit(*(abnf + *index), base)) {
            value = value * base + char_to_int(*(abnf + *index));
            (*index)++;
            i++;
        }

        if (i == 0) return_value = -1;
        else if (*(abnf + *index) == '-') {
            num_val->type = MIN_MAX;
            num_val->value.min_max[0] = value;
            value = 0;
            (*index)++;

            while (is_digit(*(abnf + *index), base)) {
                value = value * base + char_to_int(*(abnf + *index));
                (*index)++;
            }

           num_val->value.min_max[1] = value;
        
        } else {
            num_val->type = SET;
            insert_in_num_val_set(&(num_val->value.set), value);

            while (*(abnf + *index) == '.') {
                value = 0;
                (*index)++;

                while (is_digit(*(abnf + *index), base)) {
                    value = value * base + char_to_int(*(abnf + *index));
                    (*index)++;
                }

                insert_in_num_val_set(&(num_val->value.set), value);
            }
        }
    }

    if (return_value == -1) *index = previous_index;
    return return_value;
}

int get_num_val(num_val **head, char *abnf, int *index) {
    num_val *n = create_num_val(head);
    int return_value = 0;

    if (*(abnf + *index) != '%') return_value = -1;
    else {
        (*index)++;

        if (get_bin_or_dec_or_hex_val(BIN, n, abnf, index) == -1) {
            if (get_bin_or_dec_or_hex_val(DEC, n, abnf, index) == -1) {
                if (get_bin_or_dec_or_hex_val(HEX, n, abnf, index) == -1) return_value = -1;
            }
        }
    }

    return return_value;
}

int get_char_val(string **char_val, char *abnf, int *index) {
    char *base = abnf + *index + 1;
    int previous_index = *index,
        length = 0,
        return_value = 0;

    if (*(abnf + *index) != '"') return_value = -1;
    else {
        (*index)++;

        while (is_vchar(*(abnf + *index)) && *(abnf + *index) != '"') {
            (*index)++;
            length++;
        }

        if (*(abnf + *index) != '"') return_value = -1;
        else {
            (*index)++;
            *char_val = create_string(base, length);
        }
    }

    if (return_value == -1) *index = previous_index;
    return return_value;
}

int get_group_or_option(opt_or_grp type, concatenation **head, char *abnf, int *index) {
    char c1 = type == GRP ? '(' : '[',
         c2 = type == GRP ? ')' : ']';
    int previous_index = *index,
        return_value = 0;

    if (*(abnf + *index) != c1) return_value = -1;
    else {
        (*index)++;

        while (get_c_wsp(abnf, index) != -1);
        if (get_alternation(head, abnf, index) == -1) return_value = -1;
        else {
            while (get_c_wsp(abnf, index) != -1);
            if (*(abnf + *index) != c2) return_value = -1;
            else (*index)++;
        }
    }

    if (return_value == -1) *index = previous_index;
    return return_value;
}

int get_element(repetition *r, char *abnf, int *index) {
    int return_value = 0;

    if (get_rulename(&(r->content.rulename), abnf, index) != -1) r->type = RULENAME;       
    else if (get_group_or_option(GRP, &(r->content.concatenations), abnf, index) != -1) r->type = GROUP;
    else if (get_group_or_option(OPT, &(r->content.concatenations), abnf, index) != -1) {
        r->type = OPTION;
        r->min = 0;
        r->max = 1;

    } else if (get_char_val(&(r->content.char_val), abnf, index) != -1) r->type = CHAR_VAL;
    else if (get_num_val(&(r->content.num_val), abnf, index) != -1) r->type = NUM_VAL;
    else return_value = -1; // No support for prose-val

    return return_value;
}

int get_repeat(repetition *r, char *abnf, int *index) {
    int return_value = 0,
        min = 0,
        max = 0,
        i = 0;

    while (is_digit(*(abnf + *index), 10)) {
        min = min * 10 + char_to_int(*(abnf + *index));
        (*index)++;
        i++;
    }

    if (*(abnf + *index) != '*') {
        if (i == 0) {
            min = 1;
            return_value = -1;
        }

        r->min = min;
        r->max = min;

    } else {
        (*index)++;
        i = 0;

        while (is_digit(*(abnf + *index), 10)) {
            max = max * 10 + char_to_int(*(abnf + *index));
            (*index)++;
            i++;
        }

        r->min = min;
        r->max = i == 0 ? -1 : max;
    }

    return return_value;
}

int get_repetition(repetition **head, char *abnf, int *index) {
    repetition *r = create_repetition(head);
    int return_value = 0;

    get_repeat(r, abnf, index);
    if (get_element(r, abnf, index) == -1) {
        delete_last_repetition(head);
        return_value = -1;
    }

    return return_value;
}

int get_concatenation(concatenation **head, char *abnf, int *index) {
    concatenation *c = create_concatenation(head);
    int previous_index = *index,
        return_value = 0,
        is_valid = 1,
        i = 0;

    if (get_repetition(&(c->repetitions), abnf, index) == -1) return_value = -1;
    else {
        while (is_valid) {
            while (get_c_wsp(abnf, index) != -1) i++;
            if (i == 0 || get_repetition(&(c->repetitions), abnf, index) == -1) is_valid = 0;
            i = 0;
        }
    }

    if (return_value == -1 || i != 0) {
        delete_last_concatenation(head);
        *index = previous_index;
    }

    return return_value;
}

int get_alternation(concatenation **head, char *abnf, int *index) {
    int previous_index = *index,
        return_value = 0,
        is_valid = 1;

    if (get_concatenation(head, abnf, index) == -1) return_value = -1;
    else {
        while (is_valid) {
            while (get_c_wsp(abnf, index) != -1); 

            if (*(abnf + *index) != '/') is_valid = 0;
            else {
                (*index)++;
                while (get_c_wsp(abnf, index) != -1);
                if (get_concatenation(head, abnf, index) == -1) is_valid = 0;
            }
        }
    }

    if (return_value == -1) *index = previous_index;
    return return_value;
}

int get_comment(char *abnf, int *index) {
    int previous_index = *index,
        return_value = 0;

    if (*(abnf + *index) != ';') return_value = -1;
    else {
        (*index)++;

        while (is_wsp(*(abnf + *index)) || is_vchar(*(abnf + *index))) (*index)++;
        
        if (!is_crlf(abnf + *index)) return_value = -1;
        else (*index) += 2;
    }

    if (return_value == -1) *index = previous_index;
    return return_value;
}

int get_c_nl(char *abnf, int *index) {
    int return_value = 0;

    if (get_comment(abnf, index) == -1) {
        if (!is_crlf(abnf + *index)) return_value = -1;
        else (*index) += 2;
    }

    return return_value;
}

int get_c_wsp(char *abnf, int *index) {
    int previous_index = *index,
        return_value = 0;

    if (!is_wsp(*(abnf + *index))) {
        if (get_c_nl(abnf, index) == -1) return_value = -1;
        else if (!is_wsp(*(abnf + *index))) return_value = -1;
    }

    if (return_value == 0) (*index)++;
    else *index = previous_index;

    return return_value;
}

int get_elements(concatenation **head, char *abnf, int *index) {
    int return_value = 0;

    if (get_alternation(head, abnf, index) == -1) return_value = -1;
    else while (get_c_wsp(abnf, index) != -1);

    return return_value;
}

int get_defined_as(char *abnf, int *index) {
    int previous_index = *index,
        return_value = 0;

    while (get_c_wsp(abnf, index) != -1);
    if (*(abnf + *index) != '=') return_value = -1; // No support for incremental alternatives
    else {
        (*index)++;
        while (get_c_wsp(abnf, index) != -1);
    }

    if (return_value == -1) *index = previous_index;
    return return_value;
}

int get_rulename(string **rulename, char *abnf, int *index) {
    int has_rulename = 1;
    int length = 0;

    *rulename = create_string(abnf + *index, 0);

    if (!is_alpha(*(abnf + *index))) has_rulename = 0;
    else {
        do {
            (*index)++;
            (*rulename)->length++;

        } while (is_alpha(*(abnf + *index)) || is_digit(*(abnf + *index), 10) || *(abnf + *index) == '-');
    }

    if (!has_rulename) delete_string(rulename);
    return has_rulename - 1;
}

/*int get_rulename(string **rulename, char *abnf, int *index) { // No support for redefinition of rules
    char *base = abnf + *index;
    int length = 0,
        return_value = 0;

    if (!is_alpha(*(abnf + *index))) return_value = -1;
    else {
        (*index)++;
        length++;

        while (is_alpha(*(abnf + *index)) || is_digit(*(abnf + *index), 10) || *(abnf + *index) == '-') {
            (*index)++;
            length++;
        }
        *rulename = create_string(base, length);
    }

    return return_value;
}*/

int get_rule(rule **head, char *abnf, int *index) {
    rule *r = create_rule(head, NULL);
    int previous_index = *index,
        return_value = 0;

    if (get_rulename(&(r->rulename), abnf, index) == -1) return_value = -1;
    else if (get_defined_as(abnf, index) == -1) return_value = -1;
    else if (get_elements(&(r->concatenations), abnf, index) == -1) return_value = -1;
    else if (get_c_nl(abnf, index) == -1) return_value = -1;
    
    if (return_value == -1) {
        delete_last_rule(head);
        *index = previous_index;
    }

    return return_value;
}

int get_rulelist(rule **head, char *file_name) {
    int is_valid = 1,
        index = 0,
        i = 0;
    
    // Règles systèmes
    create_rule(head, create_string("_CHAR_VAL", 9));
    create_rule(head, create_string("_NUM_VAL", 8));

    FILE *f = fopen(file_name, "r");

    if (f == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier.\n");
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    rewind(f);

    char *abnf = malloc(size * sizeof(char));

    if (fread(abnf, 1, size, f) != size) {
        fprintf(stderr, "Impossible de lire le contenu du fichier.\n");
        exit(EXIT_FAILURE);
    }

    if (fclose(f) == EOF) {
        fprintf(stderr, "Impossible de fermer le fichier.\n");
        exit(EXIT_FAILURE);
    }

    while (is_valid) {
        if (get_rule(head, abnf, &index) == -1) {
            while (get_c_wsp(abnf, &index) != -1);
            if (get_c_nl(abnf, &index) == -1) is_valid = 0;
        }

        i++;
    }

    return i <= 1 ? -1 : 0;
}

typedef struct node node;

struct node {
    string *rulename;
    string *content;
    node *children;
    node *next;
};

void print_nodes(node *n, int i) {
    int j;
    while (n != NULL) {
        for (j = 0; j < i; j++) { printf(" "); } printf("name: "); print_string(n->rulename);
        for (j = 0; j < i; j++) { printf(" "); } printf("content: "); print_string(n->content);
        if (n->children != NULL) { for (j = 0; j < i; j++) { printf(" "); } printf("children:\n"); print_nodes(n->children, i + 2); }
        n = n->next;
    }
}

node *insert_node(node **head, string *rulename, string *content) {
    node *new = malloc(sizeof(node)), *element = *head;
    if (new == NULL) {
        fprintf(stderr, "Impossible d'allouer la mémoire.\n");
        exit(EXIT_FAILURE);
    }

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
    node *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }

    delete_string(&(element->content));
    while (element->children != NULL) delete_last_node(&(element->children));
    free(element);
}

int parse_http_request(node **tree, unsigned char *input, int length);
int parse_rule(node **tree, string *rulename, unsigned char *input, int *index, int length, rule *head);
int parse_concatenations(node **tree, concatenation *c, unsigned char *input, int *index, int length, rule *head);
int parse_concatenation(node **tree, concatenation *c, unsigned char *input, int *index, int length, rule *head);
int parse_repetition(node **tree, repetition *r, unsigned char *input, int *index, int length, rule *head);
int parse_char_val(node **tree, string *s, unsigned char *input, int *index, int length, rule *head);
int parse_num_val(node **tree, num_val *n, unsigned char *input, int *index, int length, rule *head);
int parse_num_val_set(node **tree, num_val_set *s, unsigned char *input, int *index, int length);

int parse_rule(node **tree, string *rulename, unsigned char *input, int *index, int length, rule *head) {
    rule *r = find_rule(head, rulename);
    int previous_index = *index,
        return_value = 0;

    if (r == NULL) return_value = -1;
    else {
        //printf("Caractère : %d / Index : %d / Règle : ", *(input + *index), *index); print_string(rulename);
        node *n = insert_node(tree, rulename, create_string((char *) input + *index, 0));
        return_value = parse_concatenations(&(n->children), r->concatenations, input, index, length, head);

        if (return_value == -1) delete_last_node(tree);
        else n->content->length = *index - previous_index;
    }

    return return_value;
}

int parse_concatenations(node **tree, concatenation *c, unsigned char *input, int *index, int length, rule *head) {
    int return_value = -1;

    while (c != NULL && return_value == -1) {
        return_value = parse_concatenation(tree, c, input, index, length, head);
        c = c->next;
    }
    return return_value;
}

int parse_concatenation(node **tree, concatenation *c, unsigned char *input, int *index, int length, rule *head) {
    repetition *r = c->repetitions;
    int previous_index = *index,
        return_value = 0;

    while (r != NULL && return_value == 0) {
        return_value = parse_repetition(tree, r, input, index, length, head);
        r = r->next;
    }
    
    if (return_value == -1) *index = previous_index;
    return return_value;
}

int parse_repetition(node **tree, repetition *r, unsigned char *input, int *index, int length, rule *rulelist) {
    repetition_type type = r->type;
    int has_repetition = 1,
        is_valid = 1,
        i = 0;

    while (i != r->max && is_valid) {
        if (type == RULENAME && parse_rule(tree, r->content.rulename, input, index, length, rulelist) == -1) is_valid = 0;
        else if ((type == GROUP || type == OPTION) && parse_concatenations(tree, r->content.concatenations, input, index, length, rulelist) == -1) is_valid = 0;
        else if (type == CHAR_VAL && parse_char_val(tree, r->content.char_val, input, index, length, rulelist) == -1) is_valid = 0;
        else if (type == NUM_VAL && parse_num_val(tree, r->content.num_val, input, index, length, rulelist) == -1) is_valid = 0;
        else i++;
    }

    if (i < r->min) has_repetition = 0;
    return has_repetition - 1;
}

/*int parse_repetition(node **tree, repetition *r, unsigned char *input, int *index, int length, rule *head) {
    repetition_type type = r->type;
    int previous_index = *index,
        return_value = 0,
        is_valid = 1,
        i = 0;

    while (i != r->max && is_valid) {
        if (type == RULENAME && parse_rule(tree, r->content.rulename, input, index, length, head) == -1) is_valid = 0;
        else if ((type == GROUP || type == OPTION) && parse_concatenations(tree, r->content.concatenations, input, index, length, head) == -1) is_valid = 0;
        else if (type == CHAR_VAL && parse_char_val(tree, r->content.char_val, input, index, length, head) == -1) is_valid = 0;
        else if (type == NUM_VAL && parse_num_val(tree, r->content.num_val, input, index, length, head) == -1) is_valid = 0;
        else i++;
    }

    if (i < r->min) {
        *index = previous_index;
        return_value = -1;
    }

    return return_value;
}*/

int parse_char_val(node **tree, string *s, unsigned char *input, int *index, int length, rule *head) {
    int content_length = min(length - *index, s->length),
        return_value = 0;
    string *content = create_string((char *) input + *index, content_length);
    insert_node(tree, find_rule(head, create_string("_CHAR_VAL", 9))->rulename, content);

    if (compare_strings(s, content)) *index += content_length;
    else {
        delete_last_node(tree);
        return_value = -1;
    }

    return return_value;
}

int parse_num_val(node **tree, num_val *v, unsigned char *input, int *index, int length, rule *head) {
    string *content = create_string((char *) input + *index, 0);
    node *n = insert_node(tree, find_rule(head, create_string("_NUM_VAL", 8))->rulename, content);
    int previous_index = *index,
        return_value = -1;

    if (*index < length && v->type == MIN_MAX && v->value.min_max[0] <= *(input + *index) && *(input + *index) <= v->value.min_max[1]) {
        return_value = 0;
        (*index)++;

    } else if (v->type == SET) return_value = parse_num_val_set(tree, v->value.set, input, index, length);

    if (return_value == -1) delete_last_node(tree);
    else n->content->length = *index - previous_index;

    return return_value;
}

int parse_num_val_set(node **tree, num_val_set *s, unsigned char *input, int *index, int length) {
    int previous_index = *index,
        return_value = 0;

    while (s != NULL && *index < length && *(input + *index) == s->value) {
        (*index)++;
        s = s->next;
    }

    if (s != NULL) {
        *index = previous_index;
        return_value = -1;
    }

    return return_value;
}

int parse_http_request(node **tree, unsigned char *input, int length) {
    string *rulename = create_string("HTTP-message", 12);
    int index = 0;

    return parse_rule(tree, rulename, input, &index, length, rulelist);
}

int main () {
    node *tree = NULL;
    unsigned char *input = (unsigned char *) "GET / HTTP/1.0\r\ntRAnSfEr-eNcoDiNG:              ,gzip,             ,qB                                    ;Yh4w=                        \"†\\à\";u     =\"\" ;r7GqcU     =   \",\"         ;           nH0|jHyqH~OBp     =                                           K8    ;            j= \"w\\ô\"           ,     m2FMM     ; SM8 = \"\";     vh           =          \"g\\©Ïå\\ø\"    ;          9c   =      \"\\×\";                   GA =          '$'                                           ;   Xe*                   = \"\";.bo   = \"ÿ\\i7\\U\\n\\fÚ­6\"  ;!           =  \"8\\ØˆG\\{\";       _^n1Oi5k   =^wzdaNfk  ,  , 3Ew-+ro  ;          2   =   \"\\©\\B\\&\"       ,,      ,dp-x*;  %l                   =                 7              ,    wAJh04;DqQ7f    =%1     , 4e+L      ;B4 =                           \"õ\"\r\nTransfer-Encoding:      cwz     ,eVs;   h6  =   \"Ð\"    ;             lL`             =  \"7œ\\V\\°'\\Â÷n\\±\\6\"           ;  t|-prcv-mTV              =Nzj;           R       =    \"\"     \r\n\r\n";
    printf("Parsing de L'ABNF...\n");
    if (get_rulelist(&rulelist, "syntax.abnf") == -1) printf("ABNF incorrect ou syntaxe non supportée.\n");
    else {
        /*printf("ABNF parsé avec succès.\nParsing de la requête :\n%s\n################################################################################################################################################################################\n\n", input);
        if (parse_http_request(&tree, input, strlen((char *)input)) == -1) printf("Requête invalide.\n");
        else {
            printf("Requête parsée avec succès. Affichage du contenu :\n");
            print_nodes(tree, 0);
        }*/

        //printf("\nAffichage des règles :\n");
        print_rules(rulelist);
    }
}