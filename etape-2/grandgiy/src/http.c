#include "functions.h"
#include "structures.h"
#include "abnf.h"
#include "http.h"

// Ces variables sont globales pour pouvoir être réutilisées d'une requête à l'autre, mais aussi pour permettre la libération de la mémoire allouée
char *abnf_buffer = NULL;
Rule *rulelist = NULL;

// Parse une requête HTTP à partir de la syntaxe ABNF
// Retourne true si la requête est syntaxiquement correcte, false sinon
int parse_http(Node **tree, char *input, int length) {
    char *base = BASE_RULE;
    String rulename = { base, BASE_RULE_LENGTH };
    int index = 0;
    Rule *r;

    // "Précompile" l'ABNF si cela n'a pas déjà été fait
    // Cette ligne n'est utile que si une exécution du code peut parser plusieurs requêtes HTTP
    if (rulelist == NULL) compile_abnf(&rulelist, &abnf_buffer, ABNF_FILE_NAME);
    // Le nom de la règle est récupéré de cette manière afin d'utiliser la chaîne déjà présente en mémoire
    // Cela évite d'avoir deux blocs de mémoire distincts mais possédant le même contenu
    r = find_rule(rulelist, &rulename);

    // Parse la requête à partir de la structure et retourne le résultat, si la règle d'entrée existe
    return r == NULL ? false : parse_rule(tree, input, &index, length, r->rulename);
}

// Les fonctions suivantes construisent un arbre à partir de la structure de données représentant l'ABNF
// Dans toutes les fonctions, index correspond à l'index du caractère courant
// Il est de type int * afin de pouvoir revenir en arrière facilement lors d'une recherche échouée

// Vérifie la validité de la requête par rapport à une règle et construit l'arbre
int parse_rule(Node **tree, char *input, int *index, int length, String *rulename) {
    // Le noeud est inséré dans tous les cas mais il est supprimé si nécessaire
    Node *n = insert_node(tree, rulename, create_string(input + *index, 0));
    Rule *r = find_rule(rulelist, rulename);
    int previous_index = *index,
        // La validité de la règle repose sur la validité de la série d'alternatives de la règle
        has_rule = r == NULL ? false : parse_alternation(&(n->children), input, index, length, r->concatenations);

    // Si la règle est invalide, libère la mémoire associée au noeud et tous ses enfants
    if (!has_rule) delete_last_node(tree);
    // Si la règle est valide, met à jour la longueur du noeud
    else n->content->length = *index - previous_index;

    return has_rule;
}

// Vérifie la validité de la requête par rapport à une série d'alternatives et construit l'arbre
int parse_alternation(Node **tree, char *input, int *index, int length, Concatenation *a) {
    int has_alternation = false;

    // Recherche successivement les différentes alternatives jusqu'à ce qu'une recherche soit un succès
    // Si toutes les recherches échouent, la série d'alternatives est invalide
    while (a != NULL && !has_alternation) {
        // La validité d'une alternative repose sur la validité de la concaténation de l'alternative
        has_alternation = parse_concatenation(tree, input, index, length, a);
        a = a->next;
    }

    return has_alternation;
}

// Vérifie la validité de la requête par rapport à une concaténation et construit l'arbre
int parse_concatenation(Node **tree, char *input, int *index, int length, Concatenation *c) {
    Repetition *r = c->repetitions;
    int previous_index = *index,
        has_concatenation = true;

    // Recherche successivement les différentes répétitions de la concaténation
    // Si une recherche échoue, la concaténation est invalide
    while (r != NULL && has_concatenation) {
        // La validité de la concaténation repose sur la validité des répétitions de la concaténation
        has_concatenation = parse_repetition(tree, input, index, length, r);
        r = r->next;
    }

    // Si la concaténation est invalide, replace le pointeur d'index au bon endroit
    if (!has_concatenation) *index = previous_index;
    return has_concatenation;
}

