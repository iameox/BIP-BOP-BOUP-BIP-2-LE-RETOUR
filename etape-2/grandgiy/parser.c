#include <stdio.h>
#include <stdlib.h>
#include "types.h"

int exec_rule(rule *rule, char *input, int index) {
    int length = 0;
    
    printf("je commence la règle !!!!!!!!!!\n");
    if (rule->sub_rule == NULL) length = rule->function(rule->params, rule->children, input, index);
    else length = exec_rule(rule->sub_rule, input, index);
    printf("g fini ma règle !!!!!!!!! :, index = %d length = %d\n", index, length);

    return length;
}

int concatenation(rule_params *unused, rule *rule, char *input, int index) {
    int total_length = 0,
        length = 0;

    while (rule != NULL && length != -1) {
        length = exec_rule(rule, input, index);
        total_length += length;
        index += length;

        rule = rule->next;
    }

    return length == -1 ? -1 : total_length;
}

int repetition(rule_params *params, rule *rule, char *input, int index) {
    int min = params->min_max[0],
        max = params->min_max[1],
        total_length = 0,
        length = 1,
        i = 0;

    while (i != max && length != -1) {
        length = concatenation(NULL, rule, input, index);

        if (length != -1) {
            total_length += length;
            index += length;
        }

        i++;
    }

    return i <= min ? -1 : total_length;
}

// Raccourci pour générer des options, p = NULL.
int option(rule_params *unused, rule *rule, char *input, int index) {
    rule_params params = { { 0, 1 }, NULL, NULL };
    return repetition(&params, rule, input, index);
}

int alternation(rule_params *unused, rule *rule, char *input, int index) {
    int length = -1;
    while (rule != NULL && length == -1) {
        length = exec_rule(rule, input, index);
        rule = rule->next;
    }

    return length;
}

int num_val(rule_params *params, rule *rule, char *input, int index) {
    char *c = input + index;
    params_set *num = params->set;
    int min = params->min_max[0],
        max = params->min_max[1],
        length = -1;

    if (*c != '\0') {
        if (num == NULL) { // min/max
            if (min <= *c && *c <= max) length = 1;
        
        } else {
            printf("eh regardez-moi jss là eeeh ohh\n");
            length = 0;

            while (*c != '\0' && num != NULL && length != -1) {
                printf("le karaktér c koa c : %d é la valeur c koi c : %d\n", *c, num->value);
                if (*c == num->value) length++;
                else length = -1;

                c++;
                num = num->next;
            }

            if (num != NULL) length = -1;
        }
    }

    printf("keske g dans ma besace : length = %d et 6 pots de miels\n", length);

    return length;
}

int char_val(rule_params *params, rule *rule, char *input, int index) {
    char *c1 = input + index,
         *c2 = params->string->base;
    int max = params->string->length,
        length = 0,
        i = 0;

    if (*c1 != '\0') {
        while (*c1 != '\0' && i < max && length != -1) {
            if (*c1 == *(c2 + i)) length++;
            else length = -1;

            c1++;
            i++;
        }
    }

    return i < max ? -1 : length;
}

int parse(char *input) {
    return concatenation(NULL, r->children, input, 0);
}