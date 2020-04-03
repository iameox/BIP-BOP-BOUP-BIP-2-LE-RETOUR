#include "http.h"
#include "api.h"

void *getRootTree() {

}

_Token *searchTree(void *start,char *name) {

}

char *getElementTag(void *node,int *len) {

}

char *getElementValue(void *node,int *len) {

}

void purgeElement(_Token **r) {

}

void purgeTree(void *root) {
    delete_nodes(&tree);
    delete_rulelist(&rulelist);
}
 
int parseur(char *req, int len) {

}