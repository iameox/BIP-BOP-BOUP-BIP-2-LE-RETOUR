#ifndef __TREE_H__
#define __TREE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "compare.h"
#include "parseur.h"
#include "tree.h"
#include "api.h"


void addToken(_Token **token_list, _Token *node_to_add);


void print(tree_node *mytree);

void purgeNode(tree_node *node);

void delete_all_nodes(tree_node **tree);


#endif