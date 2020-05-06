typedef struct int_stack {
    int top;
    struct int_stack *next; 
} int_stack;

void normalize_request_target(char *request_target, int *length);
void normalize_percent(char *string, int *length);

// Normalise une URI du point de vue des "dot segments"
// Applique l'algorithme de la partie 5.2.4 de la RFC 3986
void remove_dot_segments(char *string, int *length);

// Empile top au sommet de stack
void push_stack(int_stack **stack, int top);

// Dépile et renvoie la tête de stack si elle existe, 0 sinon
int pop_stack(int_stack **stack);

// Libère la mémoire associée à la totalité de stack
void clear_stack(int_stack **stack);