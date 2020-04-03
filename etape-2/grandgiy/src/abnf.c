#include <stdlib.h>
#include "functions.h"
#include "structures.h"
#include "abnf.h"

void compile_abnf(Rule **head, char **abnf_buffer, char *file_name) {
    int index = 0;

    insert_rule(head, create_string(NUM_VAL_RULE, NUM_VAL_RULE_LENGTH));
    insert_rule(head, create_string(CHAR_VAL_RULE, CHAR_VAL_RULE_LENGTH));

    read_file(file_name, abnf_buffer, get_file_size(file_name));
    if (!compile_rulelist(head, *abnf_buffer, &index)) {
        free(*abnf_buffer);
        exit_on_error("ABNF incorrect ou syntaxe non supportée.");
    }
}

int compile_rulelist(Rule **head, char *abnf, int *index) {
    int has_rulelist = true,
        is_valid = true,
        i = 0;

    while (is_valid) {
        if (!compile_rule(head, abnf, index)) {
            consume_c_wsps(abnf, index, 0);
            is_valid = compile_c_nl(abnf, index);
        
        } else i++;
    }

    if (i < 1) {
        delete_rulelist(head);
        has_rulelist = false;
    }

    return has_rulelist;
}

int compile_rule(Rule **head, char *abnf, int *index) {
    Rule *r = insert_rule(head, NULL),
         *r2;
    int previous_index = *index,
        is_redefinition = false,
        has_rule = true;

    if (!compile_rulename(abnf, index, &(r->rulename))) has_rule = false;
    else {
        r2 = find_rule(*head, r->rulename);
        is_redefinition = r != r2;

        if (is_redefinition) {
            delete_last_rule(head);
            r = r2;
        }

        if (!compile_defined_as(abnf, index, is_redefinition)) has_rule = false;
        else if (!compile_elements(&(r->concatenations), abnf, index)) has_rule = false;
        else if (!compile_c_nl(abnf, index)) has_rule = false;
    }
    
    if (!has_rule) {
        if (!is_redefinition) delete_last_rule(head);
        *index = previous_index;
    }

    return has_rule;
}

