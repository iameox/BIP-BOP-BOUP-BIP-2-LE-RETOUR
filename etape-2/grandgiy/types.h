typedef struct string string;
typedef struct rule rule;
typedef struct rule_params rule_params;
typedef struct params_set params_set;
typedef struct node node;

struct string {
    char *base;
    int length;
};

struct rule {
   string *name;

   // Soit la fonction, soit la sous-règle
   int (*function)(rule_params *, rule *, char *, int); 
   rule_params *params;
   rule *sub_rule;

   rule *children;
   rule *next;
};

struct rule_params {
    // L'un des trois uniquement
    int min_max[2];
    string *string;
    params_set *set;
};

struct params_set {
    int value;
    params_set *next;
};

struct node {
    // whatever
};