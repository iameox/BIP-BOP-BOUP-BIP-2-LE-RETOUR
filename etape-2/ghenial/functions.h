/*
* Structure utilisée pour traiter les chaines sans sentinelles
* Permet de récupérer un pointeur vers le début de cette chaine et sa taille
*/
typedef struct string {
	char * str;
	int size;
} string;

/*
* Structure décrivant une règle abnf
* rulename : nom de la règle (case sentitive pour le moment) 
* AFAIRE : case insentive
* expression : le contenu de cette règle
* is_terminal : explicite
* elements : pointeur vers liste de règle contenant toutes les sous règles apparaissant dans expression
*/
typedef struct abnf_rule {
	string rulename;
	string expression;
	int is_valid;
	int is_terminal;
	struct rule_list * elements;
} abnf_rule;

/*
* Liste chainée de règles abnf
*/
typedef struct rule_list {
	abnf_rule * rule;
	struct rule_list * next;
} rule_list;


typedef struct tree_node{
	abnf_rule * rule;
	string value;
	struct tree_node * next;
	struct tree_node * children;
} tree_node;

int is_upper_alpha(char c);

int is_lower_alpha(char c);

int is_alpha(char c);

int is_digit(char c);

int is_char(char c, string s);

int is_crlf(char c1, char c2);

int is_sp(char c);

int is_htab(char c);

/*
* Retourne 1 si la chaine sub est présente dans str, 0 sinon
*/
int is_subchain(string sub, string str);

/*
* Insertion en tête d'une règle dans une liste de règles
*/
void insert_rule(rule_list ** head, abnf_rule * new_rule);

/*
* Crée une nouvelle règle à partir des paramètres
*/
abnf_rule * create_rule(char * str_name, int name_size, char* str_expression, int expression_size, int is_terminal, rule_list * elements);

/* DEBUG ASUPPR
* Fonction d'affichage de liste chainée
*/
void print_list(rule_list * head);

/*
* Récupère la sous règle sous forme de structure abnf, en cherchant à l'indice start de l'expression de la règle principale
*/
abnf_rule * get_subrule(abnf_rule * rule, int start);

tree_node * create_node(abnf_rule * rule, string value);

void printntruc(char* str, int size);

void print_tree(tree_node ** tree);

void add_node(tree_node ** tree, tree_node * self_node);

void delete_node(tree_node ** tree, tree_node * self_node);