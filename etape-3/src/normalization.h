typedef struct int_stack {
    int top;
    struct int_stack *next; 
} int_stack;

// Normalise le champ request-target d'une URI
// Décode tous les "percent-encodings" et les "dot segments"
void normalize_request_target(string *request_target);

// Normalise une URI du point de vue des "percent encodings" (décode toutes les occurences)
void normalize_percent(string *str);

// Normalise une URI du point de vue des "dot segments"
// Applique l'algorithme de la partie 5.2.4 de la RFC 3986
void remove_dot_segments(string *str);

// Empile top au sommet de stack
void push_stack(int_stack **stack, int top);

// Dépile et renvoie la tête de stack si elle existe, 0 sinon
int pop_stack(int_stack **stack);

// Libère la mémoire associée à la totalité de stack
void clear_stack(int_stack **stack);