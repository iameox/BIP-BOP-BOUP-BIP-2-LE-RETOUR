#include <stdlib.h>
#include "functions.h"
#include "structures.h"
#include "abnf.h"

// "Précompile" le fichier ABNF fourni en paramètre selon le paragraphe "4. ABNF Definition of ABNF" de la RFC 5234
// abnf_buffer pointera vers le contenu du fichier et n'est présent que pour pouvoir libérer la mémoire
void compile_abnf(Rule **head, char **abnf_buffer, char *file_name) {
    int index = 0;

    // Insère des règles système permetant d'afficher les noeuds associés aux règles char-val et num-val dans l'arbre
    // Elles sont insérées premier car ce sont les règles qui seront le plus utilisées
    insert_rule(head, create_string(NUM_VAL_RULE, NUM_VAL_RULE_LENGTH));
    insert_rule(head, create_string(CHAR_VAL_RULE, CHAR_VAL_RULE_LENGTH));

    // Lecture du fichier ABNF, son contenu est placé dans abnf_buffer
    read_file(file_name, abnf_buffer);

    // Tente de "précompiler" l'ABNF
    // S'il n'est pas correct, libère la mémoire associée au fichier et ferme le programme
    if (!compile_rulelist(head, *abnf_buffer, &index)) {
        free(*abnf_buffer);
        exit_on_error("Syntaxe ABNF incorrecte ou non supportée.");
    }
}

// Les fonctions suivantes reprennent simplement les règles données dans le paragraphe 4 de la RFC 5234
// Dans toutes les fonctions, index correspond à l'index du caractère courant
// Il est de type int * afin de pouvoir revenir en arrière facilement lors d'une recherche échouée

// rulelist = 1*( rule / (*c-wsp c-nl) )
int compile_rulelist(Rule **head, char *abnf, int *index) {
    int has_rulelist = true,
        is_valid = true,
        i = 0;

    while (is_valid) {
        // Recherche une règle dans le fichier
        // S'il n'en trouve pas, la fin de la recherche repose sur la règle c-nl
        if (!compile_rule(head, abnf, index)) {
            consume_c_wsps(abnf, index, 0);
            is_valid = compile_c_nl(abnf, index);
        }

        // Le tour de boucle n'est compté que si la recherche est un succès
        if (is_valid) i++;
    }

    // Le fichier doit comporter au moins une ligne correcte
    // Si ce n'est pas le cas, libère la mémoire associée à la liste des règles
    if (i < 1) {
        delete_rulelist(head);
        has_rulelist = false;
    }

    return has_rulelist;
}

// rule = rulename defined-as elements c-nl
int compile_rule(Rule **head, char *abnf, int *index) {
    // La règle est inserée dans tous les cas mais elle est supprimée si nécessaire
    Rule *r = insert_rule(head, NULL),
         *r2;
    int previous_index = *index,
        is_redefinition = false,
        has_rule = true;

    // Recherche un nom de règle de règle dans le fichier
    // Si la recherche échoue, la syntaxe est incorrecte
    if (!compile_rulename(abnf, index, &(r->rulename))) has_rule = false;
    else {
        r2 = find_rule(*head, r->rulename);
        is_redefinition = r != r2;

        // Deux règles distinctes ne peuvent pas avoir le même nom
        // Si c'est le cas, soit la seconde ajoute des alternatives à la première, soit la syntaxe est incorrecte
        // Dans tous les cas, nous pouvons effacer la règle qui vient d'être crée et ne considérer que l'autre
        if (is_redefinition) {
            delete_last_rule(head);
            r = r2;
        }

        // Recherche un symbole "=" ou "=/", le contenu de la règle et un c-nl
        // Dans chaque cas, une recherche échouée indique une syntaxe incorrecte
        if (!compile_defined_as(abnf, index, is_redefinition)) has_rule = false;
        else if (!compile_elements(&(r->concatenations), abnf, index)) has_rule = false;
        else has_rule = compile_c_nl(abnf, index);
    }

    // Si la syntaxe est incorrecte, libère la mémoire associée à la règle si nécessaire et replace le pointeur d'index au bon endroit
    if (!has_rule) {
        if (!is_redefinition) delete_last_rule(head);
        *index = previous_index;
    }

    return has_rule;
}

