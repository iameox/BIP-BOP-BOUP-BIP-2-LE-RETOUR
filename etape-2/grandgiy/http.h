#include "structures.h"

#define BASE_RULE "HTTP-message"
#define BASE_RULE_LENGTH 12
#define ABNF_FILE_NAME "syntax.abnf"

int parse_http(node **tree, rule **rulelist, char *input, int length);
int parse_rule(node **tree, rule *rulelist, char *input, int *index, int length, string *rulename);
int parse_alternation(node **tree, rule *rulelist, char *input, int *index, int length, concatenation *a);
int parse_concatenation(node **tree, rule *rulelist, char *input, int *index, int length, concatenation *c);
int parse_repetition(node **tree, rule *rulelist, char *input, int *index, int length, repetition *r);
int parse_char_val(node **tree, rule *rulelist, char *input, int *index, int length, string *s);
int parse_num_val(node **tree, rule *rulelist, char *input, int *index, int length, num_val *v);
int parse_num_val_set(node **tree, char *input, int *index, int length, num_val_set *s);