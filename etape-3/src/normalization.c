#include <stdlib.h>
#include "utils.h"
#include "normalization.h"

// Normalise le champ request-target d'une URI
// Décode tous les "percent-encodings" et les "dot segments"
void normalize_request_target(string *request_target) {
    normalize_percent(request_target);
    remove_dot_segments(request_target);
}

// Normalise une URI du point de vue des "percent encodings" (décode toutes les occurences)
void normalize_percent(string *str) {
    // real_index représente l'index de parcourt de la chaîne d'entrée
    // virtual_index représente l'index de parcourt de la chaîne finale, c'est la position du prochain caractère à écrire
    int real_index = 0,
        virtual_index = 0,
        value, digit, i;
    char *s, c;

    while (real_index < str->length) {
        s = str->base + real_index;
        value = 0;

        if (s[0] == '%') {
            // Comme la chaîne est syntaxiquement valide, la présence d'un "%" indique forcément un percent-encoding
            // Décode les 2 caractères suivants le "%"
            for (i = 1; i < 3; i++) {
                if (is_between(s[i], 'a', 'f')) s[i] += 'A' - 'a';
                c = s[i];

                // Toutes les vérifications ont déjà été faites au parsing, il est inutile d'en faire davantage pour cette conversion
                digit = c >= 'A' ? 10 + c - 'A' : c - '0';
                value = 16 * value + digit;
            }
            
            str->base[virtual_index] = value;
            real_index += 3;

        // Le caractère n'indique pas le début d'un percent-encoding, il est simplement recopié
        } else {
            str->base[virtual_index] = str->base[real_index];
            real_index++;
        }

        virtual_index++;
    }

    // Positionne la nouvelle fin de la chaîne
    str->base[virtual_index] = '\0';
    str->length = virtual_index;
}

// Normalise une URI du point de vue des "dot segments"
// Applique l'algorithme de la partie 5.2.4 de la RFC 3986
void remove_dot_segments(string *str) {
    int_stack *index_stack = NULL;
    // real_index représente l'index de parcourt de la chaîne d'entrée
    // virtual_index représente l'index de parcourt de la chaîne finale, c'est la position du prochain caractère à écrire
    int real_index = 0,
        virtual_index = 0,
        p_len;
    string s;

    while (real_index < str->length) {
        s.base = str->base + real_index;
        s.length = str->length - real_index;

        if (match_prefix(&s, "../", &p_len) || match_prefix(&s, "./", &p_len) || ((match_prefix(&s, "..", &p_len) || match_prefix(&s, ".", &p_len)) && str->length == p_len)) real_index += p_len;
        else if ((match_prefix(&s, "/./", NULL) || compare_strings(&s, "/."))) {
            // Le motif doit être remplacé par "/"
            real_index += 2;
            str->base[real_index] = '/';

        // Il faut revenir un pas en arrière dans l'arborescence du répertoire
        } else if ((match_prefix(&s, "/../", NULL) || compare_strings(&s, "/.."))) {
            // Retrouve l'index du "/" précédent (ignore tout ce qui a été réécrit ensuite)
            virtual_index = pop_stack(&index_stack);

            // Le motif doit être remplacé par "/"
            real_index += 3;
            str->base[real_index] = '/';

        } else {
            // Empile la position actuelle du dernier "/" rencontré, au cas où il y ait un motif "/../" ou "/.." dans la suite de la chaîne
            push_stack(&index_stack, virtual_index);
            
            // Décale tous les caractères jusqu'au prochain "/" exclus à leur position dans la chaîne finale
            do {
                str->base[virtual_index] = str->base[real_index];
                real_index++;
                virtual_index++;

            } while (real_index < str->length && str->base[real_index] != '/');
        }
    }

    // Positionne la nouvelle fin de la chaîne
    str->base[virtual_index] = '\0';
    str->length = virtual_index;

    // Libère la mémoire associée à la pile
    clear_stack(&index_stack);
}

// Empile top au sommet de stack
void push_stack(int_stack **stack, int top) {
    int_stack *new = malloc(sizeof(int_stack));
    new->top = top;
    new->next = *stack;

    *stack = new;
}

// Dépile et renvoie la tête de stack si elle existe, 0 sinon
int pop_stack(int_stack **stack) {
    int_stack *element = *stack;
    int top = 0;

    if (element != NULL) {
        *stack = element->next;
        top = element->top;

        // Libère la mémoire associée à l'ancienne tête
        free(element);
    }

    return top;
}

// Libère la mémoire associée à la totalité de stack
void clear_stack(int_stack **stack) {
    while (*stack != NULL) pop_stack(stack);
}