// rulename = ALPHA *(ALPHA / DIGIT / "-")
int compile_rulename(char *abnf, int *index, String **rulename) {
    int has_rulename = true;

    // La chaîne est inserée dans tous les cas mais elle est supprimée si nécessaire
    *rulename = create_string(abnf + *index, 0);

    // Le caractère doit être une lettre
    // Si ce n'est pas le cas, libère la mémoire associée à la chaîne
    if (!is_alpha(*(abnf + *index))) {
        delete_string(rulename);
        has_rulename = false;

    // Si le caractère est une lettre, la syntaxe est correcte
    // Le nom s'arrête alors dès que le prochain caractère n'est pas une lettre, un chiffre ou un "-"
    } else {
        do {
            (*index)++;
            (*rulename)->length++;

        } while (is_alpha(*(abnf + *index)) || is_digit(*(abnf + *index), 10) || *(abnf + *index) == '-');
    }

    return has_rulename;
}

// defined-as = *c-wsp ("=" / "=/") *c-wsp
// is_redefinition indique si une règle du même nom existe déjà
int compile_defined_as(char *abnf, int *index, int is_redefinition) {
    int previous_index = *index,
        has_defined_as = true;

    consume_c_wsps(abnf, index, 0);

    // Si le caractère n'est pas un symbole "=", la syntaxe est incorrecte
    if (*(abnf + *index) != '=') has_defined_as = false;
    // Si le caractère est un symbole "/", c'est un ajout d'alternatives
    // Dans ce cas, la règle doit être une redéfinition d'une regle existante
    // Si ce n'est pas le cas, ou si la règle est une redéfinition mais que ce n'est pas un ajout d'alternatives, la syntaxe est incorrecte
    else {
        (*index)++;

        if (*(abnf + *index) == '/') {
            if (is_redefinition) (*index)++;
            else has_defined_as = false;

        } else if (is_redefinition) has_defined_as = false;
    }

    if (has_defined_as) consume_c_wsps(abnf, index, 0);
    // Si la syntaxe est incorrecte, replace le pointeur d'index au bon endroit
    else *index = previous_index;

    return has_defined_as;
}

// elements = alternation *c-wsp
int compile_elements(Concatenation **head, char *abnf, int *index) {
    // Recherche une série d'alternatives dans le fichier
    // Si la recherche échoue, la syntaxe est incorrecte
    int has_elements = compile_alternation(head, abnf, index);

    if (has_elements) consume_c_wsps(abnf, index, 0);
    return has_elements;
}

// c-wsp = WSP / (c-nl WSP)
// La règle se simplifie c-wsp = [c-nl] WSP et c'est cette version qui est considérée
int compile_c_wsp(char *abnf, int *index) {
    int previous_index = *index,
        has_c_wsp = true;

    // La correcte de la syntaxe repose sur la règle WSP
    compile_c_nl(abnf, index);
    has_c_wsp = is_wsp(*(abnf + *index));

    if (has_c_wsp) (*index)++;
    else *index = previous_index;

    return has_c_wsp;
}

// c-nl = comment / CRLF
int compile_c_nl(char *abnf, int *index) {
    int has_c_nl = true;

    // Recherche un commentaire dans le fichier
    // S'il n'en trouve pas, la validité de la syntaxe repose sur la règle CRLF
    if (!compile_comment(abnf, index)) has_c_nl = consume_crlf(abnf, index);
    return has_c_nl;
}

// comment = ";" *(WSP / VCHAR) CRLF
// Le texte capturé par cette règle est simplement ignoré
int compile_comment(char *abnf, int *index) {
    int previous_index = *index,
        has_comment = true;

    // Si le caractère n'est pas un symbole ";", la syntaxe est incorrecte
    if (*(abnf + *index) != ';') has_comment = false;
    // Si le caractère est un symbole ";", la validité de la syntaxe repose sur la règle CRLF
    else {
        // Capture le contenu du commentaire
        do (*index)++;
        while (is_wsp(*(abnf + *index)) || is_vchar(*(abnf + *index)));

        has_comment = consume_crlf(abnf, index);
    }

    // Si la syntaxe est incorrecte, replace le pointeur d'index au bon endroit
    if (!has_comment) *index = previous_index;
    return has_comment;
}

