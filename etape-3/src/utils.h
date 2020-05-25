#define true 1
#define false 0

//Constante pour prévenir l'overflow des entiers
#define MAX_DIGITS 18

typedef struct {
    char *base;
    int length;
} string;

typedef struct int_stack {
    int top;
    struct int_stack *next; 
} int_stack;

// Renvoie true si n est compris entre n1 et n2 (inclus), false sinon
int is_between_int(int n, int n1, int n2);

// Renvoie true si c est compris entre c1 et c2 (inclus), false sinon
// La comparaison est non-signée pour pouvoir traiter toutes les valeurs possibles
int is_between(unsigned char c, unsigned char c1, unsigned char c2);

// Convertit une chaîne contenant un chiffre en nombre
// Si le caractère n'est pas un chiffre, ne fait rien
int char_to_int(char c);

// Convertit une chaine de caractères en entier
// Si un caractère qui n'est pas un chiffre est rencontré, la fonction retourne -1
// Sinon s'il y a plus de MAX_DIGITS chiffres, le nombre est tronqué
// Sinon, retourne la valeur de la chaine associée
int string_to_int(string *s);

// Convertit un entier en une chaine de caractères
// Si length != null, la longueur de la chaîne lui est affectée
// Sinon s'il y a plus de MAX_DIGITS chiffres, le nombre est tronqué
char *int_to_string(long int n, int *length);

// Effectue une recopie de mémoire de la chaine s1 vers s2
void copy_to_string(const char *s1, string *s2);

// Convertit une lettre majuscule en lettre minuscule
// Si le caractère n'est pas une lettre majuscule, ne fait rien
char to_lowercase(char c);

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(string *s1, char *s2);

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(string *s, char *p, int *len);

// Empile top au sommet de stack
void push_stack(int_stack **stack, int top);

// Dépile et renvoie la tête de stack si elle existe, 0 sinon
int pop_stack(int_stack **stack);

// Libère la mémoire associée à la totalité de stack
void clear_stack(int_stack **stack);