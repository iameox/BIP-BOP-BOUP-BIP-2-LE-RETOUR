#include <stdlib.h>
#include "src/functions.h"
#include "src/structures.h"
#include "src/http.h"
#include "api.h"

Rule *rulelist = NULL;
Node *tree = NULL;

void *getRootTree() {
    return tree;
}

_Token *searchTree(void *start, char *name) {
    String rulename = { name, get_length(name) };
    int stop = false,
        i = 0;
    _Token *head = NULL,
           **tail = &head;
    Node *n;

    while (!stop) {
        n = find_node(start, &rulename, &i);

        if (n == NULL) stop = true;
        else {
            *tail = create_element(sizeof(_Token));
            (*tail)->node = n;
            (*tail)->next = NULL;

            tail = &((*tail)->next);
        }

        i++;
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
           *next = element->next;

    while (element != NULL) {
        free(element);

        element = next;
        next = element->next;
    }
}

void purgeTree(void *root) {
    delete_nodes((Node **) &root);
    delete_rulelist(&rulelist);
}
 
#include <stdio.h>
void print_string(String *s) {
    int i;
    for (i = 0; i < s->length; i++) {
        if (is_vchar(*(s->base+i)) || *(s->base+i) == ' ') printf("%c", *(s->base+i));
        else printf("<0x%02X>", (unsigned char) *(s->base+i));
    }

    printf("\n");
}

int parseur(char *req, int len) {
    //String s = {req, len};
    printf("%c\n", *req);
    return 0;//parse_http(&tree, &rulelist, req, len);
}