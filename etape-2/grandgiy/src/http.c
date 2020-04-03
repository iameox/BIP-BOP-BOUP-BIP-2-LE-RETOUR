#include "functions.h"
#include "structures.h"
#include "abnf.h"
#include "http.h"

int parse_http(Node **tree, Rule **rulelist, char *input, int length) {
    int index = 0;

    if (*rulelist == NULL) compile_abnf(rulelist, ABNF_FILE_NAME);
    return parse_rule(tree, *rulelist, input, &index, length, find_rule(*rulelist, BASE_RULE, BASE_RULE_LENGTH)->rulename);
}

int parse_rule(Node **tree, Rule *rulelist, char *input, int *index, int length, String *rulename) {
    Node *n = insert_node(tree, rulename, create_string(input + *index, 0));
    Rule *r = find_rule(rulelist, rulename->base, rulename->length);
    int previous_index = *index,
        has_rule = r == NULL ? false : parse_alternation(&(n->children), rulelist, input, index, length, r->concatenations);

    if (!has_rule) delete_last_node(tree);
    else n->content->length = *index - previous_index;

    return has_rule;
}

int parse_alternation(Node **tree, Rule *rulelist, char *input, int *index, int length, Concatenation *a) {
    int has_alternation = false;

    while (a != NULL && !has_alternation) {
        has_alternation = parse_concatenation(tree, rulelist, input, index, length, a);
        a = a->next;
    }

    return has_alternation;
}

int parse_concatenation(Node **tree, Rule *rulelist, char *input, int *index, int length, Concatenation *c) {
    Repetition *r = c->repetitions;
    int previous_index = *index,
        has_concatenation = true;

    while (r != NULL && has_concatenation) {
        has_concatenation = parse_repetition(tree, rulelist, input, index, length, r);
        r = r->next;
    }
    
    if (!has_concatenation) *index = previous_index;
    return has_concatenation;
}

int parse_repetition(Node **tree, Rule *rulelist, char *input, int *index, int length, Repetition *r) {
    Repetition_type type = r->type;
    int previous_index = *index,
        has_repetition = true,
        is_valid = true,
        i = 0;

    while (i != r->max && is_valid) {
        if (type == RULENAME && !parse_rule(tree, rulelist, input, index, length, r->content.rulename)) is_valid = false;
        else if ((type == GROUP || type == OPTION) && !parse_alternation(tree, rulelist, input, index, length, r->content.concatenations)) is_valid = false;
        else if (type == CHAR_VAL && !parse_char_val(tree, rulelist, input, index, length, r->content.char_val)) is_valid = false;
        else if (type == NUM_VAL && !parse_num_val(tree, rulelist, input, index, length, r->content.num_val)) is_valid = false;
        else i++;
    }

    if (i < r->min) has_repetition = false;
    else if (r->min == 0 && i != 0 && previous_index == *index) delete_last_node(tree);
    return has_repetition;
}

int parse_char_val(Node **tree, Rule *rulelist, char *input, int *index, int length, String *s) {
    int content_length = min(length - *index, s->length),
        has_char_val = true;
    String *content = create_string(input + *index, content_length);

    insert_node(tree, find_rule(rulelist, CHAR_VAL_RULE, CHAR_VAL_RULE_LENGTH)->rulename, content);

    if (compare_strings(s, content)) *index += content_length;
    else {
        delete_last_node(tree);
        has_char_val = false;
    }

    return has_char_val;
}

int parse_num_val(Node **tree, Rule *rulelist, char *input, int *index, int length, Num_val *v) {
    String *content = create_string(input + *index, 0);
    Node *n = insert_node(tree, find_rule(rulelist, NUM_VAL_RULE, NUM_VAL_RULE_LENGTH)->rulename, content);
    int previous_index = *index,
        has_num_val = false;

    if (*index < length && v->type == MIN_MAX && is_between(*(input + *index), v->value.min_max[0], v->value.min_max[1])) {
        has_num_val = true;
        (*index)++;

    } else if (v->type == SET) has_num_val = parse_num_val_set(tree, input, index, length, v->value.set);

    if (!has_num_val) delete_last_node(tree);
    else n->content->length = *index - previous_index;

    return has_num_val;
}

int parse_num_val_set(Node **tree, char *input, int *index, int length, Num_val_set *s) {
    int previous_index = *index,
        has_num_val_set = true;

    while (s != NULL && *index < length && *(input + *index) == s->value) {
        (*index)++;
        s = s->next;
    }

    if (s != NULL) {
        *index = previous_index;
        has_num_val_set = false;
    }

    return has_num_val_set;
}