#include "structures.h"

#define min(a, b) (a <= b ? a : b)

#define CHAR_VAL_RULE "_CHAR_VAL"
#define CHAR_VAL_RULE_LENGTH 9

#define NUM_VAL_RULE "_NUM_VAL"
#define NUM_VAL_RULE_LENGTH 8

void compile_abnf(rule **head, char *file_name);

int compile_rulelist(rule **head, char *abnf, int *index);
int compile_rule(rule **head, char *abnf, int *index);
int compile_rulename(char *abnf, int *index, string **rulename);
int compile_defined_as(char *abnf, int *index, int is_redefinition);
int compile_elements(concatenation **head, char *abnf, int *index);
int compile_c_wsp(char *abnf, int *index);
int compile_c_nl(char *abnf, int *index);
int compile_comment(char *abnf, int *index);
int compile_alternation(concatenation **head, char *abnf, int *index);
int compile_concatenation(concatenation **head, char *abnf, int *index);
int compile_repetition(repetition **head, char *abnf, int *index);
int compile_repeat(char *abnf, int *index, int *min, int *max);
int compile_element(char *abnf, int *index, repetition *r);
int compile_group_or_option(concatenation **head, char *abnf, int *index, has_group_or_option_type type);
int compile_char_val(char *abnf, int *index, string **char_val);
int compile_num_val(char *abnf, int *index, num_val **num_val);
int compile_bin_or_dec_or_hex_val(char *abnf, int *index, num_val *num_val, int base);

int consume_c_wsps(char *abnf, int *index, int min);
int consume_crlf(char *abnf, int *index);
int consume_number(char *abnf, int *index, int base, int *value, int min);