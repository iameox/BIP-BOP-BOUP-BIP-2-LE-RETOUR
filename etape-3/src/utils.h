#define true 1
#define false 0

//A changer probablement en /var/www/html/
#define ROOT "./"

#define KNOWN_METHODS {"GET", "HEAD", "POST"}
#define METHODS_NUMBER 3

//Constante pour prévenir l'overflow des entiers
#define MAX_DIGITS 18

typedef struct {
    char *base;
    int length;
} string;

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
char *int_to_string(long int n, int *length)

// Convertit une lettre majuscule en lettre minuscule
// Si le caractère n'est pas une lettre majuscule, ne fait rien
char to_lowercase(char c);

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(string *s1, char *s2);

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(string *s, char *p, int *len);
