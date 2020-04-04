#include "structures.h"

#define min(a, b) (a <= b ? a : b)

#define CHAR_VAL_RULE "_CHAR-VAL"
#define CHAR_VAL_RULE_LENGTH get_length(CHAR_VAL_RULE)

#define NUM_VAL_RULE "_NUM-VAL"
#define NUM_VAL_RULE_LENGTH get_length(NUM_VAL_RULE)

void compile_abnf(Rule **head, char **abnf_buffer, char *file_name);

int compile_rulelist(Rule **head, char *abnf, int *index);
int compile_rule(Rule **head, char *abnf, int *index);
int compile_rulename(char *abnf, int *index, String **rulename);
int compile_defined_as(char *abnf, int *index, int is_redefinition);
int compile_elements(Concatenation **head, char *abnf, int *index);
int compile_c_wsp(char *abnf, int *index);
int compile_c_nl(char *abnf, int *index);
int compile_comment(char *abnf, int *index);
int compile_alternation(Concatenation **head, char *abnf, int *index);
int compile_concatenation(Concatenation **head, char *abnf, int *index);
int compile_repetition(Repetition **head, char *abnf, int *index);
int compile_repeat(char *abnf, int *index, int *min, int *max);
int compile_element(char *abnf, int *index, Repetition *r);
int compile_group_or_option(Concatenation **head, char *abnf, int *index, Has_group_or_option_type type);
int compile_char_val(char *abnf, int *index, String **char_val);
int compile_num_val(char *abnf, int *index, Num_val **Num_val);
int compile_bin_or_dec_or_hex_val(char *abnf, int *index, Num_val *num_val, int base);

int consume_c_wsps(char *abnf, int *index, int min);
int consume_crlf(char *abnf, int *index);
int consume_number(char *abnf, int *index, int base, int *value, int min);
