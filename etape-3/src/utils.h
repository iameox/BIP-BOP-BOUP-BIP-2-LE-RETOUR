#define true 1
#define false 0

#define min(a,b) a<b?a:b

//A changer probablement en /var/www/html/
#define ROOT "./"

#define KNOWN_METHODS {"GET", "HEAD", "POST"}
#define METHODS_NUMBER 3

// Renvoie true si c est compris entre c1 et c2 (inclus), false sinon
// La comparaison est non-signée pour pouvoir traiter toutes les valeurs possibles
int is_between(unsigned char c, unsigned char c1, unsigned char c2);

// Convertit une lettre majuscule en lettre minuscule
// Si le caractère n'est pas une lettre majuscule, ne fait rien
char to_lowercase(char c);

// Retourne true si s1 et s2 contiennent les mêmes caractères, false sinon
int compare_strings(char *s1, char *s2, int len);

// Retourne true si p est un préfixe de s, false sinon
// Si len != NULL, sa valeur devient la longueur de p, si c'est un préfixe de s
int match_prefix(char *s, char *p, int *len);