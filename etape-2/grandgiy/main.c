/*#include <stdio.h>
#include "parser.h"

int main() {*/
    // o-la = *( a-lo )
    // a-lo = "salut" / %d50-67 / %d85.38.87.87

    /*
    name: "o-la"
    children: 
        function: &repetition
        params: [0, -1]
        children:
            rule: &a-lo
    next:
        name: "a-lo"
        children:
            function: &alternation
            children:
                function: &char_val
                params: [0, 0, "salut", 5]
                next:
                    function: &num_val
                    params: [50, 67]
                    next:
                        function: &num_val
                        params: [0, 0, 0, 0, 85, 38, 87, 87] -- U&WW
    */

/*    char *a_lo_name_str = "a-lo";
    string a_lo_name = { a_lo_name_str, 4 };
    char *val1_str_in = "salut";
    string val1_str = { val1_str_in, 5 };
    params_set val3_set_3 = { 87, NULL };
    params_set val3_set_2 = { 87, &val3_set_3 };
    params_set val3_set_1 = { 38, &val3_set_2 };
    params_set val3_set = { 85, &val3_set_1 };
    rule_params val3 = { { 0, 0 }, NULL, &val3_set };
    rule_params val2 = { { 50, 67 }, NULL, NULL };
    rule_params val1 = { { 0, 0 }, &val1_str, NULL };
    rule a_lo_in_3 = { NULL, &num_val, &val3, NULL, NULL, NULL };
    rule a_lo_in_2 = { NULL, &num_val, &val2, NULL, NULL, &a_lo_in_3 };
    rule a_lo_in_1 = { NULL, &char_val, &val1, NULL, NULL, &a_lo_in_2 };
    rule a_lo_in = { NULL, &alternation, NULL, NULL, &a_lo_in_1, NULL };
    rule a_lo = { &a_lo_name, NULL, NULL, NULL, &a_lo_in, NULL };

    char *o_la_name_str = "o-la";
    string o_la_name = { o_la_name_str, 4 };
    rule_params r1 = { { 0, -1 }, NULL, NULL };
    rule o_la_in_1 = { NULL, NULL, NULL, a_lo.children, NULL, NULL };
    rule o_la_in = { NULL, &repetition, &r1, NULL, &o_la_in_1, NULL };
    rule o_la = { &o_la_name, NULL, NULL, NULL, &o_la_in, &a_lo };

    char *input = "U&WWsalutA575D"; // 14
    int x = concatenation(NULL, o_la.children, input, 0);

    printf("Taille finale de la chaîne parsée : %d\n", x);

    return 1;
}*/

/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

string *create_string(char *base, int length) {
    string *new = malloc(sizeof(string));
    if (new == NULL) return NULL;

    new->base = base;
    new->length = length;

    return new;
}

rule *insert_rule(rule **rule_set, string *name, int (*function)(rule_params *, rule *, char *, int), rule_params *params, rule *sub_rule) {
    rule *new = malloc(sizeof(rule)), *head = *rule_set;
    if (new == NULL) return NULL;

    new->name = name;
    new->function = function;
    new->params = params;
    new->sub_rule = sub_rule;
    new->children = NULL;
    new->next = NULL;

    // Insertion en queue
    if (head == NULL) *set = new;
    else {
        while (head != NULL) {
            prev = head;
            head = head->next;
        }

        prev->next = new;
    }
    return new;
}

rule_params *create_rule_params(int min, int max, string *string, params_set *set) {
    rule_params *new = malloc(sizeof(rule_params));
    if (new == NULL) return NULL;

    new->min_max[0] = min;
    new->min_max[1] = max;
    new->string = string;
    new->set = set;

    return new;
}

params_set *insert_param(params_set **set, int value) {
    params_set *new = malloc(sizeof(params_set)), *head = *set, *prev = NULL;
    if (new == NULL) return NULL;

    new->value = value;
    new->next = NULL;

    // Insertion en queue
    if (head == NULL) *set = new;
    else {
        while (head != NULL) {
            prev = head;
            head = head->next;
        }

        prev->next = new;
    }

    return new;
}

