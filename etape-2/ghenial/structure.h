/*
* Structure utilisée pour traiter les chaines sans sentinelles
* Permet de récupérer un pointeur vers le début de cette chaine et sa taille
*/
typedef struct string {
	char *str;
	int size;
} string;


/*
* Structure décrivant une règle abnf
* string rulename : nom de la règle (case sentitive, bien que ce n'est pas sensé l'être du point de vue de l'ABNF) 
* string expression : le contenu de cette règle
* int is_terminal : 0 pour une règle non terminale, 1 si la règle est terminale
* (terminale : si la règle n'a aucune sous-règles)
* rule_list *elements : pointeur vers une liste de règles désignant les sous règles dont dépendent l'expression
*/
typedef struct abnf_rule {
	string rulename;
	string expression;
	int is_terminal;
	struct rule_list *elements;
} abnf_rule;


/*
* Liste chainée de règles abnf
* abnf_rule *rule : pointeur vers la règle de l'élément actuel
* struct rule_list *next : pointeur vers la règle suivante
*/
typedef struct rule_list {
	abnf_rule *rule;
	struct rule_list *next;
} rule_list;


/*
* La structure des noeuds composant l'arbre de dérivation
* abnf_rule *rule : la règle abnf vérifiée par le noeud
* string value : la chaine parsée par ce noeud
* struct tree_node *next : le noeud suivant, au même niveau de l'arbre (les deux ont le même parent)
* struct tree_node *children : les noeuds fils du noeud
*/
typedef struct tree_node{
	abnf_rule * rule;
	string value;
	struct tree_node *next;
	struct tree_node *children;
} tree_node;

int is_upper_alpha(char c);
int is_lower_alpha(char c);
int is_alpha(char c);
int is_digit(char c);
int is_sp(char c);
int is_htab(char c);

int is_crlf(char c1, char c2);

int is_char(char c, string s);

int is_subchain(string sub, string str);

void insert_rule(rule_list **head, abnf_rule *new_rule);

abnf_rule *create_rule(char *str_name, int name_size, char *str_expression, int expression_size, int is_terminal, rule_list *elements);

abnf_rule *get_subrule(abnf_rule *rule, int start);

tree_node *create_node(abnf_rule *rule, string value);

void print_tree(tree_node **tree);

void add_node(tree_node **tree, tree_node *self_node);

void delete_node(tree_node **tree, tree_node *self_node);

void delete_all_node(tree_node **tree);