// alternation = concatenation *(*c-wsp "/" *c-wsp concatenation)
int compile_alternation(Concatenation **head, char *abnf, int *index) {
    int has_alternation = true,
        is_valid = true,
        loop_index;

    // Recherche une concaténation dans le fichier
    if (!compile_concatenation(head, abnf, index)) has_alternation = false;
    // Si la recherche est un succès, la syntaxe est correcte
    // La série d'alternatives s'arrête alors dès que le fichier ne contient pas un symbole "/" suivi d'une concaténation
    else {
        while (is_valid) {
            loop_index = *index;

            consume_c_wsps(abnf, index, 0);
            // Si le caractère n'est pas un symbole "/", la recherche est terminée
            if (*(abnf + *index) != '/') is_valid = false;
            // Si le caractère n'est pas un symbole "/", la fin de la recherche repose sur la règle concatenation
            else {
                (*index)++;
                consume_c_wsps(abnf, index, 0);
                is_valid = compile_concatenation(head, abnf, index);
            }

            // Si la recherche a échoué, replace le pointeur d'index au bon endroit
            if (!is_valid) *index = loop_index;
        }
    }

    return has_alternation;
}

// concatenation = repetition *(1*c-wsp repetition)
int compile_concatenation(Concatenation **head, char *abnf, int *index) {
    // La concaténation est ajoutée dans tous les cas mais elle est supprimée si nécessaire
    Concatenation *c = insert_concatenation(head);
    int has_concatenation = true,
        is_valid = true,
        loop_index;

    // Recherche une répétition dans le fichier
    // Si la recherche échoue, la syntaxe est incorrecte et il faut libèrer la mémoire associée à la concaténation
    if (!compile_repetition(&(c->repetitions), abnf, index))  {
        delete_last_concatenation(head);
        has_concatenation = false;

    } else {
        while (is_valid) {
            loop_index = *index;

            // Il doit y avoir au moins une espace ou une tabulation entre chaque repetition
            // Si ce n'est pas le cas, la recherche est teminrée
            if (!consume_c_wsps(abnf, index, 1)) is_valid = false;
            // S'il y a au moins une espace ou une tabulation, la fin de la recheche repose sur la règle repetition
            else is_valid = compile_repetition(&(c->repetitions), abnf, index);

            // Si la recherche a échoué, replace le pointeur d'index au bon endroit
            if (!is_valid) *index = loop_index;
        }
    }

    return has_concatenation;
}

// repetition = [repeat] element
int compile_repetition(Repetition **head, char *abnf, int *index) {
    // La répétition est ajoutée dans tous les cas mais elle est supprimée si nécessaire
    Repetition *r = insert_repetition(head);
    int has_repetition = true;

    compile_repeat(abnf, index, &(r->min), &(r->max));
    // Recherche une élément dans le fichier
    // Si la recherche échoue, la syntaxe est incorrecte et il faut libèrer la mémoire associée à la répétition
    if (!compile_element(abnf, index, r)) {
        delete_last_repetition(head);
        has_repetition = false;
    }

    return has_repetition;
}

// repeat = 1*DIGIT / (*DIGIT "*" *DIGIT)
int compile_repeat(char *abnf, int *index, int *min, int *max) {
    int has_repeat = true,
        // Capture le nombre minimal de répétitions et le place dans la structure
        is_consumed = consume_number(abnf, index, 10, min, 1);

    // Si le caractère n'est pas un symbole "*", c'est une répétition fixe
    // Dans ce cas il faut avoir capturé au moins un chiffre pour le nombre minimal de répétitions
    // Si ce n'est pas le cas, la syntaxe est incorrecte
    if (*(abnf + *index) != '*') {
        if (!is_consumed) {
            has_repeat = false;
            *min = 1;
        }

        // Le nombre de répétitions est fixe donc le minimum est égal au maximum
        *max = *min;

    // Si le caractère est un symbole "*", c'est une répétition variable
    } else {
        (*index)++;
        // Capture le nombre maximal de répétitions et le place dans la structure
        // Si rien n'est capturé, la répétition peut être infinie, par convention le nombre maximal de répétitions vaut -1 
        if (!consume_number(abnf, index, 10, max, 1)) *max = -1;
    }

    return has_repeat;
}