int compile_rulename(char *abnf, int *index, String **rulename) {
    int has_rulename = true;

    *rulename = create_string(abnf + *index, 0);

    if (!is_alpha(*(abnf + *index))) {
        delete_string(rulename);
        has_rulename = false;

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
        has_defined_as = true;

    consume_c_wsps(abnf, index, 0);
    if (*(abnf + *index) != '=') has_defined_as = false;
    else {
        (*index)++;

        if (*(abnf + *index) == '/') {
            if (is_redefinition) (*index)++;
            else has_defined_as = false;
        
        } else if (is_redefinition) has_defined_as = false;
    }

    if (has_defined_as) consume_c_wsps(abnf, index, 0);
    else *index = previous_index;

    return has_defined_as;
}

int compile_elements(Concatenation **head, char *abnf, int *index) {
    int has_elements = compile_alternation(head, abnf, index);

    if (has_elements) consume_c_wsps(abnf, index, 0);
    return has_elements;
}

int compile_c_wsp(char *abnf, int *index) {
    int previous_index = *index,
        has_c_wsp = true;

    if (!is_wsp(*(abnf + *index))) {
        if (!compile_c_nl(abnf, index)) has_c_wsp = false;
        else if (!is_wsp(*(abnf + *index))) has_c_wsp = false;
    }

    if (has_c_wsp) (*index)++;
    else *index = previous_index;

    return has_c_wsp;
}

int compile_c_nl(char *abnf, int *index) {
    int has_c_nl = true;

    if (!compile_comment(abnf, index)) has_c_nl = consume_crlf(abnf, index);
    return has_c_nl;
}

int compile_comment(char *abnf, int *index) {
    int previous_index = *index,
        has_comment = true;

    if (*(abnf + *index) != ';') has_comment = false;
    else {
        do (*index)++;
        while (is_wsp(*(abnf + *index)) || is_vchar(*(abnf + *index)));
        
        has_comment = consume_crlf(abnf, index);
    }

    if (!has_comment) *index = previous_index;
    return has_comment;
}

int compile_alternation(Concatenation **head, char *abnf, int *index) {
    int has_alternation = true,
        is_valid = true,
        loop_index;

    if (!compile_concatenation(head, abnf, index)) has_alternation = false;
    else {
        while (is_valid) {
            loop_index = *index;
            
            consume_c_wsps(abnf, index, 0);
            if (*(abnf + *index) != '/') is_valid = false;
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

int compile_concatenation(Concatenation **head, char *abnf, int *index) {
    Concatenation *c = insert_concatenation(head);
    int has_concatenation = true,
        is_valid = true,
        loop_index;

    if (!compile_repetition(&(c->repetitions), abnf, index))  {
        delete_last_concatenation(head);
        has_concatenation = false;

    } else {
        while (is_valid) {
            loop_index = *index;

            if (!consume_c_wsps(abnf, index, 1)) is_valid = false;
            else if (!compile_repetition(&(c->repetitions), abnf, index)) is_valid = false;

            if (!is_valid) *index = loop_index;
        }
    }

    return has_concatenation;
}

int compile_repetition(Repetition **head, char *abnf, int *index) {
    Repetition *r = insert_repetition(head);
    int has_repetition = true;

    compile_repeat(abnf, index, &(r->min), &(r->max));
    if (!compile_element(abnf, index, r)) {
        delete_last_repetition(head);
        has_repetition = false;
    }

    return has_repetition;
}

int compile_repeat(char *abnf, int *index, int *min, int *max) {
    int has_repeat = true,
        is_consumed = consume_number(abnf, index, 10, min, 1);

    if (*(abnf + *index) != '*') {
        if (!is_consumed) {
            has_repeat = false;
            *min = 1;
        }

        *max = *min;

    } else {
        (*index)++;
        if (!consume_number(abnf, index, 10, max, 1)) *max = -1;
    }

    return has_repeat;
}

int compile_element(char *abnf, int *index, Repetition *r) {
    int has_element = true;

    if (compile_rulename(abnf, index, &(r->content.rulename))) r->type = RULENAME;
    else if (compile_group_or_option(&(r->content.concatenations), abnf, index, HAS_GROUP)) r->type = GROUP;
    else if (compile_group_or_option(&(r->content.concatenations), abnf, index, HAS_OPTION)) {
        r->type = OPTION;
        r->min = 0;
        r->max = 1;

    } else if (compile_char_val(abnf, index, &(r->content.char_val))) r->type = CHAR_VAL;
    else if (compile_num_val(abnf, index, &(r->content.num_val))) r->type = NUM_VAL;
    else has_element = false; // No support for prose-val

    return has_element;
}

int compile_group_or_option(Concatenation **head, char *abnf, int *index, Has_group_or_option_type type) {
    char opening_char = type == HAS_GROUP ? '(' : '[',
         closing_char = type == HAS_GROUP ? ')' : ']';
    int previous_index = *index,
        has_group_or_option = true;

    if (*(abnf + *index) != opening_char) has_group_or_option = false;
    else {
        (*index)++;

        consume_c_wsps(abnf, index, 0);
        if (!compile_alternation(head, abnf, index)) has_group_or_option = false;
        else {
            consume_c_wsps(abnf, index, 0);
            if (*(abnf + *index) != closing_char) has_group_or_option = false;
            else (*index)++;
        }
    }

    if (!has_group_or_option) *index = previous_index;
    return has_group_or_option;
}

int compile_char_val(char *abnf, int *index, String **char_val) {
    int previous_index = *index,
        has_char_val = true;

    *char_val = create_string(abnf + *index + 1, 0);

    if (*(abnf + *index) != '"') has_char_val = false;
    else {
        (*index)++;

        while ((is_vchar(*(abnf + *index)) && *(abnf + *index) != '"') || *(abnf + *index) == ' ') {
            (*index)++;
            (*char_val)->length++;
        }

        if (*(abnf + *index) != '"') has_char_val = false;
        else (*index)++;
    }

    if (!has_char_val) {
        delete_string(char_val);
        *index = previous_index;
    }

    return has_char_val;
}

int compile_num_val(char *abnf, int *index, Num_val **num_val) {
    int has_num_val = true;
    *num_val = create_num_val();

    if (*(abnf + *index) != '%') has_num_val = false;
    else {
        (*index)++;

        if (!compile_bin_or_dec_or_hex_val(abnf, index, *num_val, 2)) {
            if (!compile_bin_or_dec_or_hex_val(abnf, index, *num_val, 10)) {
                if (!compile_bin_or_dec_or_hex_val(abnf, index, *num_val, 16)) has_num_val = false;
            }
        }
    }

    if (!has_num_val) delete_num_val(num_val);
    return has_num_val;
}

int compile_bin_or_dec_or_hex_val(char *abnf, int *index, Num_val *num_val, int base) {
    char base_indicator;
    int previous_index = *index,
        has_bin_or_dec_or_hex_val = true,
        value;

    if (base == 2) base_indicator = 'b';
    else if (base == 10) base_indicator = 'd';
    else if (base == 16) base_indicator = 'x';

    if (!compare_insensitive(*(abnf + *index), base_indicator)) has_bin_or_dec_or_hex_val = false;
    else {
        (*index)++;

        if (!consume_number(abnf, index, base, &value, 1)) has_bin_or_dec_or_hex_val = false;
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