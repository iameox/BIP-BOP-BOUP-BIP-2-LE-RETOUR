#include "structures.h"

#define BASE_RULE "HTTP-message"
#define BASE_RULE_LENGTH get_length(BASE_RULE)
#define ABNF_FILE_NAME "syntax.abnf"

int parse_http(Node **tree, Rule **rulelist, char *input, int length);
int parse_rule(Node **tree, Rule *rulelist, char *input, int *index, int length, String *rulename);
int parse_alternation(Node **tree, Rule *rulelist, char *input, int *index, int length, Concatenation *a);
int parse_concatenation(Node **tree, Rule *rulelist, char *input, int *index, int length, Concatenation *c);
int parse_repetition(Node **tree, Rule *rulelist, char *input, int *index, int length, Repetition *r);
int parse_char_val(Node **tree, Rule *rulelist, char *input, int *index, int length, String *s);
int parse_num_val(Node **tree, Rule *rulelist, char *input, int *index, int length, Num_val *v);
int parse_num_val_set(Node **tree, char *input, int *index, int length, Num_val_set *s);