// element = rulename / group / option / char-val / num-val
// prose-val n'est pas supporté par ce parseur
int compile_element(char *abnf, int *index, Repetition *r) {
    int has_element = true;

    // Recherche un nom de règle, un groupe ou une option, une chaîne de caractères, ou une valeur numérique
    // Dans chaque cas, une recherche effectuée avec succès positionne correctement le type de la répétition
    if (compile_rulename(abnf, index, &(r->content.rulename))) r->type = RULENAME;
    else if (compile_group_or_option(&(r->content.concatenations), abnf, index, HAS_GROUP)) r->type = GROUP;
    else if (compile_group_or_option(&(r->content.concatenations), abnf, index, HAS_OPTION)) {
        r->type = OPTION;
        // Une option peut être capturée 0 ou une fois
        r->min = 0;
        r->max = 1;

    } else if (compile_char_val(abnf, index, &(r->content.char_val))) r->type = CHAR_VAL;
    else if (compile_num_val(abnf, index, &(r->content.num_val))) r->type = NUM_VAL;
    // Si toutes les recherches échouent, la syntaxe est incorrecte
    else has_element = false;

    return has_element;
}

// group = "(" *c-wsp alternation *c-wsp ")"
// option = "[" *c-wsp alternation *c-wsp "]"
// Les règles sont quasiment identiques et sont donc traitées très similairement
// type permet d'indiquer si la recherche cible un groupe ou une option
int compile_group_or_option(Concatenation **head, char *abnf, int *index, Has_group_or_option_type type) {
    char opening_char = type == HAS_GROUP ? '(' : '[',
         closing_char = type == HAS_GROUP ? ')' : ']';
    int previous_index = *index,
        has_group_or_option = true;

    // Si le caractère n'est pas un symbole "(" pour un groupe ou "[" pour une option, la syntaxe est incorrecte
    if (*(abnf + *index) != opening_char) has_group_or_option = false;
    else {
        (*index)++;

        consume_c_wsps(abnf, index, 0);
        // Recherche une série d'alternatives dans le fichier
        // Si la recherche échoue, la syntaxe est incorrecte
        if (!compile_alternation(head, abnf, index)) has_group_or_option = false;
        else {
            consume_c_wsps(abnf, index, 0);
            // Si le caractère n'est pas un symbole ")" pour un groupe ou "]" pour une option, la syntaxe est incorrecte
            if (*(abnf + *index) != closing_char) has_group_or_option = false;
            else (*index)++;
        }
    }

    // Si la syntaxe est incorrecte, replace le pointeur d'index au bon endroit
    if (!has_group_or_option) *index = previous_index;
    return has_group_or_option;
}

// char-val = DQUOTE *(%x20-21 / %x23-7E) DQUOTE
int compile_char_val(char *abnf, int *index, String **char_val) {
    int previous_index = *index,
        has_char_val = true;

    // La chaîne est inserée dans tous les cas mais elle est supprimée si nécessaire
    *char_val = create_string(abnf + *index + 1, 0);


    if (*(abnf + *index) != '"') has_char_val = false;
    // Si le caractère est un symbole '"', capture le contenu de la chaîne de caractère
    // La chaîne s'arrête alors dès que le caractère n'est pas un caractère visuel à part '"', ou une espace
    else {
        (*index)++;

        while ((is_vchar(*(abnf + *index)) && *(abnf + *index) != '"') || *(abnf + *index) == ' ') {
            (*index)++;
            (*char_val)->length++;
        }

        // Si le caractère n'est pas un symbole '"', la syntaxe est incorrecte
        if (*(abnf + *index) != '"') has_char_val = false;
        else (*index)++;
    }

    // Si la syntaxe est incorrecte, libère la mémoire associée à la chaîne et replace le pointeur d'index au bon endroit
    if (!has_char_val) {
        delete_string(char_val);
        *index = previous_index;
    }

    return has_char_val;
}

// num-val = "%" (bin-val / dec-val / hex-val)
int compile_num_val(char *abnf, int *index, Num_val **num_val) {
    int bases[3] = { 2, 10, 16 },
        has_num_val = false,
        i = 0;

    // La valeur numérique est inserée dans tous les cas mais elle est supprimée si nécessaire
    *num_val = create_num_val();

    // Si le caractère est un symbole "%", la validité de la syntaxe repose sur les règles bin-val, dec-val et hex-val
    if (*(abnf + *index) == '%') {
        (*index)++;

        // Recherche successivement un nombre en binaire, décimal puis hexadécimal jusqu'à ce qu'une recherche soit un succès
        // Si toutes les recherches échouent, la syntaxe est incorrecte
        while (i < 3 && !has_num_val) {
            has_num_val = compile_bin_or_dec_or_hex_val(abnf, index, *num_val, bases[i]);
            i++;
        }
    }

    // Si la syntaxe est incorrecte, libère la mémoire associée à la valeur numérique
    if (!has_num_val) delete_num_val(num_val);
    return has_num_val;
}

