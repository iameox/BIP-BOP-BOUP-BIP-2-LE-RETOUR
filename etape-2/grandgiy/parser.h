#include "types.h"

int exec_rule(rule *rule, char *chaine, int index);

int exec_rules(rule *rule, char *input, int index);

int repetition(rule_params *params, rule *rule, char *input, int index);

int alternation(rule_params *params, rule *rule, char *input, int index);

int num_val(rule_params *params, rule *rule, char *input, int index);

int char_val(rule_params *params, rule *rule, char *input, int index);