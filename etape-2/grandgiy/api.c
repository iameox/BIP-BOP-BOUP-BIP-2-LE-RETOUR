#include <stdlib.h>
#include "src/functions.h"
#include "src/structures.h"
#include "src/http.h"
#include "api.h"

Node *tree = NULL;

void *getRootTree() {
    return tree;
}

_Token *searchTree(void *start, char *name) {
    String rulename = { name, get_length(name) };
    _Token **tail = NULL;
    int stop = false,
        index = 0,
        i;
    Node *n;

    while (*name != '\0') { 
        name++;
        if (*name == '_') *name = '-';
    }

    while (!stop) {
        i = 0;
        n = find_node(start, &rulename, &i, index);

        if (n == NULL) stop = true;
        else {
            *tail = create_element(sizeof(_Token));
            (*tail)->node = n;
            (*tail)->next = NULL;

            tail = &((*tail)->next);
        }

        index++;
    }

    return head;
}

char *getElementTag(void *node, int *len) {
    Node *n = (Node *) node;

    if (len != NULL) *len = n->rulename->length;
    return n->rulename->base;
}

char *getElementValue(void *node, int *len) {
    Node *n = (Node *) node;

    if (len != NULL) *len = n->content->length;
    return n->content->base;
}

void purgeElement(_Token **r) {
    _Token *element = *r,
           *next;

    while (element != NULL) {
        next = element->next;
        free(element);

        element = next;
    }
}

void purgeTree(void *root) {
    delete_nodes((Node **) &root);
    delete_rulelist(&rulelist);
    free(abnf_buffer);
}

int parseur(char *req, int len) {
    return parse_http(&tree, req, len);
}
