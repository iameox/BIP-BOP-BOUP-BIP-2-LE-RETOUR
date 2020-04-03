#include "functions.h"
#include "structures.h"
#include "abnf.h"

void compile_abnf(rule **head, char *file_name) {
    char *abnf = NULL;
    int index = 0;

    read_file(file_name, &abnf, get_file_size(file_name));
    if (!compile_rulelist(head, abnf, &index)) exit_on_error("ABNF incorrect ou syntaxe non supportée.");

    insert_rule(head, NUM_VAL_RULE, NUM_VAL_RULE_LENGTH);
    insert_rule(head, CHAR_VAL_RULE, CHAR_VAL_RULE_LENGTH);
}

int compile_rulelist(rule **head, char *abnf, int *index) {
    int is_valid = 1,
        i = 0;

    while (is_valid) {
        if (!compile_rule(head, abnf, index)) {
            consume_c_wsps(abnf, index, 0);
            is_valid = compile_c_nl(abnf, index);
        
        } else i++;
    }

    return i >= 1;
}

int compile_rule(rule **head, char *abnf, int *index) {
    rule *r = insert_rule(head, NULL, 0),
         *r2;
    int previous_index = *index,
        is_redefinition = 0,
        has_rule = 1;

    if (!compile_rulename(abnf, index, &(r->rulename))) has_rule = 0;
    else {
        r2 = find_rule(*head, r->rulename->base, r->rulename->length);
        is_redefinition = r != r2;

        if (is_redefinition) {
            delete_last_rule(head);
            r = r2;
        }

        if (!compile_defined_as(abnf, index, is_redefinition)) has_rule = 0;
        else if (!compile_elements(&(r->concatenations), abnf, index)) has_rule = 0;
        else if (!compile_c_nl(abnf, index)) has_rule = 0;
    }
    
    if (!has_rule) {
        if (!is_redefinition) delete_last_rule(head);
        *index = previous_index;
    }

    return has_rule;
}

int compile_rulename(char *abnf, int *index, string **rulename) {
    int has_rulename = 1;

    *rulename = create_string(abnf + *index, 0);

    if (!is_alpha(*(abnf + *index))) {
        delete_string(rulename);
        has_rulename = 0;

    } else {
        do {
            (*index)++;
            (*rulename)->length++;

        } while (is_alpha(*(abnf + *index)) || is_digit(*(abnf + *index), 10) || *(abnf + *index) == '-');
    }

    return has_rulename;
}

int compile_defined_as(char *abnf, int *index, int is_redefinition) {
    int previous_index = *index,
        has_defined_as = 1;

    consume_c_wsps(abnf, index, 0);
    if (*(abnf + *index) != '=') has_defined_as = 0;
    else {
        (*index)++;

        if (*(abnf + *index) == '/') {
            if (is_redefinition) (*index)++;
            else has_defined_as = 0;
        
        } else if (is_redefinition) has_defined_as = 0;
    }

    if (has_defined_as) consume_c_wsps(abnf, index, 0);
    else *index = previous_index;

    return has_defined_as;
}

int compile_elements(concatenation **head, char *abnf, int *index) {
    int has_elements = compile_alternation(head, abnf, index);

    if (has_elements) consume_c_wsps(abnf, index, 0);
    return has_elements;
}

int compile_c_wsp(char *abnf, int *index) {
    int previous_index = *index,
        has_c_wsp = 1;

    if (!is_wsp(*(abnf + *index))) {
        if (!compile_c_nl(abnf, index)) has_c_wsp = 0;
        else if (!is_wsp(*(abnf + *index))) has_c_wsp = 0;
    }

    if (has_c_wsp) (*index)++;
    else *index = previous_index;

    return has_c_wsp;
}

int compile_c_nl(char *abnf, int *index) {
    int has_c_nl = 1;

    if (!compile_comment(abnf, index)) has_c_nl = consume_crlf(abnf, index);
    return has_c_nl;
}

int compile_comment(char *abnf, int *index) {
    int previous_index = *index,
        has_comment = 1;

    if (*(abnf + *index) != ';') has_comment = 0;
    else {
        do (*index)++;
        while (is_wsp(*(abnf + *index)) || is_vchar(*(abnf + *index)));
        
        has_comment = consume_crlf(abnf, index);
    }

    if (!has_comment) *index = previous_index;
    return has_comment;
}

int compile_alternation(concatenation **head, char *abnf, int *index) {
    int has_alternation = 1,
        is_valid = 1,
        loop_index;

    if (!compile_concatenation(head, abnf, index)) has_alternation = 0;
    else {
        while (is_valid) {
            loop_index = *index;
            
            consume_c_wsps(abnf, index, 0);
            if (*(abnf + *index) != '/') is_valid = 0;
            else {
                (*index)++;
                consume_c_wsps(abnf, index, 0);
                is_valid = compile_concatenation(head, abnf, index);
            }

            if (!is_valid) *index = loop_index;
        }
    }

    return has_alternation;
}

int compile_concatenation(concatenation **head, char *abnf, int *index) {
    concatenation *c = insert_concatenation(head);
    int has_concatenation = 1,
        is_valid = 1,
        loop_index;

    if (!compile_repetition(&(c->repetitions), abnf, index))  {
        delete_last_concatenation(head);
        has_concatenation = 0;

    } else {
        while (is_valid) {
            loop_index = *index;

            if (!consume_c_wsps(abnf, index, 1)) is_valid = 0;
            else if (!compile_repetition(&(c->repetitions), abnf, index)) is_valid = 0;

            if (!is_valid) *index = loop_index;
        }
    }

    return has_concatenation;
}

