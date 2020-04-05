#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "utils.h"
#include "compare.h"
#include "parseur.h"
#include "tree.h"


// Son absolute-path est au moins un ('/' + segment) (here + means concatenation)
int is_absolute_path(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "absolute_path\0";
    tree->adress = &is_absolute_path;
    tree->children = children;
    tree->children->next_node = children_next;

    //printf("\n\n\n\n\n Absolute-path : %s de taille %lu\n\n ",characters,strlen(characters));
    int to_return = 0;
    int to_return_tmp = 1;
    int index_slash = 0;
    int index_last_slash = 0;
    int index = 1;
    int index_reset;
    int index_segment;
    int is_query_ok;
    char segment[strlen(characters)];
    //printf("\n %d \n\n",(segment[0]=='\0'));

    while(characters[index_slash] == '/') /* assure que le premier caractère ne soit pas directement '\0' */
    {
        for(index_reset = 0; index_reset < strlen(segment); index_reset++) segment[index_reset] = '\0';
        index_segment = 0;

        //printf("\n Comparaison du segment suivant l'indice %d \n\n",index_slash);
        while(characters[index] != '\0' && characters[index] != '/' && characters[index] != '?')
        {
            segment[index_segment] = characters[index];
            index_segment++;
            index++;
        }
        segment[index_segment] = '\0';
        //printf("\n\n\n\n\n\n\n\n  SEGMENT ANALYSÉ : %s. \n\n\n\n\n\n\n\n",segment);
        //if (strlen(segment) == 0) printf("Index %d : %c => %d %d %d \n\n\n\n",index,characters[index],characters[index] == '\0', characters[index] == '/', characters[index] == '?');

        if(characters[index] == '\0')
        {
            //printf("\n\n Comparaison dans le if du segment %s7\n\n",segment);
            /* Si le absolute-path n'est qu'un / ou se termina par un /, il est valide */
            if (segment[0] == '\0')
            {
                to_return = 1;
            }
            /* Sinon on regarde si le segment entre les / est valide */
            else
            {
                to_return_tmp *= is_segment(segment,tree->children);
                to_return = to_return_tmp;
            }
            //if (to_return == 0) printf("\n Segment invalide dans le if. \n\n");
            //else printf("\n Segment valide dans le if. \n\n");
        }
        else if (characters[index] != '?')
        {
            //printf("\n\n Comparaison dans le else du segment %s.\n\n",segment);
            to_return_tmp *= is_segment(segment,tree->children);
            to_return = to_return_tmp;
            //if (to_return == 0) printf("\n Segment invalide dans le else. \n\n");
            //else printf("\n Segment valide dans le else. \n\n");
            index++;
        }
        else /* S'il y a une query */
        {
            to_return_tmp *= is_segment(segment,tree->children);
            to_return = to_return_tmp;
            //if (to_return == 0) printf("\n Segment invalide dans le else. \n\n");
            //else printf("\n Segment valide dans le else. \n\n");

            index++;
            index_last_slash = index_slash;
            //printf("\n\n\n\n\n\n\n QUERY DETECTED \n\n Index_last_slash : %d \n Index : %d \n\n",index_last_slash,index);
            if (characters[index-1] == '?') is_query_ok = is_query(characters + index,tree->children->next_node);
            else is_query_ok = is_query(characters + index_last_slash + 2,tree);

            //if (is_query_ok != 0 && characters[index-1] == '?') printf("\n Query %s valide \n\n",(characters + index));
            //else if (is_query_ok != 0) printf("\n Query %s valide \n\n",(characters + index_last_slash + 2));
            //else
            //{
            //    if (characters[index-1] == '?') printf("\n Is_query %s : %d \n\n",(characters + index),is_query_ok);
            //    else printf("\n Is_query %s : %d \n\n",(characters + index_last_slash + 2),is_query_ok);
            //}
            to_return *= is_query_ok;
        }
        index_last_slash = index_slash;
        index_slash = index_slash + index - 1;
        /* Si le segment n'est que /, on sort volontairement de la boucle infinie */
        if (index == 1) index_slash++;
        //printf("\n Index last slash : %d \n Index slash : %d \n Index : %d \n\n",index_last_slash,index_slash,index);
    }

    //if (to_return == 0) printf("\n %s n'est pas un absolute-path valide. \n\n",characters);
    //else printf("\n %s est un absolute path valide!! \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_alpha(char character, tree_node *tree)
{
    int to_return = ((0x41 <= character && character <= 0x5A) || (0x61 <= character && character <= 0x7A));

    if (tree != NULL)
    {
    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    }
    return to_return;
}

int is_bit(char character, tree_node *tree)
{
    int to_return = (character == 0x30 || character == 0x31);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_bws(char *characters, tree_node *tree)
{
    return is_ows(characters,tree);
}

int is_char(char character, tree_node *tree)
{
    int to_return = (0x01 <= character && character <= 0x7F);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_cr(char character, tree_node *tree)
{
    int to_return = (character == 0x0D);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_crlf(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "crlf\0";
    tree->adress = &is_crlf;
    tree->children = children;
    tree->children->next_node = children_next;

    int to_return = 0;
    if(strlen(characters) == 2)
    {
        to_return = (is_cr(characters[0],tree->children) && is_lf(characters[1],tree->children->next_node));
    }
    //else printf("\nTaille Invalide (taille attendue : 2, taille réelle : %lu)\n",strlen(characters));
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_ctl(char character, tree_node *tree)
{
    int to_return = ((0x00 <= character && character <= 0x1F) || (character == 0x7F));

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

/* dec-octet = "25" %x30-35 / "2" %x30-34 DIGIT / "1" 2 DIGIT / %x31-39 DIGIT / DIGIT */
/* DIGIT = %x30-39 ; 0-9 */
int is_dec_octet(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree->name = "dec_octet\0";
    tree->adress = &is_dec_octet;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;

    int to_return = 0;

    if (strlen(characters) == 1)
    {
        to_return = is_digit(characters[0],tree->children);
    }
    else if (strlen(characters) == 2)
    {
        to_return = (0x31 <= characters[0] && characters[0] <= 0x39 && is_digit(characters[1],tree->children->next_node));
    }
    else if (strlen(characters) == 3)
    {
        to_return = ((characters[0]==0x32 && characters[1]==0x35 && 0x30 <= characters[2] && characters[2] <= 0x35) || (characters[0]==0x32 && 0x30 <= characters[1] && characters[1] <= 0x34 && is_digit(characters[2],tree->children->next_node->next_node)) || (characters[0]==0x31 && is_digit(characters[1],tree->children->next_node) && is_digit(characters[2],tree->children->next_node)));
    }

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_digit(char character, tree_node *tree)
{
    int to_return = (0x30 <= character && character <= 0x39);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    return to_return;
}

int is_dquote(char character, tree_node *tree)
{
    int to_return = (character == 0x22);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_header(char *ligne, int taille_ligne, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "header\0";
    tree->adress = &is_header;
    tree->children = children;

	/* On assume que la ligne donnée en paramètre est soit un Host Header, soit un Transfer-Encoding-Header */
	int to_return = 0;
	if (taille_ligne > 0) to_return = (is_host_header(ligne,tree->children) || is_transfer_encoding_header(ligne,tree->children));


    if (to_return)
    {
        tree->content = ligne;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    if (to_return == 0) printf("\n %s n'est pas un header valide. \n\n",ligne);
    return to_return;
}

int is_hexdig(char character, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "hexidg\0";
    tree->adress = &is_hexdig;
    tree->children = children;

    /* 0x41 à 0x46 pour majuscules (un hexdig peut être une lettre de A à F inclues), 0x61 à 0x66 sinon */
    int to_return = ((is_digit(character,tree->children)) || (0x41 <= character && character <= 0x46) || (0x61 <= character && character <= 0x66));

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/*      RAJOUTER L'OPTION [ : port] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*      INCOHÉRENCE MAJEURE DANS LES RFC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*      Les termes définis dans les RFC sont censés être case-insensitive d'après RFC5234 */
/*      MAIS il y a une différence entre un Host et un host car on peut avoir Host = host : port */
int is_host(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));


    tree->name = "host\0";
    tree->adress = &is_host;
    tree->children = children;
    tree->children->next_node = children_next;

    int to_return = 0;

    int is_ip_literal_ok;
    int is_ipvquatre_adress_ok;
    int is_reg_name_ok;

    int index = 0;
    int index_last_deux_points = 0;

    char tmp[strlen(characters) + 1];
    tmp[strlen(characters)] = '\0';

    is_ip_literal_ok = is_ip_literal(characters,tree->children);
    //printf("\n Is_IP_literal_adress ok : %d \n\n",is_ip_literal_ok);
    if (!is_ip_literal_ok) is_ipvquatre_adress_ok = is_ipvquatre_adress(characters,tree->children);
    //printf("\n Is_IPv4_adress ok : %d \n\n",is_ipvquatre_adress_ok);
    if (!is_ip_literal_ok && !is_ipvquatre_adress_ok) is_reg_name_ok = is_reg_name(characters,tree->children);
    //printf("\n Is_reg_name ok : %d \n\n",is_reg_name_ok);

    to_return = (is_ip_literal_ok || is_ipvquatre_adress_ok || is_reg_name_ok);

    /* Si l'on n'a pas reconnu le host, il se peut que se soit dû à l'option port */
    if(to_return == 0)
    {
        while (characters[index] != '\0')
        {
            while(characters[index] != '\0' && characters[index] != 0x3A) index++;
            if (characters[index] == 0x3A)
            {
                index_last_deux_points = index;
                index++;
            }
            //printf("\n On a trouvé 0x3A à l'index %d \n\n",index_last_deux_points);
        }

        for (index = 0; index < index_last_deux_points; index++) tmp[index] = characters[index];
        tmp[index] = '\0';
        is_ip_literal_ok = is_ip_literal(tmp,tree->children);
        //printf("\n Is_IP_literal_adress ok : %d \n\n",is_ip_literal_ok);
        if (!is_ip_literal_ok) is_ipvquatre_adress_ok = is_ipvquatre_adress(tmp,tree->children);
        //printf("\n Is_IPv4_adress ok : %d \n\n",is_ipvquatre_adress_ok);
        if (!is_ip_literal_ok && !is_ipvquatre_adress_ok) is_reg_name_ok = is_reg_name(tmp,tree->children);
        //printf("\n Is_reg_name ok : %d \n\n",is_reg_name_ok);

        //printf("\n Let's try again ? %d !! \n\n",(is_ip_literal_ok || is_ipvquatre_adress_ok || is_reg_name_ok));

        characters[strlen(characters)-1] = '\0';
        to_return = ((is_ip_literal_ok || is_ipvquatre_adress_ok || is_reg_name_ok) && is_port((characters + index_last_deux_points + 1),tree->children->next_node));
    }

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    if (to_return == 0) printf("\n %s n'est pas un host valide. \n\n",characters);
    else printf("\n %s est un host valide. \n\n",characters);
    return to_return;
}

/* Dans cette implémentation, on ne se sert pas de la fonction is_ows(char *characters)  -_-'  */
int is_host_header(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "host_header\0";
    tree->adress = &is_host_header;
    tree->children = children;

    int to_return = 0;
    int is_end_ok = 0;
    int is_host_ok = 0;

    int index;
    int index_host_beginning = 6;
    int index_host_ending = 6;

    //printf("\n Regardons si %s de taille %lu est un Host Header valide. \n\n",characters,strlen(characters));

    /* Il faut que le host header commence au minimum par Host: */
    if (strlen(characters) >= 5 && (characters[0] == 0x48 || characters[0] == 0x68) && (characters[1] == 0x6F || characters[1] == 0x4F) && (characters[2] == 0x73 || characters[2] == 0x53) && (characters[3] == 0x74 || characters[3] == 0x54) && characters[4] == 0x3A)
    {
        //printf("\n Début d'en-tête correcte. \n\n");
        while (is_sp(characters[index_host_beginning],tree) || is_htab(characters[index_host_beginning],tree))
        {
            index_host_beginning++;
            index_host_ending++;
        }
        if (characters[index_host_beginning] != '\0') index_host_ending++;
        /* Tant que l'on ne rencontre ni le caractère sentinelle, ni un espace, ni un htab, on avance */
        while (characters[index_host_ending] != '\0' && (!is_sp(characters[index_host_ending],tree) || !is_htab(characters[index_host_ending],tree))) index_host_ending++;
    
        index = index_host_ending;
        while (is_sp(characters[index],tree) || is_htab(characters[index],tree)) index++;
        if (characters[index] == '\0') is_end_ok = 1;

        /* D'après le deuxième if (entre les deux while), la différence suivante est au moins égale à 1 */
        char tmp[index_host_ending - index_host_beginning + 1];
        tmp[index_host_ending - index_host_beginning] = '\0';
        /* index peut aller jusqu'à index_host_ending - index_host_opening - 1 */
        for (index = index_host_beginning; index < index_host_ending; index++) tmp[index - index_host_beginning] = characters[index];

        //printf("\n Appel à is_host avec la chaine %s. \n\n",tmp);
        is_host_ok = is_host(tmp,tree->children);
        to_return = (is_host_ok && is_end_ok);
        //if (is_host_ok == 0) printf("\n %s n'est pas un Host valide. \n\n",tmp);
        //else if (is_end_ok == 0) printf("\n Fin invalide. \n\n");
    }
    //else printf("\n %d %d %d %d %d %d \n\n\n\n", strlen(characters) >= 5, characters[0] == 0x48, characters[1] == 0x6F, characters[2] == 0x73, characters[3] == 0x74, characters[4] == 0x3A);

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    if (to_return == 0) printf("\n %s n'est pas un host header valide. \n\n",characters);
    return to_return;
}


/* h16 = 1*4 HEXDIG */
int is_hseize(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next_next;
    children_next_next_next = malloc(sizeof(tree_node));
    children_next_next_next->name = malloc(50*sizeof(char));
    children_next_next_next->name = "HTTP_message\0";
    children_next_next_next->adress = &is_http_message;
    children_next_next_next->content = malloc(500*sizeof(char));
    children_next_next_next->is_correct = 0;
    children_next_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next_next->children = malloc(sizeof(tree_node));

    tree->name = "hseize\0";
    tree->adress = &is_hseize;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;
    tree->children->next_node->next_node->next_node = children_next_next_next;

    int to_return = 0;
    int to_return_tmp = 1;
    int index;
    if(1 <= strlen(characters) && strlen(characters) <= 4)
    {
        for (index = 0; index < strlen(characters); index++)
        {
            if (index == 0) to_return_tmp *= is_hexdig(characters[index],tree->children);
            if (index == 1) to_return_tmp *= is_hexdig(characters[index],tree->children->next_node);
            if (index == 2) to_return_tmp *= is_hexdig(characters[index],tree->children->next_node->next_node);
            if (index == 3) to_return_tmp *= is_hexdig(characters[index],tree->children->next_node->next_node->next_node);
            //if (! is_hexdig(characters[index])) printf("\n %c n'est pas un hexdig valide.\n\n",characters[index]);
        }
        to_return = to_return_tmp;
    }

    //if (to_return == 0) printf("\n %s n'est pas un hseize valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_htab(char character, tree_node *tree)
{
    return (character == 0x09);
}



/*
        A FAIRE !!!!!!!!!

        1) Libérer la mémoire dans le cas où is_correct vaut 0

        2) Rajouter des caractères sentinelles à la fin des content dans le cas où is_correct vaut 1
            Ceci peut se faire avec strlen(characters) comme indice 
*/



/*
0) Je crée un fils et tous les suivants du fils pour le noeud courant

1) Je fais le lien entre le fils et ses suivants

2) Je fais l'appel de fonction avec le fils ou l'un de ses suivants
    - Un nouveau fils et de nouveaux suivants sont créés
    - Le fils appelant est rempli par la chaine de caractères si méthode valide, par NULL sinon

3)  Si l'appel de fonction retourne une méthode correcte, je remplis mon noeud actuel
    par la chaine de caractères qui a validé la condition
    Sinon je mets le contenu de mon noeud à NULL
*/

int is_http_message_preparsage(char *req, tree_node *tree)
{
    tree = malloc(sizeof(tree_node));
    tree->name = malloc(50*sizeof(char));
    tree->name = "HTTP_message\0";
    tree->adress = &is_http_message;
    tree->content = malloc(500*sizeof(char));
    tree->is_correct = 0;
    tree->next_node = malloc(sizeof(tree_node));
    tree->children = malloc(sizeof(tree_node));

    int to_return = is_http_message(req,tree);

    //if (tree != NULL) print(tree);
    printf("\n Fichier correctement parsé : %d \n\n",to_return);
    if (to_return != 0) printf("\n FOUND \n\n");

    //purgeTree(&tree);

    return to_return;
}



int is_http_message(char *requete, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next_next;
    children_next_next_next = malloc(sizeof(tree_node));
    children_next_next_next->name = malloc(50*sizeof(char));
    children_next_next_next->name = "HTTP_message\0";
    children_next_next_next->adress = &is_http_message;
    children_next_next_next->content = malloc(500*sizeof(char));
    children_next_next_next->is_correct = 0;
    children_next_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next_next->children = malloc(sizeof(tree_node));
    

    tree->name = "HTTP_message\0";
    tree->adress = &is_http_message;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;
    tree->children->next_node->next_node->next_node = children_next_next_next;

    /*
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;
    tree->children->next_node->next_node->next_node = children_next_next_next;
    */

    int to_return = 1;

    int nombre_de_lignes = compter_nombre_de_lignes(requete);
    int index_ligne = 1;

	int *tailles_des_lignes;
	tailles_des_lignes = malloc(2 + nombre_de_lignes*sizeof(int));

	char *lignes[nombre_de_lignes];

	int index;
	for(index = 0; index < nombre_de_lignes; index++)
	{
		lignes[index] = malloc(sizeof(char*));
	}

	tailles_lignes(requete,lignes,nombre_de_lignes,tailles_des_lignes);

    int *tailles_des_mots[nombre_de_lignes];
	char *mots[nombre_de_lignes][500];

	tailles_mots(lignes, nombre_de_lignes, tailles_des_mots, mots);

    //printf("\n Ligne 0 : %s de taille %d (%lu)\n\n",lignes[0],tailles_des_lignes[0],strlen(lignes[0]));
    to_return *= is_start_line(lignes[0], tailles_des_lignes[0], mots[0], tailles_des_mots[0],tree->children);
 
    if (tailles_des_lignes[index_ligne] == 0 && tailles_des_lignes[index_ligne+1] != 0)
    {
        index_ligne++;
        //printf("\n Ligne 1 : %s de taille %d (%lu) \n\n",lignes[index_ligne],tailles_des_lignes[index_ligne],strlen(lignes[index_ligne]));
        if (to_return != 0) to_return *= is_header(lignes[index_ligne], tailles_des_lignes[index_ligne],tree->children->next_node);
        index_ligne++;
    }

    if (tailles_des_lignes[index_ligne] == 0 && tailles_des_lignes[index_ligne+1] != 0)
    {
        index_ligne++;
        //printf("\n Ligne 2 : %s de taille %d (%lu) \n\n",lignes[index_ligne],tailles_des_lignes[index_ligne],strlen(lignes[index_ligne]));
        if (to_return != 0) to_return *= is_header(lignes[index_ligne], tailles_des_lignes[index_ligne],tree->children->next_node->next_node);
        index_ligne++;
    }

	if(to_return != 0 && tailles_des_lignes[index_ligne] == 0 && tailles_des_lignes[index_ligne+1] != 0) to_return *= is_message_body(lignes[index_ligne],tree->children->next_node->next_node->next_node);
    

    /*
    free(tailles_des_lignes);
    for(index = 0; index < nombre_de_lignes; index++)
	{
	    free(lignes[index]);
	}
    */
    
    if (to_return)
    {
        tree->content = requete;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    if (to_return == 0) printf("\n %s n'est pas un http-message valide. \n\n",requete);

    //if (tree != NULL) print(tree);

    return to_return;
}










/* Les quatre premiers caractères doivent être "HTTP"
   Le caractère suivant doit être un '/'
   Le caractère suivant doit être un DIGIT
   Le caractère suivant doit être un '.'
   Le caractère suivant doit être un DIGIT
*/
int is_http_version(char *characters, tree_node *tree)
{
    //printf("\n Regardons si %s est une HTTP-version valide ou non. \n\n",characters);

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "http_version\0";
    tree->adress = &is_http_version;
    tree->children = children;
    tree->children->next_node = children_next;

    int to_return = 0;
    if (strlen(characters) == 8)
    {
        to_return = ((characters[0] == 0x48 || characters[0] == 0x68) && (characters[1] == 0x54 || characters[1] == 0x74) && (characters[2] == 0x54 || characters[2] == 0x74) && (characters[3] == 0x50 || characters[3] == 0x70) && characters[4] == 0x2F && is_digit(characters[5],tree->children) && characters[6] == 0x2E && is_digit(characters[7],tree->children->next_node));
    }

    if(to_return == 0) printf("\n %s n'est pas une http-version valide. \n\n",characters);
    //else printf("\n %s est une http-version valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/* IP-literal = "[" ( IPv6address / IPvFuture ) "]" */
int is_ip_literal(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "ip_literal\0";
    tree->adress = &is_ip_literal;
    tree->children = children;

    int to_return = 0;
    int is_ipvsix_adress_ok = 0;
    int is_ipv_future_adress_ok = 0;

    int index = 0;
    int index_tmp;
    char tmp[strlen(characters) -2 +1];

    //printf("\n Evaluons si %s est une adresse IP littérale valide. \n\n",characters);

    /* la première condition certifie que strlen(characters) >= 1 pour pouvoir effectuer la 2eme comparaison */
    /* la deuxième condition certifie que strlen(characters) >= 2 pour pouvoir effectuer la boucle for*/
    /* Si le premier caractère est un [ et le dernier caractère est un ] */
    if (characters[index] == 0x5B && characters[strlen(characters)-1] == 0x5D)
    {
        for (index_tmp = 1; index_tmp < strlen(characters)-1 ; index_tmp++) tmp[index_tmp-1] = characters[index_tmp];
        tmp[strlen(characters) - 2] = '\0';

        //printf("\n Evaluons si %s est une adresse IPv6 ou une adresse IPvFuture valide. \n\n",tmp);
        /* Si tmp continet un nombre de char strictment inférieur à 2, to_return = 0 instantanément */
        is_ipvsix_adress_ok = is_ipvsix_adress(tmp,tree->children);
        //printf("\n Is_IPv6 adress valide : %d \n\n",is_ipvsix_adress_ok);
        if (is_ipvsix_adress_ok == 0) is_ipv_future_adress_ok = is_ipv_future_adress(tmp,tree->children);
        //printf("\n Is_IPvFuture adress valide : %d \n\n",is_ipv_future_adress_ok);
        to_return = (is_ipvsix_adress_ok || is_ipv_future_adress_ok);
    }

    //if (to_return == 0) printf("\n %s n'est pas une adresse IP littérale valide. \n\n",characters);
    //else printf("\n %s est une adresse IP littérale valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

/* IPvFuture = "v" 1* HEXDIG "." 1* ( unreserved / sub-delims / ":" ) */
/* Un is_hexdig ou un is_unreserved ou un is_sub-delims ne prend qu'un char en argument */
int is_ipv_future_adress(char *characters, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 1;

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "ipv_future_adress\0";
    tree->adress = &is_ipv_future_adress;
    tree->children = children;
    tree->children->next_node = children_next;
    
    //printf("\n Vérifions si %s est une adresse IP futuriste valide. \n\n",characters);

    /* certifie que l'on a bien au moins un hexdig et au moins un ( unreserved / sub-delims / ":" ) */
    /* certifie également que la chaine n'est pas juste '\0' */
    if(strlen(characters) >= 4 && characters[0] == 'v')
    {
        while (characters[index] != '\0' && characters[index] != '.')
        {
            to_return_tmp *= is_hexdig(characters[index],tree->children);
            index++;
        }
        
        if (characters[index] == 0x2E) index++; /* certifie que characters[index] était égal à '.' */
        
        while (characters[index] != '\0')
        {
            to_return_tmp *= (is_unreserved(characters[index],tree->children->next_node) || is_sub_delims(characters[index],tree->children->next_node) || characters[index] == 0x3A);
            index++;
        }

        to_return = to_return_tmp;
    }

    //if (to_return == 0) printf("\n %s n'est pas une adresse IPvFuture valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/* IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet */
/* Un dec_octet contient entre 1 et 3 characters */
/* Size min : 7, size max : 15 */
int is_ipvquatre_adress(char *characters, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 0;
    int index_tmp;
    int index_dec_octet;
    char tmp[4];
    tmp[3] = '\0';

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next_next;
    children_next_next_next = malloc(sizeof(tree_node));
    children_next_next_next->name = malloc(50*sizeof(char));
    children_next_next_next->name = "HTTP_message\0";
    children_next_next_next->adress = &is_http_message;
    children_next_next_next->content = malloc(500*sizeof(char));
    children_next_next_next->is_correct = 0;
    children_next_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next_next->children = malloc(sizeof(tree_node));

    tree->name = "ipvquatre_adress\0";
    tree->adress = &is_ipvquatre_adress;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;

    //printf("\n Testons si %s est une adresse IPv4 valide. \n\n",characters);

    if (characters[0] != '\0' && 7 <= strlen(characters) && strlen(characters) <= 15)
    {
        for (index_dec_octet = 0; index_dec_octet < 4; index_dec_octet++)
        {
            index_tmp = 0;

            /* 0x2E : un . */
            while(characters[index] != '\0' && characters[index] != 0x2E && index_tmp < 3)
            {
                //printf("\n Boucle while     %d     ok \n\n",index_tmp);
                tmp[index_tmp] = characters[index];
                index_tmp++;
                index++;
            }
            if (characters[index] == '\0' && index_dec_octet != 3) to_return_tmp = 0;
            else if (index_tmp == 3 && characters[index] != '\0' && characters[index] != 0x2E) to_return_tmp = 0;
            else if (index_tmp != 0)
            {
                while (index_tmp != 3)
                {
                    tmp[index_tmp] = '\0';
                    index_tmp++;
                }
                if (index_dec_octet == 0) to_return_tmp *= is_dec_octet(tmp,tree->children);
                if (index_dec_octet == 1) to_return_tmp *= is_dec_octet(tmp,tree->children->next_node);
                if (index_dec_octet == 2) to_return_tmp *= is_dec_octet(tmp,tree->children->next_node->next_node);
                if (index_dec_octet == 3) to_return_tmp *= is_dec_octet(tmp,tree->children->next_node->next_node->next_node);
            }
            else to_return_tmp = 0;

            index++;
        }

        to_return = to_return_tmp;
    }

    //if (to_return == 0) printf("\n %s n'est pas une adresse IPv4 valide. \n\n",characters);
    //else printf("\n %s est une adresse IPv4 valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/*
IPv6address = 
6 ( h16 ":" ) ls32 /                                ; case 1
"::" 5 ( h16 ":" ) ls32 /                           ; case 2
[ h16 ] "::" 4 ( h16 ":" ) ls32 /                   ; case 3
[ h16 *1 ( ":" h16 ) ] "::" 3 ( h16 ":" ) ls32 /    ; case 4
[ h16 *2 ( ":" h16 ) ] "::" 2 ( h16 ":" ) ls32 /    ; case 5
[ h16 *3 ( ":" h16 ) ] "::" h16 ":" ls32 /          ; case 6
[ h16 *4 ( ":" h16 ) ] "::" ls32 /                  ; case 7
[ h16 *5 ( ":" h16 ) ] "::" h16 /                   ; case 8
[ h16 *6 ( ":" h16 ) ] "::"                         ; case 9

Un h16 = 1 à 4 char ; Un ls32 = 3 à 15 char ;       0x3A = ':'
*/


/* nb_deux_points_expected vaut 6 pour traiter le premier cas, 5 pour le second, 4 pour le troisième ... */
/* si option = 0 on regarde si après les nb_deux_points_expected ':', il y a un ls32 */
/* si option = 1 on regarde si après les nb_deux_points_expected ':', après le dernier ':', il y a d'autre ':' */
int is_ipvsix_adress_first_case(char *characters, int nb_deux_points_expected, int option, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 0;
    int index_tmp = 0;
    int index_reset;
    int nb_deux_points = 0;

    char hseize_tmp[5];
    for (index_reset = 0; index_reset <= 4; index_reset++) hseize_tmp[index_reset] = '\0';

    char lstrentedeux_tmp[16];
    for (index_reset = 0; index_reset <= 15; index_reset++) lstrentedeux_tmp[index_reset] = '\0';

    //if (option == 0) printf("\n Vérifions que %s est une adresse IPv6 valide avec %d : expected et se termine par un ls_trentedeux \n\n",characters,nb_deux_points_expected);
    //else if (option == 1) printf("\n Vérifions que %s est une adresse IPv6 valide avec %d : expected et se termine par un : de plus que expected. \n\n",characters,nb_deux_points_expected);
    //else printf("\n Ne vérifions rien ! \n\n");

    while (characters[index] != '\0' && nb_deux_points <= nb_deux_points_expected)
    {
        if (nb_deux_points < nb_deux_points_expected)
        {
            while (characters[index] != '\0' && characters[index] != 0x3A)
            {
                hseize_tmp[index_tmp] = characters[index];
                index++;
                index_tmp++;
            }
        }
        else
        {
            while (characters[index] != '\0' && characters[index] != 0x3A)
            {
                lstrentedeux_tmp[index_tmp] = characters[index];
                index++;
                index_tmp++;
            }
        }
        /* Quatre possibilités :
            - La boucle s'arrête parce que '\0' est attteint et nb_deux_points != nb_deux_points_expected :
                    Ce ne sera pas bon. Inutile d'aller plus loin.
            - La boucle s'arrête parce que '\0' est atteint et nb_deux_points == nb_deux_points_expected
                    Si option == 0
                        Comparer la chaine depuis les derniers : avec is_lstrentedeux
                    Sinon Comparer la chaine depuis les derniers : avec ":\0"
            - La boucle s'arrête parce que ':' est atteint et nb_deux_points != nb_deux_points_expected
                    Augmenter nb_deux_points et comparer avec is_hseize
            - La boucle s'arrête parce que ':' est atteint et nb_points == nb_deux_points_expected
                    Si option == 0
                       Charger la variable lstrentedeux_tmp puis comparer avec is_lstrentedeux
                    Sinon idem puis comparer avec ":\0"
        */
        if (characters[index] == '\0' && nb_deux_points != nb_deux_points_expected) to_return_tmp = 0;
        else if (characters[index] == '\0' && nb_deux_points == nb_deux_points_expected)
        {
            //printf("\n La fin de la chaine a été atteinte et %d == %d et option = %d et to_return_tmp = %d \n\n",nb_deux_points,nb_deux_points_expected,option,to_return_tmp);
            if (option == 0)
            {
                //printf("\n Vérifions si %s (%s) est un ls_trentedeux valide. \n\n",lstrentedeux_tmp,characters + index - index_tmp);
                to_return_tmp *= is_lstrentedeux(lstrentedeux_tmp,tree);
                //if(to_return_tmp != 0) printf("\n C'était un ls_trentedeux valide !! Il faut sortir de la boucle !! \n\n");
            }
            else to_return_tmp *= (lstrentedeux_tmp[0] == 0x3A && lstrentedeux_tmp[1] == '\0');
            nb_deux_points++; /* Pour sortir de la boucle */
        }
        else if (characters[index] == 0x3A && nb_deux_points != nb_deux_points_expected)
        {
            nb_deux_points++;
            //printf("\n On a désormais trouvé %d : \n\n",nb_deux_points);
            to_return_tmp *= is_hseize(hseize_tmp,tree);
            for (index_reset = 0; index_reset < 4; index_reset++) hseize_tmp[index_reset] = '\0';
            index++;
            index_tmp = 0;
        }
        else /* Si characters[index] == ':' && nb_deux_points == nb_deux_points_expected */
        {
            //printf("\n On a trouvé un : et on en avait déjà trouvé %d. \n\n",nb_deux_points);
            nb_deux_points++;
            for (index_tmp = 0; index_tmp < strlen(characters) - index; index_tmp++) lstrentedeux_tmp[index_tmp] = characters[index + index_tmp];
            if (option == 0) to_return_tmp *= is_lstrentedeux(lstrentedeux_tmp,tree);
            else to_return_tmp *= (lstrentedeux_tmp[0] == 0x3A && lstrentedeux_tmp[1] == '\0');
        }

        /* Dans tous les cas */
        to_return = to_return_tmp;
    }

    return to_return;
}


/*
        "::" 5 ( h16 ":" ) ls32 /                           ; case 2
*/
int is_ipvsix_adress_second_case(char *characters, tree_node *tree)
{
    int to_return = 0;
    if (strlen(characters) > 2)
    {
        to_return = (characters[0] == 0x3A && characters[1] == 0x3A && is_ipvsix_adress_first_case((characters+2), 5, 0,tree));
    }

    return to_return;
}


/*
        [ h16 ] "::" 4 ( h16 ":" ) ls32 /                   ; case 3
*/
int is_ipvsix_adress_third_case(char *characters, tree_node *tree)
{
    int to_return = 0;
    int index = 0;
    int is_hseize_ok;
    int is_ipvsix_adress_first_case_ok;

    char hseize_tmp[5];
    hseize_tmp[4] = '\0';

    //printf("\n Vérifions que %s est une adresse IPv6 de troisième format valide. \n\n",characters);

    while(characters[index] != '\0' && characters[index] != 0x3A && index < 4)
    {
        hseize_tmp[index] = characters[index];
        index++;
    }
    //printf("\n Fin de la boucle dans le troisième cas de la comparaison is_IPv6_adress valide. H_seize_tmp : %s et Index : %d \n\n",hseize_tmp,index);

    if(characters[index] != '\0' && characters[index] == 0x3A)
    {
        if(characters[index+1] != '\0' && characters[index+1] == 0x3A)
        {
            //printf("\n Vérifions si %s est un hseize valide et si %s est une adresse IPv6 valide pour le premier cas. \n\n",hseize_tmp,characters+index+2);
            is_hseize_ok = is_hseize(hseize_tmp,tree);
            is_ipvsix_adress_first_case_ok = is_ipvsix_adress_first_case((characters+index+2), 4, 0,tree);
            to_return = (is_hseize_ok && is_ipvsix_adress_first_case_ok);
        }
    }

    //if(to_return == 0) printf("\n %s n'est pas une adresse IPv6 de troisième format valide. \n\n",characters);
    return to_return;
}


int is_ipvsix_adress_fourth_case(char *characters, tree_node *tree)
{
    int to_return = 0;
    int index = 0;
    int nb_deux_points = 0;

    int index_hseize_option;
    int is_hseize_option_correct = 0;
    int index_hseize_option_correct = -1;

    //printf("\n Vérifions que %s est une adresse IPv6 de quatrième format valide. \n\n",characters);

    for (index_hseize_option = 1; index_hseize_option <= 6; index_hseize_option++)
    {
        is_hseize_option_correct = (is_hseize_option_correct || is_ipvsix_adress_first_case(characters, index_hseize_option+1, 1,tree));
        if (is_hseize_option_correct) index_hseize_option_correct = index_hseize_option;
    }

    if (is_hseize_option_correct) /* certifie qu'il y ait deux ':' successifs quelque part */
    {
        //printf("\n Is hseize option correct : %d \n\n",is_hseize_option_correct);
        while (characters[index] != 0x3A || characters[index+1] != 0x3A) index++;
        index++; /* à ce stade, characters[index] est le deuxième : des deux : successifs */
        
        /*
        [ h16 *1 ( ":" h16 ) ] "::" 3 ( h16 ":" ) ls32 / 
        [ h16 *2 ( ":" h16 ) ] "::" 2 ( h16 ":" ) ls32 / 
        [ h16 *3 ( ":" h16 ) ] "::" h16 ":" ls32 / 
        [ h16 *4 ( ":" h16 ) ] "::" ls32 / 
        [ h16 *5 ( ":" h16 ) ] "::" h16 / 
        [ h16 *6 ( ":" h16 ) ] "::" 
        */
        if (index_hseize_option_correct == 1) to_return = is_ipvsix_adress_first_case((characters + index + 1), 3, 0,tree);
        if (index_hseize_option_correct == 2) to_return = is_ipvsix_adress_first_case((characters + index + 1), 2, 0,tree);
        if (index_hseize_option_correct == 3) to_return = is_ipvsix_adress_first_case((characters + index + 1), 1, 0,tree);
        if (index_hseize_option_correct == 4) to_return = is_lstrentedeux(characters + index + 1,tree);
        if (index_hseize_option_correct == 5) to_return = is_hseize(characters + index + 1,tree);
        if (index_hseize_option_correct == 6) to_return = 1;
    }
    else if (strlen(characters) >= 2) /* Traiter les cas où il n'y a pas d'option !!!! */
    {
        //printf("\n Il n'y a pas d'option, l'adresse %s commence donc par :: \n\n",characters);
        if(characters[0] == 0x3A && characters[1] == 0x3A)
        {
            /*
            "::" 3 ( h16 ":" ) ls32 / 
            ::" 2 ( h16 ":" ) ls32 / 
            "::" h16 ":" ls32 / 
            "::" ls32 / 
            "::" h16 / 
            "::" 
            */
            index = 2;
            while (characters[index] != '\0')
            {
                if (characters[index] == 0x3A) nb_deux_points++;
                index++;
            }
            index = 2;

            if (nb_deux_points == 0 && characters[2] == '\0') to_return = 1;
            else if (nb_deux_points == 0) to_return = (is_hseize(characters+2,tree) || is_lstrentedeux(characters+2,tree));
            else if (nb_deux_points >= 1) to_return = is_ipvsix_adress_first_case((characters+2), nb_deux_points, 0,tree);
        }
    }

    return to_return;
}



int is_ipvsix_adress(char *characters, tree_node *tree)
{
    int to_return = 0;
    int is_ipvsix_adress_first_case_ok;
    int is_ipvsix_adress_second_case_ok;
    int is_ipvsix_adress_third_case_ok;
    int is_ipvsix_adress_fourth_case_ok;

    /*
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    */

    tree->name = "ipvsix_adress\0";
    tree->adress = &is_ipvsix_adress;
    //tree->children = children;
    tree->children = NULL;

    //printf("\n Vérifions si %s est une adresse IPv6 valide. \n\n",characters);

    /* La plus petite adresse IPv6 acceptée selon les règles abnf est (cas 6) "::" */
    if(strlen(characters) >= 2)
    {
        is_ipvsix_adress_first_case_ok = is_ipvsix_adress_first_case(characters, 6, 0,tree);
        //if (is_ipvsix_adress_first_case_ok == 0) printf("\n %s n'est pas une adresse IPv6 du premier format. \n\n",characters);

        is_ipvsix_adress_second_case_ok = is_ipvsix_adress_second_case(characters,tree);
        //if (is_ipvsix_adress_second_case_ok == 0) printf("\n %s n'est pas une adresse IPv6 du second format. \n\n",characters);

        is_ipvsix_adress_third_case_ok = is_ipvsix_adress_third_case(characters,tree);
        //if (is_ipvsix_adress_third_case_ok == 0) printf("\n %s n'est pas une adresse IPv6 du troisième format. \n\n",characters);

        is_ipvsix_adress_fourth_case_ok = is_ipvsix_adress_fourth_case(characters,tree);
        //if (is_ipvsix_adress_fourth_case_ok == 0) printf("\n %s n'est pas une adresse IPv6 du quatrième format. \n\n",characters);

        to_return = (is_ipvsix_adress_first_case_ok || is_ipvsix_adress_second_case_ok || is_ipvsix_adress_third_case_ok || is_ipvsix_adress_fourth_case_ok);
        //to_return = (is_ipvsix_adress_first_case(characters, 6, 0) || is_ipvsix_adress_second_case(characters) || is_ipvsix_adress_third_case(characters) || is_ipvsix_adress_fourth_case(characters));
    }
    //if (to_return == 0) printf("\n %s n'est pas une adresse IPv6 valide. \n\n",characters);
    //else printf("\n %s est une adresse IPv6 valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_lf(char character, tree_node *tree)
{
    int to_return = (character == 0x0A);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/* ls32 = 
( h16 ":" h16 ) /           size min : 3 ; size max : 9
IPv4address                 size min : 7 ; size max : 15
*/
int is_lstrentedeux(char *characters, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 0;
    int index_deux_points;
    int is_deux_hseize = 1;

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "lstrentedeux\0";
    tree->adress = &is_lstrentedeux;
    tree->children = children;
    tree->children->next_node = children_next;

    while(characters[index] != '\0' && characters[index] != 0x3A) index++;
    if (characters[index] == '\0') is_deux_hseize = 0;
    else index_deux_points = index;

    if (is_deux_hseize)
    {
        char tmp[index_deux_points+1];
        tmp[index_deux_points] = '\0';
        for (index = 0; index < index_deux_points; index++) tmp[index] = characters[index];

        to_return_tmp *= is_hseize(tmp,tree->children);
        for (index = index_deux_points; characters[index] != '\0'; index++) tmp[index - index_deux_points] = characters[index];
        while (index - index_deux_points < index_deux_points)
        {
            tmp[index - index_deux_points] = '\0';
            index++;
        }
        to_return_tmp *= is_hseize(tmp,tree->children->next_node);
        to_return = to_return_tmp;
    }
    else to_return = is_ipvquatre_adress(characters,tree->children);

    //if (to_return == 0) printf("\n %s n'est pas un ls_trentedeux valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


// LWSP = *(WSP / CRLF WSP)
int is_lwsp(char *characters, tree_node *tree)
{
    int to_return = 0;
    int size = strlen(characters);
    int index;
    char tmp[3];
    tmp[2] = '\0';

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "lwsp\0";
    tree->adress = &is_lwsp;
    tree->children = children;

    if (size > 0)
    {
        to_return = 1;
        for (index = 0; index < size; index++)
        {
            if (index + 1 < size && index + 2 < size)
            {
                tmp[0] = characters[index];
                tmp[1] = characters[index+1];
                to_return *= (is_wsp(characters[index],tree) || (is_crlf(tmp,tree->children) && is_wsp(characters[index+2],tree)));
                /* problème si la comparaison avec CR LF est effectuée deux fois */
                /* par exemple si CR LF WSP est envoyé, la deuxième comparaison fait passer to_return à 0 */
                if(is_crlf(tmp,tree->children)) index++;
                /*if(to_return == 0)
                {
                    printf("\n %d || %d && %d\n\n",is_wsp(characters[index]),is_crlf(tmp),is_wsp(characters[index+2]));
                }*/
            }
            /* si il ne reste qu'un ou deux caractère(s), ça ne peut pas être un CR LF WSP (trois caractères) */
            else
            {
                to_return *= is_wsp(characters[index],tree);
            }
            // if (to_return == 0) printf("\nWARNING to_return a changé à l'indice %d \n",index);
        }
    }
    //else printf("\nTaille Invalide (taille attendue > 0, taille réelle : %lu)\n",strlen(characters));

    //if(to_return == 0) printf("\n %s n'est pas un LWSP valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_message_body(char *characters, tree_node *tree)
{
    int to_return = 1;
    int index = 0;

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "message_body\0";
    tree->adress = &is_message_body;
    tree->children = children;

    while(characters[index] != '\0' || characters[index] != '\n' || characters[index] != '\r')
    {
        to_return *= is_octet(characters[index],tree->children);
        index++;
    }

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/* obs-text = %x80-FF */
int is_obs_text(char character, tree_node *tree)
{
    int to_return = (0x80 <= character && character <= 0xFF);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    return to_return;
}


int is_octet(char character, tree_node *tree)
{
    int to_return = (0x00 <= character && character <= 0xFF);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_ows(char *characters, tree_node *tree)
{
    int to_return = 1;
    int index;

    while(characters[index] != '\0')
    {
        to_return *= (is_wsp(characters[index],tree));
        index++;
    }

    return to_return;
}


// Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@" 
int is_pchar(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
  
    tree->name = "pchar\0";
    tree->adress = &is_pchar;
    tree->children = children;

    //printf("\n Is_pchar %s de taille %lu ?\n\n",characters,strlen(characters));
    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = is_pct_encoded(characters,tree->children);
    }
    else if(strlen(characters) == 1)
    {
        to_return = (is_unreserved(characters[0],tree->children) || is_sub_delims(characters[0],tree->children) || characters[0] == 0x3A || characters[0] == 0x40);
    }

    //if(to_return == 0) printf("\n %s n'est pas un pchar valide. \n\n", characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

// pct-encoded = "%" HEXDIG HEXDIG
int is_pct_encoded(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "pct_encoded\0";
    tree->adress = &is_pct_encoded;
    tree->children = children;
    tree->children->next_node = children_next;

    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = (characters[0] == 0x25 && is_hexdig(characters[1],tree->children) && is_hexdig(characters[2],tree->children->next_node));
    }

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_port(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "port\0";
    tree->adress = &is_port;
    tree->children = children;

    int to_return = 1;
    int index = 0;

    //printf("\n Is port : %s. \n\n",characters);

    while(characters[index] != '\0')
    {
        to_return *= (is_digit(characters[index],tree->children));
        index++;
    }

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    if (to_return == 0) printf("\n %s n'est pas un port valide. \n\n",characters);
    return to_return;
}


int is_qdtext(char character, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "qdtext\0";
    tree->adress = &is_qdtext;
    tree->children = children;

    int to_return = (is_htab(character,tree) || is_sp(character,tree) || is_obs_text(character,tree->children) || character == 0x21 || (0x23 <= character && character <= 0x5B) || (0x5D <= character && character <= 0x7E));

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/* Une query est une combinaison de ( pchar / "/" / "?" )  */
int is_query(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "query\0";
    tree->adress = &is_query;
    tree->children = children;

    int to_return = 1;
    int to_return_tmp;
    int index = 1;
    int index_tmp;

    char tmp[4];
    tmp[3] = '\0';
    char test[2];
    test[1] = '\0';

    while(characters[0] != '\0' && characters[index] != '\0')
    {
        //printf("\n %c %c %c\n\n",characters[index],characters[index+1],characters[index+2]);
        //break;
        if(characters[index+1] != '\0' && characters[index + 2] != '\0')
        {
            tmp[0] = characters[index];
            tmp[1] = characters[index + 1];
            tmp[2] = characters[index + 2];
            //printf("\n Tmp : %s de taille %lu avec %c ignoré\n\n",tmp,strlen(tmp),characters[index+2]);
            
            to_return_tmp = is_pchar(tmp,tree->children);
            if(to_return_tmp == 0)
            {
                for (index_tmp = 0; index_tmp < 2; index_tmp++)
                {
                    test[0] = characters[index + index_tmp];
                    //printf("\n Test : %s de taille %lu\n\n",test,strlen(test));
                    to_return *= (is_pchar(test,tree->children) || test[0] == '/' || test[0] == '?');
                    //if(to_return == 0) printf("\n %s n'est pas un pchar valide. \n\n",test);
                }
                /*
                to_return *= is_pchar(tmp[1]);
                if(to_return == 0) printf("\n %s n'est pas un pchar valide. \n\n",tmp);
                to_return *= is_pchar(&characters[index + 2]);
                if(to_return == 0) printf("\n %c n'est pas un pchar valide. \n\n",characters[index+2]);
                */
            }
            index = index + 3;
        }
        else if(characters[index + 1] != '\0') /* il y a deux caractères avant '\0' */
        { 
            to_return *= is_pchar(&characters[index],tree->children);
            to_return *= is_pchar(&characters[index + 1],tree->children);
            index = index + 2;
        }
        else
        {
            to_return *= is_pchar(&characters[index],tree->children);
            index++;
        }
    }
    if (characters[0] != '\0' && characters[1] == '\0')
    {
        to_return = (is_pchar(characters,tree->children) || characters[0] == 0x3F || characters[0] == 0x2F);
    }

    //if(to_return == 0) printf("\n %s n'est pas une query valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_quoted_pair(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "quoted_pair\0";
    tree->adress = &is_quoted_pair;
    tree->children = children;

    /* La première condition assure qu'il y a au moins un caractère pour faire la deuxième. */
    int to_return = (characters[0] == 0x5C && (is_sp(characters[1],tree) || is_htab(characters[1],tree) || is_vchar(characters[1],tree->children) || is_obs_text(characters[1],tree->children)));

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_quoted_string(char *characters, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 1;
    int index_tmp;
    char quoted_pair_tmp[3];
    quoted_pair_tmp[2] = '\0';

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "quoted_string\0";
    tree->adress = &is_quoted_string;
    tree->children = children;

    if (strlen(characters) >= 2 && characters[0] == 0x5B)
    {
        while (characters[index] != '\0' && characters[index] != 0x5D) index++;
        if (characters[index] == 0x5D)
        {
            char tmp[index - 1];
            for (index_tmp = 0; index_tmp < index-1; index_tmp++) tmp[index_tmp] = characters[index_tmp + 1];
            tmp[index_tmp] = '\0';

            for (index = 0; index < index_tmp; index++)
            {
                if (tmp[index+1] != '\0')
                {
                    quoted_pair_tmp[0] = tmp[index];
                    quoted_pair_tmp[1] = tmp[index + 1];
                    to_return_tmp *= (is_qdtext(tmp[index],tree->children) || is_quoted_pair(quoted_pair_tmp,tree->children));
                }
                else to_return_tmp *= is_qdtext(tmp[index],tree->children);
            }

            to_return = to_return_tmp;
        }
    }

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


// Une reason-phrase doit être une combinaison de HTAB, d'espaces, de VCHAR ou d'obs-text (=?? %x80-FF)
int is_reason_phrase(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "reason_phrase\0";
    tree->adress = &is_reason_phrase;
    tree->children = children;
    
    int to_return = 1;
    int index;

    for(index = 0; index < strlen(characters); index++)
    {
        to_return *= (is_htab(characters[index],tree) || is_sp(characters[index],tree) || is_vchar(characters[index],tree->children) || is_obs_text(characters[index],tree->children));
    }


    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


/* reg-name = * ( unreserved / pct-encoded / sub-delims )  */
/* Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@"   */ 
int is_reg_name(char *characters, tree_node *tree)
{
    int to_return = 1;
    int index;
    char test[2];
    test[1] = '\0';

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "reg_name\0";
    tree->adress = &is_reg_name;
    tree->children = children;

    for(index = 0; index < strlen(characters); index++)
    {
        test[0] = characters[index];
        to_return *= (test[0] != 0x3A && test[0] != 0x40 && is_pchar(test,tree->children));
    }

    //if (to_return == 0) printf("\n %s n'est pas un reg name valide. \n\n",characters);
    //else printf("\n %s est un reg name valide !! \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


// Un segment est une combinaison de pchar
// Un pchar fait soit un caractère soit trois caractères
int is_segment(char *characters, tree_node *tree)
{
    //printf("\n Segment : %s de taille %lu\n\n",characters,strlen(characters));
    int to_return = 1;
    int to_return_tmp;
    int index = 0;
    int index_tmp;
    char tmp[4];
    tmp[3] = '\0';
    char test[2];
    test[1] = '\0';

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "segment\0";
    tree->adress = &is_segment;
    tree->children = children;

    while(characters[index] != '\0')
    {
        //printf("\n %c %c %c\n\n",characters[index],characters[index+1],characters[index+2]);
        //break;
        if(characters[index+1] != '\0' && characters[index + 2] != '\0')
        {
            tmp[0] = characters[index];
            tmp[1] = characters[index + 1];
            tmp[2] = characters[index + 2];
            //printf("\n Tmp : %s de taille %lu\n\n",tmp,strlen(tmp));
            
            to_return_tmp = is_pchar(tmp,tree->children);
            if(to_return_tmp == 0)
            {
                for (index_tmp = 0; index_tmp < 3; index_tmp++)
                {
                    test[0] = characters[index + index_tmp];
                    //printf("\n Test : %s de taille %lu : %d\n\n",test,strlen(test),is_pchar(test));
                    to_return *= is_pchar(test,tree->children);
                    //if(to_return == 0) printf("\n %s n'est pas un pchar valide. \n\n",test);
                }
                /*
                to_return *= is_pchar(tmp[1]);
                if(to_return == 0) printf("\n %s n'est pas un pchar valide. \n\n",tmp);
                to_return *= is_pchar(&characters[index + 2]);
                if(to_return == 0) printf("\n %c n'est pas un pchar valide. \n\n",characters[index+2]);
                */
            }
            index = index + 3;
        }
        else if(characters[index + 1] != '\0') /* il y a deux caractères avant '\0' */
        { 
            to_return *= is_pchar(&characters[index],tree->children);
            to_return *= is_pchar(&characters[index + 1],tree->children);
            index = index + 2;
        }
        else
        {
            to_return *= is_pchar(&characters[index],tree->children);
            index++;
        }
    }
    if (characters[0] != '\0' && characters[1] == '\0')
    {
        to_return = is_pchar(characters,tree->children);
    }
    

    //if(to_return == 0) printf("\n %s n'est pas un segment valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_sp(char character, tree_node *tree)
{
    return (character == 0x20);
}


int is_start_line(char *ligne, int taille_ligne, char *mots[], int tailles_des_mots[], tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "start_line\0";
    tree->adress = &is_start_line;
    tree->children = children;

    int to_return = (taille_ligne >= 11);
    
    //printf("\n Ligne start line : %s de taille %d\n\n",ligne,taille_ligne);

    int to_return_status = (is_status_line(ligne,mots,tailles_des_mots,tree->children));
    int to_return_request = 0;
    if (to_return_status == 0) to_return_request = (is_request_line(ligne,mots,tailles_des_mots,tree->children));

    //printf("\n TO RETURN REQUEST : %d \n\n \n\n",to_return_request);

    if (to_return != 0) to_return = (to_return_status || to_return_request);

    if (to_return)
    {
        tree->content = ligne;
        tree->is_correct = 1;
    }
    else
    {
        tree->content = NULL;
        purgeNode(tree);
    }

    if (to_return == 0) printf("\n %s n'est pas une start line valide. \n\n",ligne);
    return to_return;
}



// Un status_code ne doit comporter que trois caractères 
// Chacun de ces caractères doit être un DIGIT
int is_status_code(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree->name = "status_code\0";
    tree->adress = &is_status_code;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;

    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = (is_digit(characters[0],tree->children) && is_digit(characters[1],tree->children->next_node) && is_digit(characters[2],tree->children->next_node->next_node));
    }
    
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    return to_return;
}


// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
int is_sub_delims(char character, tree_node *tree)
{
    int to_return = (character == 0x21 || character == 0x24 || character == 0x26 || character == 0x27 || character == 0x28 || character == 0x29 || character == 0x2A || character == 0x2B || character == 0x2C || character == 0x3B || character == 0x3D);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    return to_return;
}

// TCHAR = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA 
int is_tchar(char character, tree_node *tree)
{
    int to_return = (is_digit(character,tree) || is_alpha(character,tree) || character == 0x21 || character == 0x23 || character == 0x24 || character == 0x25 || character == 0x26 || character == 0x27 || character == 0x2A || character == 0x2B || character == 0x2D || character == 0x2E || character == 0x5E || character == 0x5F || character == 0x60 || character == 0x7C || character == 0x7E);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

// Un token est composé d'au moins un caractère ET tous ses caractères sont des TCHAR
int is_token(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "token\0";
    tree->adress = &is_token;
    tree->children = children;

    int to_return = 0;
    int is_tchar_okay = 1;
    int token_size = taille_mot(characters); /* fonction définie dans utils.h */

    int index;
    if(token_size >= 1)
    {
        for(index = 0; index < token_size; index++)
        {
            is_tchar_okay *= is_tchar(characters[index],tree->next_node);
        }

        to_return = is_tchar_okay;
    }

    //if(to_return == 0) printf("\n %s n'est pas un token valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_transfer_coding(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "transfer_coding\0";
    tree->adress = &is_transfer_coding;
    tree->children = children;

    /* Pour toute chaine de char valide, on a characters[strlen(characters)] = '\0' */
    int to_return = ((strcmp(characters, "chunked") == 0) || (strcmp(characters,"compress") == 0) || (strcmp(characters,"deflate") == 0) || (strcmp(characters,"gzip") == 0) || is_transfer_extension(characters,tree->children));

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


int is_transfer_encoding(char *characters, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;

    int index = 0;
    int index_transfer_coding = 0;
    int index_reset;

    char transfer_coding[strlen(characters) + 1];
    transfer_coding[strlen(characters)] = '\0';

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "transfer_encoding\0";
    tree->adress = &is_transfer_encoding;
    tree->children = children;

    //printf("\n Vérifions si %s est un transfer_encoding. \n\n",characters);

    /* La virgule n'est pas dans les tchar !!!! Quelle chance */
    /* Si on rencontre une virgule, on avance jusqu'à rencontrer un tchar ; break si ni sp ni htab ni tchar */
    /* Si pas de virgule, directement un tchar sinon exit */
    while(characters[index] != '\0')
    {
        while(characters[index] == 0x2C || is_sp(characters[index],tree) || is_htab(characters[index],tree)) index++;
        if (is_tchar(characters[index],tree->children))
        {
            while(is_tchar(characters[index],tree->children))
            {
                transfer_coding[index_transfer_coding] = characters[index];
                index_transfer_coding++;
                index++;
            }
            transfer_coding[index_transfer_coding] = '\0';

            to_return_tmp *= is_transfer_coding(transfer_coding,tree->children);
            //if (to_return_tmp == 0) printf("\n\n\n\n\n %s n'est pas un transfer_coding valide. \n\n",transfer_coding);
            to_return = to_return_tmp;

            for (index_reset = 0; index_reset < index_transfer_coding; index_reset++) transfer_coding[index_reset] = '\0';
            index_transfer_coding = 0;
        }
    }

    printf("\n\n\n %s n'est pas un transfer_encoding. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}



int is_transfer_encoding_header(char *characters, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "transfer_encoding_header\0";
    tree->adress = &is_transfer_encoding_header;
    tree->children = children;

    int to_return = 0;
    int index = 18;
    int index_transfer_encoding = 0;

    //printf("\n Entrée dans le if imminente avec %s comme chaine \n\n",characters);

    if (strlen(characters) >= 18 && (characters[0] == 0x54 || characters[0] == 0x74) && (characters[1] == 0x72 || characters[1] == 0x52) && (characters[2] == 0x61 || characters[2] == 0x41) && (characters[3] == 0x6E || characters[3] == 0x4E) && (characters[4] == 0x73 || characters[4] == 0x53) && (characters[5] == 0x66 || characters[5] == 0x46) && (characters[6] == 0x65 || characters[6] == 0x45) && (characters[7] == 0x72 || characters[7] == 0x52) && characters[8] == 0x2D && (characters[9] == 0x45 || characters[9] == 0x65) && (characters[10] == 0x6E || characters[10] == 0x4E) && (characters[11] == 0x63 || characters[11] == 0x43) && (characters[12] == 0x6F || characters[12] == 0x4F) && (characters[13] == 0x64 || characters[13] == 0x44) && (characters[14] == 0x69 || characters[14] == 0x49) && (characters[15] == 0x6E || characters[15] == 0x4E) && (characters[16] == 0x67 || characters[16] == 0x47) && characters[17] == 0x3A)
    {
        //printf("\n Vérifions si %s est un transfer encoding header. \n\n",characters);
        char transfer_encoding[strlen(characters) - 17];
        while (is_sp(characters[index],tree) || is_htab(characters[index],tree) || characters[index] == 0x2C) index++;
        while (characters[index] != '\0' && !is_sp(characters[index],tree) && !is_htab(characters[index],tree) && (characters[index] != 0x2C))
        {
            transfer_encoding[index_transfer_encoding] = characters[index];
            index_transfer_encoding++;
            index++;
        }
        transfer_encoding[index_transfer_encoding] = '\0';
        //printf("\n\n\n\n Transfer encoding : %s. \n\n",transfer_encoding);
        
        to_return = is_transfer_encoding(transfer_encoding,tree->children);
        //if (to_return == 0) printf("\n\n\n %s n'est pas un transfer_encoding_header valide. \n\n",characters);
        if (characters[index] != '\0')
        {
            while(is_sp(characters[index],tree) || is_htab(characters[index],tree) || (characters[index] == 0x2C)) index++;
            if(characters[index] != '\0') to_return = 0;
        }
    }
    //else printf("\n %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n\n", strlen(characters) >= 18, (characters[0] == 0x54 || characters[0] == 0x74), (characters[1] == 0x72 || characters[1] == 0x52), (characters[2] == 0x61 || characters[2] == 0x41), (characters[3] == 0x6E || characters[3] == 0x4E), (characters[4] == 0x73 || characters[4] == 0x53), (characters[5] == 0x66 || characters[5] == 0x46), (characters[6] == 0x65 || characters[6] == 0x45), (characters[7] == 0x72 || characters[7] == 0x52), characters[8] == 0x2D, (characters[9] == 0x45 || characters[9] == 0x65), (characters[10] == 0x6E || characters[10] == 0x4E), (characters[11] == 0x63 || characters[11] == 0x43), (characters[12] == 0x6F || characters[12] == 0x4F), (characters[13] == 0x64 || characters[13] == 0x44), (characters[14] == 0x69 || characters[14] == 0x49), (characters[15] == 0x6E || characters[15] == 0x4E), (characters[16] == 0x67 || characters[16] == 0x47), characters[17] == 0x3A);

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}



int is_transfer_extension(char *characters, tree_node *tree)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 0;
    int index_transfer_parameter = 0;
    char token[strlen(characters) + 1];
    char transfer_parameter[strlen(characters)];

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree->name = "transfer_extension\0";
    tree->adress = &is_transfer_extension;
    tree->children = children;
    tree->children->next_node = children_next;

    while(characters[index] && !is_sp(characters[index],tree) && !is_htab(characters[index],tree))
    {
        token[index] = characters[index];
        index++;
    }
    token[index] = '\0';

    to_return = is_token(token,tree->children);

    /* A ce stade, soit to_return = 0 et on quitte, soit to_return != 0 et on continue */
    while (characters[index] != '\0')
    {
        /* assure que characters[index] != '\0' && characters[index] != ';' (0x3B) */
        while(is_sp(characters[index],tree) || is_htab(characters[index],tree)) index++;
        if (characters[index] == 0x3B)
        {
            index++;
            while(is_sp(characters[index],tree) || is_htab(characters[index],tree)) index++; /* idem */
            while (characters[index] != '\0' && !is_sp(characters[index],tree) && ! is_htab(characters[index],tree))
            {
                transfer_parameter[index_transfer_parameter] = characters[index];
                index_transfer_parameter++;
                index++;
            }
            to_return_tmp *= is_transfer_parameter(transfer_parameter,tree->children->next_node);

            for (index_transfer_parameter = 0; index_transfer_parameter < strlen(characters); index_transfer_parameter++) transfer_parameter[index_transfer_parameter] = '\0';
            index_transfer_parameter = 0;
        }
        else /* option invalide. on sort de la boucle */
        {
            to_return_tmp = 0;
            to_return = 0;
            /* Pour toute chaine de char valide, on a characters[strlen(characters)] = '\0' */
            index = strlen(characters);
        }
    }

    if(to_return != 0) to_return = to_return_tmp;
    //else printf("\n %s n'est pas un token valide. \n\n",token);

    //if (to_return == 0) printf("\n %s n'est pas une transfer-extension valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}



int is_transfer_parameter(char *characters, tree_node *tree)
{
    int to_return = 0;
    int index = 0;
    int index_token = 0;
    char token[strlen(characters)]; /* Pas + 1 car doit comporter un '=' qui ne fait pas partie du token */

    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "transfer_parameter\0";
    tree->adress = &is_transfer_parameter;
    tree->children = children;

    while (characters[index] != '\0' && !is_sp(characters[index],tree) && !is_htab(characters[index],tree))
    {
        token[index] = characters[index];
        index++;
    }
    token[index] = '\0';

    if (characters[index] != '\0' && is_token(token,tree))
    {
        while(is_sp(characters[index],tree) || is_htab(characters[index],tree)) index++;
        if (characters[index] == 0x3D)
        {
            index++;
            while(is_sp(characters[index],tree) || is_htab(characters[index],tree)) index++;

            while(characters[index] != '\0')
            {
                token[index_token] = characters[index];
                index_token++;
                index++;
            }

            to_return = (is_token(token,tree->children) || is_quoted_string(token,tree->children));
        }
    }

    //if (to_return == 0) printf("\n %s n'est pas un transfer parameter valide. \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" 
int is_unreserved(char character, tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));

    tree->name = "unreserved\0";
    tree->adress = &is_unreserved;
    tree->children = children;

    int to_return = (is_alpha(character,tree->children) || is_digit(character,tree->children) || character == 0x2D || character == 0x2E || character == 0x5F || character == 0x7E);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}

int is_vchar(char character, tree_node *tree)
{
    int to_return = (0x21 <= character && character <= 0x7E);

    if (to_return)
    {
        tree->content = &character;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }

    return to_return;
}

int is_wsp(char character, tree_node *tree)
{
    return (is_sp(character,tree) || is_htab(character,tree));
}
	





/*
 rulelist       =  1*( rule / (*c-wsp c-nl) )

         rule           =  rulename defined-as elements c-nl
                                ; continues if next line starts
                                ;  with white space

         rulename       =  ALPHA *(ALPHA / DIGIT / "-")


         defined-as     =  *c-wsp ("=" / "=/") *c-wsp
                                ; basic rules definition and
                                ;  incremental alternatives

         elements       =  alternation *c-wsp

         c-wsp          =  WSP / (c-nl WSP)

         c-nl           =  comment / CRLF
                                ; comment or newline

         comment        =  ";" *(WSP / VCHAR) CRLF

         alternation    =  concatenation
                           *(*c-wsp "/" *c-wsp concatenation)

         concatenation  =  repetition *(1*c-wsp repetition)

         repetition     =  [repeat] element

         repeat         =  1*DIGIT / (*DIGIT "*" *DIGIT)

         element        =  rulename / group / option /
                           char-val / num-val / prose-val

         group          =  "(" *c-wsp alternation *c-wsp ")"

         option         =  "[" *c-wsp alternation *c-wsp "]"

         char-val       =  DQUOTE *(%x20-21 / %x23-7E) DQUOTE
                                ; quoted string of SP and VCHAR
                                ;  without DQUOTE

         num-val        =  "%" (bin-val / dec-val / hex-val)

         bin-val        =  "b" 1*BIT
                           [ 1*("." 1*BIT) / ("-" 1*BIT) ]
                                ; series of concatenated bit values
                                ;  or single ONEOF range

         dec-val        =  "d" 1*DIGIT
                           [ 1*("." 1*DIGIT) / ("-" 1*DIGIT) ]

         hex-val        =  "x" 1*HEXDIG
                           [ 1*("." 1*HEXDIG) / ("-" 1*HEXDIG) ]

         prose-val      =  "<" *(%x20-3D / %x3F-7E) ">"
                                ; bracketed string of SP and VCHAR
                                ;  without angles
                                ; prose description, to be used as
                                ;  last resort
*/

















// Supposons d'abord que la ligne start_line est une request-line
		// Son premier mot est sa méthode
			// Son premier mot est son token
			// Chacun de ses caractères (au moins 1) doit être un TCHAR
				// TCHAR = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA 
		
		// Vérifier qu'il y a un seul espace
		
		// Son deuxième mot est sa request target
			// Son deuxième mot est son origin-form
			// Son origin-form est son absolute-path[ "?" query ] 
				// Son absolute-path est au moins un ('/' + segment) (here + means concatenation)
				// Un segment est une combinaison de pchar
					// Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@" 
						// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" 
						// pct-encoded = "%" HEXDIG HEXDIG 
						// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
				// Une query est une combinaison de pchar / "/" / "?"

		// Vérifier qu'il y a un seul espace
		
		// Sa HTTP_version est son troisième mot
			// Les quatre premiers caractères doivent être "HTTP"
			// Le caractère suivant doit être un '/'
			// Le caractère suivant doit être un DIGIT
			// Le caractère suivant doit être un '.'
			// Le caractère suivant doit être un DIGIT

		// Vérifier que la ligne se termine bien par un CRLF

int is_request_line(char *characters, char *mots[], int tailles_des_mots[], tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree->name = "request_line\0";
    tree->adress = &is_request_line;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;


    /* On suppose ici que la chaine est correcte (comporte au moins 11 caractères (token = '\0' et segment = '/')) */
    int to_return = 0;
    int index;
    char http_version[9];

    //printf("\n Request line : %s \n\n",characters);

    /* Le token est le premier mot */
    int nb_espaces_1_correct;
    /* Le absolute-path est le deuxième mot */    
    int nb_espaces_2_correct;

    nb_espaces_1_correct = (is_sp(characters[tailles_des_mots[0]],tree) && !(is_sp(characters[tailles_des_mots[0] + 1],tree)));

    //printf("\n Request line : %s. \n\n",characters);

    if(is_token(mots[0],tree->children) && nb_espaces_1_correct) /* is_token(token) assure que token_size est non nul ; tout calculé :) */
    {
        //printf("\n Token correct + nb_espaces_1 correct \n\n");

        nb_espaces_2_correct = (is_sp(characters[tailles_des_mots[0] + 1 + tailles_des_mots[1]],tree) && !(is_sp(characters[tailles_des_mots[0] + 1 + tailles_des_mots[1] + 1],tree)));

        //printf("\n Request line : %s. \n\n",characters);

        //printf("\n\n %d %d %lu \n\n",tailles_des_mots[0], tailles_des_mots[1], strlen(characters));
        //printf(" %d \n\n",nb_espaces_2_correct);
        //printf(" %s \n\n\n\n",mots[1]);

        if(is_absolute_path(mots[1],tree->children->next_node) && nb_espaces_2_correct)
        {
            //printf("\n Absolute path correct + nb_espaces_2 correct \n\n");

            for(index = 0; index < 8; index++) http_version[index] = mots[2][index];
            http_version[index] = '\0';
    
            //printf("\n Request line : %s ; HTTP-version : %s. \n\n",characters,http_version);
            to_return = is_http_version(http_version,tree->children->next_node->next_node);
            //to_return = is_http_version(mots[2],tree->children->next_node->next_node);
            //printf("\n Request line ultime : %s ; to_return : %d \n\n",characters,to_return);
        }
        //printf("\n Request line ultime 2 : %s \n\n",characters);
    }
    //printf("\n\n\n\n %s n'a toujours pas été modifié. \n\n",characters);

    if (to_return == 0) printf("\n %s n'est pas une request line. \n\n",characters);
    //else printf("\n %s est une request line!! \n\n",characters);
    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}


// Supposons maintenant que la ligne start_line soit une status_line
		// Le premier mot est sa HTTP_version
			// Les quatre premiers caractères doivent être "HTTP"
			// Le caractère suivant doit être un '/'
			// Le caractère suivant doit être un DIGIT
			// Le caractère suivant doit être un '.'
			// Le caractère suivant doit être un DIGIT

		// Vérifier qu'il n'y a qu'un seul espace 

		// Le deuxième mot est son status_code
			// Il ne doit comporter que trois caractères 
			// Chacun de ces caractères doit être un DIGIT

		// Vérifier qu'il n'y a qu'un seul espace

		// Le troisième mot est sa reason_phrase
			// Celle-ci doit être une combinaison de HTAB, d'espaces, de VCHAR ou d'obs-text (=?? %x80-FF)

		// Vérifier que la ligne se termine bien par un CRLF

int is_status_line(char *characters, char *mots[], int tailles_des_mots[], tree_node *tree)
{
    tree_node *children;
    children = malloc(sizeof(tree_node));
    children->name = malloc(50*sizeof(char));
    children->name = "HTTP_message\0";
    children->adress = &is_http_message;
    children->content = malloc(500*sizeof(char));
    children->is_correct = 0;
    children->next_node = malloc(sizeof(tree_node));
    children->children = malloc(sizeof(tree_node));
    
    tree_node *children_next;
    children_next = malloc(sizeof(tree_node));
    children_next->name = malloc(50*sizeof(char));
    children_next->name = "HTTP_message\0";
    children_next->adress = &is_http_message;
    children_next->content = malloc(500*sizeof(char));
    children_next->is_correct = 0;
    children_next->next_node = malloc(sizeof(tree_node));
    children_next->children = malloc(sizeof(tree_node));

    tree_node *children_next_next;
    children_next_next = malloc(sizeof(tree_node));
    children_next_next->name = malloc(50*sizeof(char));
    children_next_next->name = "HTTP_message\0";
    children_next_next->adress = &is_http_message;
    children_next_next->content = malloc(500*sizeof(char));
    children_next_next->is_correct = 0;
    children_next_next->next_node = malloc(sizeof(tree_node));
    children_next_next->children = malloc(sizeof(tree_node));

    tree->name = "status_line\0";
    tree->adress = &is_status_line;
    tree->children = children;
    tree->children->next_node = children_next;
    tree->children->next_node->next_node = children_next_next;

    /* On suppose ici que la chaine est correcte (comporte au moins 13 caractères (si reason-phrase = '\0') */
    int to_return = 0;

    char http_version[9];
    int nb_espaces_1_correct;
    char status_code[4];
    int nb_espaces_2_correct;

    int index;
    for (index = 0; index < 8; index++) http_version[index] = characters[index];
    http_version[index] = '\0';

    nb_espaces_1_correct = (is_sp(characters[8],tree) && !(is_sp(characters[9],tree)));
    
    if(is_http_version(http_version,tree->children) && nb_espaces_1_correct)
    {
        for (index = 0; index < 3; index ++) status_code[index] = characters[index + 9];
        status_code[index] = '\0';

        nb_espaces_2_correct = (is_sp(characters[12],tree) && !(is_sp(characters[13],tree)));

        if(is_status_code(status_code,tree->children->next_node) && nb_espaces_2_correct) to_return = is_reason_phrase((characters+13),tree->children->next_node->next_node);
    }

    if (to_return == 0) printf("\n %s n'est pas une status line. \n\n",characters);
    //else printf("\n %s est une status line!! \n\n",characters);

    if (to_return)
    {
        tree->content = characters;
        tree->is_correct = 1;
    }
    else
    {  
        tree->content = NULL;
        purgeNode(tree);
    }
    return to_return;
}