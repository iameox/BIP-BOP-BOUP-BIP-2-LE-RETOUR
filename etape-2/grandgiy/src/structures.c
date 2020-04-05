#include <stdlib.h>
#include "functions.h"
#include "structures.h"

// Insère une nouvelle règle dans la liste donnée
// L'insertion se fait en queue car l'ordre d'insertion est important pour les ajouts d'alternatives
Rule *insert_rule(Rule **head, String *rulename) {
    Rule *new = create_element(sizeof(Rule)),
         *element = *head;

    new->rulename = rulename;
    new->concatenations = NULL;
    new->next = NULL;

    // Si la liste est vide, insère en tête
    if (element == NULL) *head = new;
    // Si la liste est non vide, parcourt la liste et insère en queue
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

// Recherche la règle de nom demandé dans la liste donnée
// Retourne NULL si elle n'existe pas
Rule *find_rule(Rule *head, String *rulename) {
    Rule *element = head,
         *value = NULL;

    // Recherche dans la liste jusqu'à ce qu'une règle ait le nom demandé
    while (element != NULL && !compare_strings(element->rulename, rulename)) element = element->next;
    // Si une règle possède le nom demandé, il faut mettre à jour la valeur de retour
    if (element != NULL) value = element;

    return value;
}

// Libère la mémoire associée à la dernière règle de la liste donnée, si elle n'est pas vide
void delete_last_rule(Rule **head) {
    Rule *element = *head,
         *previous = *head;

    if (element != NULL) {
        // Si la liste ne contient qu'un seul élément, libère la tête
        if (element->next == NULL) *head = NULL;
        // Si la liste contient plus d'un élément, parcourt la liste et libère le dernier élément
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        // Libère également la mémoire associée au nom de la règle et à tout son contenu
        delete_string(&(element->rulename));
        delete_concatenations(&(element->concatenations));
        free(element);
    }
}

// Libère la totalité de la mémoire associée à la liste de règles données
void delete_rulelist(Rule **head) {
    while (*head != NULL) delete_last_rule(head);
}

// Insère une nouvelle concaténation dans la liste donnée
// L'insertion se fait en queue car l'ordre d'insertion est important pour le parsing
Concatenation *insert_concatenation(Concatenation **head) {
    Concatenation *new = create_element(sizeof(Concatenation)),
                  *element = *head;

    new->repetitions = NULL;
    new->next = NULL;

    // Si la liste est vide, insère en tête
    if (element == NULL) *head = new;
    // Si la liste est non vide, parcourt la liste et insère en queue
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

// Libère la mémoire associée à la dernière concaténation de la liste donnée, si elle n'est pas vide
void delete_last_concatenation(Concatenation **head) {
    Concatenation *element = *head,
                  *previous = *head;

    if (element != NULL) {
        // Si la liste ne contient qu'un seul élément, libère la tête
        if (element->next == NULL) *head = NULL;
        // Si la liste contient plus d'un élément, parcourt la liste et libère le dernier élément
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        // Libère également la mémoire associée à tout son contenu
        delete_repetitions(&(element->repetitions));
        free(element);
    }
}

// Libère la totalité de la mémoire associée à la liste de concaténations donnée
void delete_concatenations(Concatenation **head) {
    while (*head != NULL) delete_last_concatenation(head);
}

// Insère une nouvelle répétition dans la liste donnée
// L'insertion se fait en queue car l'ordre d'insertion est important pour le parsing
Repetition *insert_repetition(Repetition **head) {
    Repetition *new = create_element(sizeof(Repetition)),
               *element = *head;

    new->type = NO_REPETITION_TYPE;
    new->content.concatenations = NULL;
    new->next = NULL;

    // Si la liste est vide, insère en tête
    if (element == NULL) *head = new;
    // Si la liste est non vide, parcourt la liste et insère en queue
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

// Libère la mémoire associée à la dernière répétition de la liste donnée, si elle n'est pas vide
void delete_last_repetition(Repetition **head) {
    Repetition *element = *head,
               *previous = *head;

    if (element != NULL) {
        // Si la liste ne contient qu'un seul élément, libère la tête
        if (element->next == NULL) *head = NULL;
        // Si la liste contient plus d'un élément, parcourt la liste et libère le dernier élément
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        // Libère également la mémoire associée à tout son contenu, suivant le type de la répétition
        if (element->type == RULENAME) delete_string(&(element->content.rulename));
        else if (element->type == GROUP || element->type == OPTION) delete_concatenations(&(element->content.concatenations));
        else if (element->type == CHAR_VAL) delete_string(&(element->content.char_val));
        else if (element->type == NUM_VAL) delete_num_val(&(element->content.num_val));
        free(element);
    }
}

// Libère la totalité de la mémoire associée à la liste de répétitions donnée
void delete_repetitions(Repetition **head) {
    while (*head != NULL) delete_last_repetition(head);
}

// Crée une nouvelle valeur numérique
// Ce type est utilisé pour manipuler soit des intervalles lorsque le type est "MIN_MAX", soit des suites de nombres lorsque le type est "SET"
Num_val *create_num_val() {
    Num_val *new = create_element(sizeof(Num_val));
    new->type = NO_NUM_VAL_TYPE;
    new->value.set = NULL;

    return new;
}

// Libère la mémoire associée à la valeur numérique donnée, si elle n'est pas vide
void delete_num_val(Num_val **head) {
    Num_val *element = *head;

    if (element != NULL) {
        // Libère également la mémoire associée à tout son contenu s'il s'agit d'une suite de nombre
        if (element->type == SET) delete_set(&(element->value.set));

        *head = NULL;
        free(element);
    }
}

// Insère un nouveau nombre dans la suite donnée
// L'insertion se fait en queue car l'ordre d'insertion est important pour le parsing
Num_val_set *insert_set_value(Num_val_set **head, int value) {
    Num_val_set *new = create_element(sizeof(Num_val_set)),
                *element = *head;

    new->value = value;
    new->next = NULL;

    // Si la liste est vide, insère en tête
    if (element == NULL) *head = new;
    // Si la liste est non vide, parcourt la liste et insère en queue
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

// Libère la mémoire associée au dernier nombre de la suite donnée, si elle n'est pas vide
void delete_last_set_value(Num_val_set **head) {
    Num_val_set *element = *head,
                *previous = *head;

    if (element != NULL) {
        // Si la liste ne contient qu'un seul élément, libère la tête
        if (element->next == NULL) *head = NULL;
        // Si la liste contient plus d'un élément, parcourt la liste et libère le dernier élément
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        free(element);
    }
}

// Libère la totalité de la mémoire associée à la suite de nombres donnée
void delete_set(Num_val_set **head) {
    while (*head != NULL) delete_last_set_value(head);
}

// Insère un nouveau noeud dans l'arbre donné
// L'insertion se fait en feuille car l'ordre d'insertion est important pour le traitement
Node *insert_node(Node **head, String *rulename, String *content) {
    Node *new = create_element(sizeof(Node)),
         *element = *head;

    new->rulename = rulename;
    new->content = content;
    new->children = NULL;
    new->next = NULL;

    // Si l'arbre est vide, insère à la racine
    if (element == NULL) *head = new;
    // Si l'arbre est non vide, parcourt l'arbre et insère en feuille
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

// Recherche le index-ieme noeud de nom demandé dans l'arbre donné
// Retourne NULL s'il n'existe pas
Node *find_node(Node *head, String *rulename, int *i, int index) {
    Node *element = head,
         *value = NULL;

    // Recherche dans la l'arbre jusqu'à ce qu'un noeud ait le nom demandé
    while (element != NULL && value == NULL) {
        // Si le noeud possède le nom demandé, deux cas sont possibles
        // S'il s'agit du index-ieme noeud trouvé, il faut mettre à jour la valeur de retour
        // Si ce n'est pas le cas, il faut continuer la recherche
        if (compare_strings(element->rulename, rulename)) {
            if (*i == index) value = element;
            else (*i)++;

        // Si le noeud ne possède pas le noeud demandé, recherche dans ses enfants
        } else value = find_node(element->children, rulename, i, index);

        element = element->next;
    }

    return value;
}

// Libère la mémoire associée au dernier noeud de l'arbre donné, s'il n'est pas vide
void delete_last_node(Node **head) {
    Node *element = *head,
         *previous = *head;

    if (element != NULL) {
        // Si l'arbre ne contient qu'un seul élément, libère la racine
        if (element->next == NULL) *head = NULL;
        // Si l'arbre contient plus d'un élément, parcourt l'arbre et libère le dernier élément
        else {
            while (element->next != NULL) {
                previous = element;
                element = element->next;
            }

            previous->next = NULL;
        }

        // Libère également la mémoire associée à tout son contenu
        delete_string(&(element->content));
        delete_nodes(&(element->children));
        free(element);
    }
}

// Libère la totalité de la mémoire associée à l'arbre donné
void delete_nodes(Node **head) {
    while (*head != NULL) delete_last_node(head);
}

// Crée une nouvelle chaîne de caractères
// Ce type est utiliser pour manipuler des sous-chaînes d'adresse et de longueur données, ou plus généralement pour associer une chaîne à sa longueur
String *create_string(char *base, int length) {
    String *new = create_element(sizeof(String));
    new->base = base;
    new->length = length;

    return new;
}

// Compare deux chaînes de caractères de longueurs données, insensiblement à la casse
// Retourne true si elles ont même taille n et que le n caractères des deux chaînes sont égaux
int compare_strings(String *s1, String *s2) {
    int i = 0;

    // Inutile de comparer si les chaînes n'ont pas la même taille
    if (s1->length == s2->length) {
        while (i < s1->length && compare_insensitive(*(s1->base + i), *(s2->base + i))) i++;
    }

    return i == s1->length;
}

// Libère la mémoire associée à la chaîne de caractères donnée
void delete_string(String **head) {
    String *element = *head;

    *head = NULL;
    free(element);
}