int compile_repetition(repetition **head, char *abnf, int *index) {
    repetition *r = insert_repetition(head);
    int has_repetition = 1;

    compile_repeat(abnf, index, &(r->min), &(r->max));
    if (!compile_element(abnf, index, r)) {
        delete_last_repetition(head);
        has_repetition = 0;
    }

    return has_repetition;
}

int compile_repeat(char *abnf, int *index, int *min, int *max) {
    int has_repeat = 1,
        is_consumed = consume_number(abnf, index, 10, min, 1);

    if (*(abnf + *index) != '*') {
        if (!is_consumed) {
            has_repeat = 0;
            *min = 1;
        }

        *max = *min;

    } else {
        (*index)++;
        if (!consume_number(abnf, index, 10, max, 1)) *max = -1;
    }

    return has_repeat;
}

int compile_element(char *abnf, int *index, repetition *r) {
    int has_element = 1;

    if (compile_rulename(abnf, index, &(r->content.rulename))) r->type = RULENAME;
    else if (compile_group_or_option(&(r->content.concatenations), abnf, index, HAS_GROUP)) r->type = GROUP;
    else if (compile_group_or_option(&(r->content.concatenations), abnf, index, HAS_OPTION)) {
        r->type = OPTION;
        r->min = 0;
        r->max = 1;

    } else if (compile_char_val(abnf, index, &(r->content.char_val))) r->type = CHAR_VAL;
    else if (compile_num_val(abnf, index, &(r->content.num_val))) r->type = NUM_VAL;
    else has_element = 0; // No support for prose-val

    return has_element;
}

int compile_group_or_option(concatenation **head, char *abnf, int *index, has_group_or_option_type type) {
    char opening_char = type == HAS_GROUP ? '(' : '[',
         closing_char = type == HAS_GROUP ? ')' : ']';
    int previous_index = *index,
        has_group_or_option = 1;

    if (*(abnf + *index) != opening_char) has_group_or_option = 0;
    else {
        (*index)++;

        consume_c_wsps(abnf, index, 0);
        if (!compile_alternation(head, abnf, index)) has_group_or_option = 0;
        else {
            consume_c_wsps(abnf, index, 0);
            if (*(abnf + *index) != closing_char) has_group_or_option = 0;
            else (*index)++;
        }
    }

    if (!has_group_or_option) *index = previous_index;
    return has_group_or_option;
}

int compile_char_val(char *abnf, int *index, string **char_val) {
    int previous_index = *index,
        has_char_val = 1;

    *char_val = create_string(abnf + *index + 1, 0);

    if (*(abnf + *index) != '"') has_char_val = 0;
    else {
        (*index)++;

        while ((is_vchar(*(abnf + *index)) && *(abnf + *index) != '"') || *(abnf + *index) == ' ') {
            (*index)++;
            (*char_val)->length++;
        }

        if (*(abnf + *index) != '"') has_char_val = 0;
        else (*index)++;
    }

    if (!has_char_val) {
        delete_string(char_val);
        *index = previous_index;
    }

    return has_char_val;
}

int compile_num_val(char *abnf, int *index, num_val **num_val) {
    int has_num_val = 1;
    *num_val = create_num_val();

    if (*(abnf + *index) != '%') has_num_val = 0;
    else {
        (*index)++;

        if (!compile_bin_or_dec_or_hex_val(abnf, index, *num_val, 2)) {
            if (!compile_bin_or_dec_or_hex_val(abnf, index, *num_val, 10)) {
                if (!compile_bin_or_dec_or_hex_val(abnf, index, *num_val, 16)) has_num_val = 0;
            }
        }
    }

    if (!has_num_val) delete_num_val(num_val);
    return has_num_val;
}

int compile_bin_or_dec_or_hex_val(char *abnf, int *index, num_val *num_val, int base) {
    char base_indicator;
    int previous_index = *index,
        has_bin_or_dec_or_hex_val = 1,
        value;

    if (base == 2) base_indicator = 'b';
    else if (base == 10) base_indicator = 'd';
    else if (base == 16) base_indicator = 'x';

    if (!compare_insensitive(*(abnf + *index), base_indicator)) has_bin_or_dec_or_hex_val = 0;
    else {
        (*index)++;

        if (!consume_number(abnf, index, base, &value, 1)) has_bin_or_dec_or_hex_val = 0;
        else if (*(abnf + *index) == '-') {
            num_val->value.min_max[0] = value;
            num_val->type = MIN_MAX;
            (*index)++;

            if (!consume_number(abnf, index, base, num_val->value.min_max + 1, 1)) (*index)--;
        
        } else {
            num_val->type = SET;
            insert_set_value(&(num_val->value.set), value);

            while (*(abnf + *index) == '.') {
                (*index)++;

                if (!consume_number(abnf, index, base, &value, 1)) (*index)--;
                insert_set_value(&(num_val->value.set), value);
            }
        }
    }

    if (!has_bin_or_dec_or_hex_val) *index = previous_index;
    return has_bin_or_dec_or_hex_val;
}

int consume_c_wsps(char *abnf, int *index, int min) {
    int i = 0;

    while (compile_c_wsp(abnf, index)) i++;
    return i >= min;
}

int consume_crlf(char *abnf, int *index) {
    int is_consumed = is_crlf(abnf + *index);

    if (is_consumed) (*index) += 2;
    return is_consumed;
}

int consume_number(char *abnf, int *index, int base, int *value, int min) {
    int i = 0;
    *value = 0;

    while (is_digit(*(abnf + *index), base)) {
        *value = *value * base + char_to_digit(*(abnf + *index), base);
        (*index)++;

        i++;
    }

    return i >= min;
}