// Vérifie la validité de la requête par rapport à une répétition et construit l'arbre
int parse_repetition(Node **tree, char *input, int *index, int length, Repetition *r) {
    Repetition_type type = r->type;
    int previous_index = *index,
        has_repetition = true,
        is_valid = true,
        i = 0;

    // Recherche successivement les différentes membres de la répétitions jusqu'à ce qu'une recherche échoue
    // Suivant le type de la répétition, la validité de la répétition dépend de la validité de la règle, du groupe, de l'option, de la chaîne de caractères ou de la valeur numérique de la répétition
    while (i != r->max && is_valid) {
        if (type == RULENAME && !parse_rule(tree, input, index, length, r->content.rulename)) is_valid = false;
        else if ((type == GROUP || type == OPTION) && !parse_alternation(tree, input, index, length, r->content.concatenations)) is_valid = false;
        else if (type == CHAR_VAL && !parse_char_val(tree, input, index, length, r->content.char_val)) is_valid = false;
        else if (type == NUM_VAL && !parse_num_val(tree, input, index, length, r->content.num_val)) is_valid = false;
        // Le tour de boucle n'est compté que si le membre est valide
        else i++;
    }

    // Si le nombre de répétitions capturées est insuffisant, la répétition est invalide
    if (i < r->min) has_repetition = false;
    // Si rien n'a été capturé mais que ce n'est pas un problème, libère la mémoire associée au dernier membre de la répétition, qui est vide
    else if (r->min == 0 && i != 0 && previous_index == *index) delete_last_node(tree);
    return has_repetition;
}

// Vérifie la validité de la requête par rapport à une chaîne de caractères et construit l'arbre
int parse_char_val(Node **tree, char *input, int *index, int length, String *s) {
    // La chaîne de caractères ne doit pas "sortir" du bloc de mémoire en ayant une longueur trop grande
    int content_length = min(length - *index, s->length),
        has_char_val = true;
    char *base = CHAR_VAL_RULE;
    String *content = create_string(input + *index, content_length),
           rulename = { base, CHAR_VAL_RULE_LENGTH };

    // Le noeud est inséré dans tous les cas mais il est supprimé si nécessaire
    // Le nom de la règle est récupéré de cette manière afin d'utiliser la chaîne déjà présente en mémoire
    insert_node(tree, find_rule(rulelist, &rulename)->rulename, content);

    // Si les deux chaînes de caractères sont égales, la chaîne est valide
    if (compare_strings(s, content)) *index += content_length;
    // Si les deux chaînes sont différentes, la chaîne est invalide
    // Dans ce cas, libère la mémoire associée au noeud
    else {
        delete_last_node(tree);
        has_char_val = false;
    }

    return has_char_val;
}

// Vérifie la validité de la requête par rapport à une valeur numérique et construit l'arbre
int parse_num_val(Node **tree, char *input, int *index, int length, Num_val *v) {
    char *base = NUM_VAL_RULE;
    String *content = create_string(input + *index, 0),
           rulename = { base, NUM_VAL_RULE_LENGTH };
    // Le noeud est inséré dans tous les cas mais il est supprimé si nécessaire
    // Le nom de la règle est récupéré de cette manière afin d'utiliser la chaîne déjà présente en mémoire
    Node *n = insert_node(tree, find_rule(rulelist, &rulename)->rulename, content);
    int previous_index = *index,
        has_num_val = false;

    // Si la valeur numérique désigne un intervale, elle est valide si le caractère appartient à l'intervalle
    if (*index < length && v->type == MIN_MAX && is_between(*(input + *index), v->value.min_max[0], v->value.min_max[1])) {
        has_num_val = true;
        (*index)++;

    // Si la valeur numérique désigne une suite de nombres, sa validité repose sur la validité de chacun des membres de la suite 
    } else if (v->type == SET) has_num_val = parse_num_val_set(tree, input, index, length, v->value.set);

    // Si la valeur numérique est invalide, libère la mémoire associée au noeud et tous ses enfants
    if (!has_num_val) delete_last_node(tree);
    // Si la valeur numérique est valide, met à jour la longueur du noeud
    else n->content->length = *index - previous_index;

    return has_num_val;
}

// Vérifie la validité de la requête par rapport à une suite de nombres
int parse_num_val_set(Node **tree, char *input, int *index, int length, Num_val_set *s) {
    int previous_index = *index,
        has_num_val_set = true;

    // Compare chaque valeur de la suite avec les caractères de la requête
    // Si une des comparaisons échoue, la recherche s'arrête
    while (s != NULL && *index < length && *(input + *index) == s->value) {
        (*index)++;
        s = s->next;
    }

    // Si une des comparaisons a échoué, la suite de nombres est invalide
    // Dans ce cas, replace le pointeur d'index au bon endroit
    if (s != NULL) {
        *index = previous_index;
        has_num_val_set = false;
    }

    return has_num_val_set;
}