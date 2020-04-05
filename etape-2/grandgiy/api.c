#include <stdlib.h>
#include "src/functions.h"
#include "src/structures.h"
#include "src/http.h"
#include "api.h"

// Cette variable est globale par nécessité par rapport aux prototypes demandés
Node *tree = NULL;

// Récupère la racine de l'arbre
void *getRootTree() {
    return tree;
}

// Récupère tous les noeuds de l'arbre donné dont le nom correspond à celui demandé
// Les noeuds sont encapsulés dans une liste chaînée d'éléments de type _Token
_Token *searchTree(void *start, char *name) {
    String rulename = { name, get_length(name) };
    _Token *head = NULL,
           // Permet une insertion en queue beaucoup plus rapide
           **tail = &head;
    int stop = false,
        index = 0,
        i;
    Node *n;

    // Convertit les symboles "_" en "-" dans le nom recherché pour relier le parseur et la fonction main
    // Si le premier caractères du nom est un symbole "_", il n'est pas convertit
    // Cela permet de rechercher les noeuds système dont les noms sont "_CHAR-VAL" et "_NUM-VAL"
    while (*name != '\0') {
        name++;
        if (*name == '_') *name = '-';
    }

    while (!stop) {
        i = 0;
        n = find_node(start, &rulename, &i, index);

        // Si plus aucun noeud ne correspond au nom demandé, la recherche s'arrête
        if (n == NULL) stop = true;
        // S'il y a encore de tels noeuds, il faut les insérer à la suite de la liste
        else {
            *tail = create_element(sizeof(_Token));
            (*tail)->node = n;
            (*tail)->next = NULL;

            // Met à jour à l'adresse de la queue de la liste
            tail = &((*tail)->next);
        }

        index++;
    }

    return head;
}

// Récupère le champ "name" d'un noeud de l'arbre
// Si len != NULL, la longueur de ce nom y est écrite
char *getElementTag(void *node, int *len) {
    Node *n = (Node *) node;

    if (len != NULL) *len = n->rulename->length;
    return n->rulename->base;
}

// Récupère le champ "content" d'un noeud de l'arbre
// Si len != NULL, la longueur de ce noeud y est écrite
char *getElementValue(void *node, int *len) {
    Node *n = (Node *) node;

    if (len != NULL) *len = n->content->length;
    return n->content->base;
}

// Libère la mémoire associée à la liste chaînée de résultat d'une recherche dans l'arbre
void purgeElement(_Token **r) {
    _Token *element = *r,
           *next;

    // Ne libère que les éléments de type _Token, l'arbre est toujours disponible
    while (element != NULL) {
        next = element->next;
        free(element);

        element = next;
    }
}

// Libère la totalité de la mémoire encore utilisée : l'arbre, la liste de règles, et le fichier ABNF
void purgeTree(void *root) {
    delete_nodes((Node **) &root);
    delete_rulelist(&rulelist);
    free(abnf_buffer);
}

// Parse une requête HTTP
// Renvoie true si la requête est syntaxiquement valide, false sinon
int parseur(char *req, int len) {
    return parse_http(&tree, req, len);
}