// bin-val = "b" 1*BIT [ 1*("." 1*BIT) / ("-" 1*BIT) ]
// dec-val = "d" 1*DIGIT [ 1*("." 1*DIGIT) / ("-" 1*DIGIT) ]
// hex-val = "x" 1*HEXDIG [ 1*("." 1*HEXDIG) / ("-" 1*HEXDIG) ]
// Les règles sont quasiment identiques et sont donc traitées très similairement
int compile_bin_or_dec_or_hex_val(char *abnf, int *index, Num_val *num_val, int base) {
    char base_indicator;
    int previous_index = *index,
        has_bin_or_dec_or_hex_val = true,
        loop = true,
        value;

    if (base == 2) base_indicator = 'b';
    else if (base == 10) base_indicator = 'd';
    else if (base == 16) base_indicator = 'x';

    // Si le caractère n'est pas un symbole "b" pour un valeur binaire, "d" pour un valeur décimale ou "x" pour une valeur hexadécimale, la syntaxe est incorrecte
    if (!compare_insensitive(*(abnf + *index), base_indicator)) has_bin_or_dec_or_hex_val = false;
    else {
        (*index)++;

        // Au moins un chiffre doit être capturé
        // Si ce n'est pas le cas, la syntaxe est incorrecte
        if (!consume_number(abnf, index, base, &value, 1)) has_bin_or_dec_or_hex_val = false;
        // Si au moins un chiffre est capturé, la syntaxe est correcte
        // Dans ce cas il faut rechercher soit un symbole "-" suivie d'un nombre, soit une série de nombres précédés d'un symbole "."
        
        // Si le caractère est un symbole "-", c'est un intervalle de valeurs possibles
        // La valeur capturée précédémment est le minimum de l'intervalle
        else if (*(abnf + *index) == '-') {
            num_val->value.min_max[0] = value;
            num_val->type = MIN_MAX;
            (*index)++;

            // Capture la valeur maximale de l'intervalle
            // Si rien n'est capturé, replace le pointeur d'index sur le symbole "-"
            if (!consume_number(abnf, index, base, num_val->value.min_max + 1, 1)) (*index)--;

        // Si le caractère n'est pas un symbole "-", c'est une suite de valeurs
        // Dans ce cas la recherche s'arrête dès que le caractère n'est pas un symbole "." ou qu'aucun nombre n'est capturé
        // La valeur capturée précédemment est la première de la suite
        } else {
            num_val->type = SET;
            insert_set_value(&(num_val->value.set), value);

            while (*(abnf + *index) == '.' && loop) {
                (*index)++;

                // Capture la prochaine valeur de la suite
                // Si rien n'est capturé, replace le pointeur d'index sur le symbole "." et la recherche s'arrête
                if (consume_number(abnf, index, base, &value, 1)) insert_set_value(&(num_val->value.set), value);
                else {
                    loop = false;
                    (*index)--;
                }
            }
        }
    }

    // Si la syntaxe est incorrecte, replace le pointeur d'index au bon endroit
    if (!has_bin_or_dec_or_hex_val) *index = previous_index;
    return has_bin_or_dec_or_hex_val;
}

// Capture une série d'espaces et de tabulations
// Retourne true si le nombre minimum d'espaces demandé a été atteint, false sinon
int consume_c_wsps(char *abnf, int *index, int min) {
    int i = 0;

    // S'arrête dès que le caractère n'est pas une espace ou une tabulation
    while (compile_c_wsp(abnf, index)) i++;
    return i >= min;
}

// Capture un retour chariot suivi d'une fin de ligne et déplace le pointeur d'index
// Retourne true si la capture est un succès, false sinon
int consume_crlf(char *abnf, int *index) {
    int is_consumed = is_crlf(abnf + *index);

    if (is_consumed) (*index) += 2;
    return is_consumed;
}

// Capture une série de chiffres dans la base demandée et convertit le texte lu en un nombre en base 10
// Retourne true si le nombre minimum de chiffres demandé a été atteint, false sinon
int consume_number(char *abnf, int *index, int base, int *value, int min) {
    int i = 0;
    *value = 0;

    // S'arrête dès que le caractère n'est pas un chiffre dans la base demandée
    while (is_digit(*(abnf + *index), base)) {
        *value = *value * base + char_to_digit(*(abnf + *index), base);
        (*index)++;

        i++;
    }

    return i >= min;
}
