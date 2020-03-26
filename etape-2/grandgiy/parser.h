#include "types.h"

int exec_rule(rule *rule, char *chaine, int index);

int concatenation(rule_params *unused, rule *rule, char *input, int index);

int repetition(rule_params *params, rule *rule, char *input, int index);

int option(rule_params *unused, rule *rule, char *input, int index);

int alternation(rule_params *unused, rule *rule, char *input, int index);

int num_val(rule_params *params, rule *rule, char *input, int index);

int char_val(rule_params *params, rule *rule, char *input, int index);

int parse(char *input);