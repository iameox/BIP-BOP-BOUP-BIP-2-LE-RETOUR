#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "dot-segments.h"

void remove_dot_segments(char *string, int *length) {
    int_stack *index_stack = NULL;
    int real_index = 0,
        virtual_index = 0,
        p_len;
    char *s;

    while (real_index < *length) {
        s = string + real_index;
        p_len = *length - real_index;

        if (match_prefix(s, "../", &p_len) || match_prefix(s, "./", &p_len)) {
            real_index += p_len;

        } else if ((match_prefix(s, "/./", NULL) || compare_strings(s, "/.", p_len))) {
            real_index += 2;
            string[real_index] = '/';

        } else if ((match_prefix(s, "/../", NULL) || compare_strings(s, "/..", p_len))) {
            virtual_index = pop_stack(&index_stack);

            real_index += 3;
            string[real_index] = '/';

        } else if ((match_prefix(s, "..", &p_len) || match_prefix(s, ".", &p_len)) && *length == p_len) {
            real_index += p_len;

        } else if (*s == '/') {
            push_stack(&index_stack, virtual_index);
            
            do {
                string[virtual_index] = string[real_index];
                real_index++;
                virtual_index++;

            } while (real_index < *length && *(string + real_index) != '/');
        }
    }

    string[virtual_index] = '\0';
    *length = virtual_index;

    clear_stack(&index_stack);
}

int push_stack(int_stack **stack, int top) {
    int_stack *new = malloc(sizeof(int_stack));
    if (new == NULL) exit(EXIT_FAILURE);

    new->top = top;
    new->next = *stack;

    *stack = new;
}

int pop_stack(int_stack **stack) {
    int_stack *element = *stack;
    int top = 0;

    if (element != NULL) {
        *stack = element->next;
        top = element->top;

        free(element);
    }

    return top;
}

void clear_stack(int_stack **stack) {
    while (*stack != NULL) pop_stack(stack);
}

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(char *s1, char *s2, int len) {
    int p_len;
    return match_prefix(s1, s2, &p_len) && len == p_len;
}

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(char *s, char *p, int *len) {
    int matched = true,
        i = 0;

    // Comparaison caractère par caractère, tant que le préfixe n'a pas été consummé
    while (p[i] != '\0' && matched) {
        if (s[i] != p[i]) matched = false;

        i++;
    }

    if (matched && len != NULL) *len = i;
    return matched;
}

int main() {
    char s[] = "/ma/super/url/./../voiture/../../../mon/super/sac/a/dos/./././../main/haha/je/te/hack/../../../../../../../../../../../";
    int length = strlen(s);
    
    printf("Chaîne : %s\nLongueur : %d\n", s, length);
    remove_dot_segments(s, &length);
    printf("Nouvelle chaîne : %s\nNouvelle longueur : %d\n", s, length);

    return EXIT_SUCCESS;
}
/*
/ma/super/url/./../voiture/../../../mon/super/sac/a/dos/./././../main/haha/je/te/hack/../../../../../../../../../../../
*/