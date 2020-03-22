/*
-- Cas pouvant poser problème
A = 2X
X = *"a"

-- Retourne la longueur du texte trouvé s'il est trouvé
-- Retourne 0 si rien n'est trouvé mais que isok
-- Retourne -1 si rien n'est trouvé mais qu'il aurait dû
*/

typedef struct {
    char *base;
    int length;
} string;

typedef struct {
    // L'un des trois uniquement
    int min_max[2];
    string string;
    int *set;
} rule_params;

typedef struct {
   string name;

   // Soit la fonction, soit la sous-règle
   int (*function)(rule_params, struct rule *, char *, int); 
   rule_params params;
   struct rule *rule;

   struct rule *children;
   struct rule *next;
} rule;

typedef struct {
    // whatever
} node;

/*

o-la = *( a-lo 2*8( b-lo c-lo ) d-lo )
a-lo = "salut" / %d98.45.64 / %x2A-5B
...

name: "o-la"
children: 
    function: &repetition
    params: [0, -1, 0, 0, 0]
    children:
        rule: &a-lo
        next:
            function: &repetition
            params: [2, 8, 0, 0, 0]
            chidlren:
                rule: &b-lo
                next:
                    rule: &c-lo
            next:
                rule: &d-lo
next:
    name: "a-lo"
    children:
        function: &alternation
        children:
            function: &char_val
            params: [0, 0, "salut", 5, 0]
            next:
                function: &dec_val
                params: [0, 0, 0, 0, 98, 45, 64] -- creer le set comme un int[] et le passer en paramètre
                next:
                    function: &hex_val
                    params: [2A, 5B, 0, 0, 0]

    next:...

parse(abnf, chaine):
    http_message ← get_content(get_node(abnf, "http-message"))
    longueur ← exec_rule(http_message, chaine, 0)

exec_rule(regle, chaine, index):
    longueur_totale ← 0
    longueur ← 0

    tant que rule_exists(regle) et longueur != -1 faire
        si has_function(regle) alors
            si has_string_param(rule) alors longueur ← get_function(regle)(get_string_param(regle), get_children(regle), chaine, index)
            sinon si has_set_param(rule) alors longueur ← get_function(regle)(get_set_param(regle), get_children(regle), chaine, index)
            sinon longueur ← get_function(regle)(get_min_max_param(regle), get_children(regle), chaine, index)
        
        sinon longueur ← exec_rule(get_rule(regle), chaine, index)

        longueur_totale ← longueur_totale + longueur
        index ← index + longueur
        regle ← get_next(regle)

    si longueur = -1 alors retourner -1
    retourner longueur_totale

repetition(param, regle, chaine, index):
    min ← param(0)
    max ← param(1)
    longueur_totale ← 0
    longueur ← 1
    i ← 0

    tant que i != max et longueur > 0 faire
        longueur ← exec_rule(regle, chaine, index)
        longueur_totale ← longueur_totale + longueur
        index ← index + longueur
        i ← i + 1

    si longueur = -1 ou si (longueur = 0 et i <= min) alors retourner -1
    retourner longueur_totale

option(param, regle, chaine, index):
    retourner repetition(0, 1, regle, chaine, index)

alternation(param, regle, chaine, index):
    longueur ← 0

    tant que rule_exists(regle) et longueur = 0 faire
        longueur ← exec_rule(regle, chaine, index)
        regle ← get_next(regle)

    si longueur = -1 alors retourner -1
    retourner longueur_totale
*/