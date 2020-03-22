#include <stdio.h>
#include "parser.h"

int main() {
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

    char *a_lo_name_str = "a-lo";
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
    int x = exec_rules(o_la.children, input, 0);

    printf("Taille finale de la chaîne parsée : %d\n", x);

    return 1;
}