rule *insert_child(rule *rule_set, int (*function)(rule_params *, rule *, char *, int), rule_params *params, rule *sub_rule) {
    return insert_rule(&(rule_set->children), NULL, function, params, sub_rule->children);
}

void print_sub_rules(rule* rule_set) {
    int i = 0;
    rule_set = rule_set->children;

    while (rule_set != NULL) {
        print_sub_rules(rule_set);
        i++;
        rule_set = rule_set->next;
    }
    printf("  Sous-règles : %d\n", i);
}

void print_string(string *string) {
    int i;

    for (i = 0; i < string->length; i++) {
        printf("%c", *(string->base + i));
    }
    printf("\n");
}

void print_rules(rule* rule_set) {
    while (rule_set != NULL) {
        printf("Règle : ");
        print_string(rule_set->name);
        print_sub_rules(rule_set);
        rule_set = rule_set->next;
    }
}

//a-lo = "salut" / %d50-67 / %d85.38.87.87
//o-la = *( a-lo )

/*int main()
{
    r = NULL;

    params_set *s = NULL;
    insert_param(&s, 85);
    insert_param(&s, 38);
    insert_param(&s, 87);
    insert_param(&s, 87);

    rule /**a_lo = insert_rule(&r, create_string("a-lo", 4), NULL, NULL, NULL),
         *alt = insert_child(a_lo, &alternation, NULL, NULL),
         *alt_1 = insert_child(alt, &char_val, create_rule_params(0, 0, create_string("salut", 5), NULL), NULL),
         *alt_2 = insert_child(alt, &num_val, create_rule_params(0, 0, NULL, s), NULL),
         *alt_3 = insert_child(alt, &num_val, create_rule_params(50, 67, NULL, NULL), NULL),

         *o_la = insert_rule(&r, create_string("o-la", 4), NULL, NULL, NULL),
         *rep = insert_child(o_la, &repetition, create_rule_params(0, -1, NULL, NULL), NULL),
         *rep_1 = insert_child(rep, NULL, NULL, a_lo->children);

         *defined_as

         *rulename = insert_rule(&r, create_string("rulename", 8), NULL, NULL, NULL),
           *rulename_1 = insert_child(rulename, NULL, NULL, alpha),
           *rep = insert_child(rulename, &repetition, create_rule_params(0, -1, NULL, NULL), NULL),
             *alt = insert_child(rep, &alternation, NULL, NULL),
               *alt_1 = insert_child(alt, NULL, NULL, alpha),
               *alt_2 = insert_child(alt, NULL, NULL, digit),
               *alt_3 = insert_child(alt, char_val, create_rule_params(0, 0, create_string("-", 1), NULL), NULL),

         *rule = insert_rule(&r, create_string("rule", 4), NULL, NULL, NULL),
           *rule_1 = insert_child(rule, NULL, NULL, rulename),
           *rule_2 = insert_child(rule, NULL, NULL, defined_as),
           *rule_3 = insert_child(rule, NULL, NULL, elements),
           *rule_4 = insert_child(rule, NULL, NULL, c_nl),

         *rulelist = insert_rule(&r, create_string("rulelist", 8), NULL, NULL, NULL),
           *rep = insert_child(rulelist, &repetition, create_rule_params(1, -1, NULL, NULL), NULL),
             *alt = insert_child(rep, &alternation, NULL, NULL),
               *alt_1 = insert_child(alt, NULL, NULL, rule),
               *alt_2 = insert_child(alt, &concatenation, NULL, NULL),
                 *cat_1 = insert_child(alt_2, &repetition, create_rule_params(0, -1, NULL, NULL), NULL),
                   *cat_1_1 = insert_child(cat_1, NULL, NULL, &c_wsp),
                 *cat_2 = insert_child(alt_2, NULL, NULL, &c_nl);*


    char *input = "U&WWsalutA575D"; // 14
    int x = parse(input);
    return 0;
}*/