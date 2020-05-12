#define true 1
#define false 0

//A changer probablement en /var/www/html/
#define ROOT "./"

#define KNOWN_METHODS {"GET", "HEAD", "POST"}
#define METHODS_NUMBER 3

typedef struct {
    char *base;
    int length;
} string;

// Renvoie true si c est compris entre c1 et c2 (inclus), false sinon
// La comparaison est non-signée pour pouvoir traiter toutes les valeurs possibles
int is_between(unsigned char c, unsigned char c1, unsigned char c2);

// Convertit une chaîne contenant un chiffre en nombre
// Si le caractère n'est pas un chiffre, ne fait rien
int char_to_int(char c);

// Convertit une lettre majuscule en lettre minuscule
// Si le caractère n'est pas une lettre majuscule, ne fait rien
char to_lowercase(char c);

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(string *s1, char *s2);

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(string *s, char *p, int *len);