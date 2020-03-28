#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "compare.h"
/*
			DEFINITIONS PRIMORDIALES 


ALPHA =  %x41-5A / %x61-7A   ; A-Z / a-z

BIT =  "0" / "1"

CHAR = %x01-7F ; any 7-bit US-ASCII character excluding NUL

CR = %x0D ; carriage return

CRLF = %d13.10
CRLF = CR LF ; Internet standard newline

CTL = %x00-1F / %x7F ; controls

DIGIT = %x30-39 ; 0-9

DQUOTE = %x22 ; " (Double Quote)

HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"

HTAB = %x09 ; horizontal tab

LF = %x0A ; linefeed

LWSP = *(WSP / CRLF WSP) ; Use of this linear-white-space rule permits lines containing only white
						space that are no longer legal in mail headers and have caused interoperability 
						problems in other contexts.
                    	Do not use when defining mail headers and use with caution in other contexts.

OCTET = %x00-FF ; 8 bits of data

SP = %x20

VCHAR = %x21-7E ; visible (printing) characters

WSP = SP / HTAB ; white space

*/


// Son absolute-path est au moins un ('/' + segment) (here + means concatenation)
int is_absolute_path(char *characters)
{
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
            if (segment[0] == '\0') to_return = 1;
            /* Sinon on regarde si le segment entre les / est valide */
            else
            {
                to_return_tmp *= is_segment(segment);
                to_return = to_return_tmp;
            }
            //if (to_return == 0) printf("\n Segment invalide dans le if. \n\n");
            //else printf("\n Segment valide dans le if. \n\n");
        }
        else if (characters[index] != '?')
        {
            //printf("\n\n Comparaison dans le else du segment %s.\n\n",segment);
            to_return_tmp *= is_segment(segment);
            to_return = to_return_tmp;
            //if (to_return == 0) printf("\n Segment invalide dans le else. \n\n");
            //else printf("\n Segment valide dans le else. \n\n");
            index++;
        }
        else /* S'il y a une query */
        {
            to_return_tmp *= is_segment(segment);
            to_return = to_return_tmp;
            //if (to_return == 0) printf("\n Segment invalide dans le else. \n\n");
            //else printf("\n Segment valide dans le else. \n\n");

            index++;
            index_last_slash = index_slash;
            //printf("\n\n\n\n\n\n\n QUERY DETECTED \n\n Index_last_slash : %d \n Index : %d \n\n",index_last_slash,index);
            if (characters[index-1] == '?') is_query_ok = is_query(characters + index);
            else is_query_ok = is_query(characters + index_last_slash + 2);

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
        //printf("\n Index last slash : %d \n Index slash : %d \n Index : %d \n\n",index_last_slash,index_slash,index);
    }

    if (to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas un absolute-path valide. \n\n");
    //else printf("\n Fin de la comparaison. C'est un absolute path valide!! \n\n");
    return to_return;
}

int is_alpha(char character)
{
    return ((0x41 <= character && character <= 0x5A) || (0x61 <= character && character <= 0x7A));
}

int is_bit(char character)
{
    return (character == 0x30 || character == 0x31);
}

int is_bws(char *characters)
{
    return is_ows(characters);
}

int is_char(char character)
{
    return (0x01 <= character && character <= 0x7F);
}

int is_cr(char character)
{
    return (character == 0x0D);
}

int is_crlf(char *characters)
{
    int to_return = 0;
    if(strlen(characters) == 2)
    {
        to_return = (is_cr(characters[0]) && is_lf(characters[1]));
    }
    //else printf("\nTaille Invalide (taille attendue : 2, taille réelle : %lu)\n",strlen(characters));
    return to_return;
}

int is_ctl(char character)
{
    return ((0x00 <= character && character <= 0x1F) || (character == 0x7F));
}

/* dec-octet = "25" %x30-35 / "2" %x30-34 DIGIT / "1" 2 DIGIT / %x31-39 DIGIT / DIGIT */
/* DIGIT = %x30-39 ; 0-9 */
int is_dec_octet(char *characters)
{
    int to_return = 0;

    if (strlen(characters) == 1) to_return = is_digit(characters[0]);
    else if (strlen(characters) == 2) to_return = (0x31 <= characters[0] && characters[0] <= 0x39 && is_digit(characters[1]));
    else if (strlen(characters) == 3) to_return = ((characters[0]==0x32 && characters[1]==0x35 && 0x30 <= characters[2] && characters[2] <= 0x35) || (characters[0]==0x32 && 0x30 <= characters[1] && characters[1] <= 0x34 && is_digit(characters[2])) || (characters[0]==0x31 && is_digit(characters[1]) && is_digit(characters[2])));

    return to_return;
}


int is_digit(char character)
{
    return (0x30 <= character && character <= 0x39);
}

int is_dquote(char character)
{
    return (character == 0x22);
}

int is_hexdig(char character)
{
    return ((is_digit(character)) || (0x61 <= character && character <= 0x66));
}


/*      RAJOUTER L'OPTION [ : port] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*      INCOHÉRENCE MAJEURE DANS LES RFC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*      Les termes définis dans les RFC sont censés être case-insensitive d'après RFC5234 */
/*      MAIS il y a une différence entre un Host et un host car on peut avoir Host = host : port */
int is_host(char *characters)
{
    return ((is_ip_literal(characters)) || (is_ipvquatre_adress(characters)) || (is_reg_name(characters)));
}

/* Dans cette implémentation, on ne se sert pas de la fonction is_ows(char *characters)  -_-'  */
int is_host_header(char *characters)
{
    int to_return = 0;
    int is_end_ok = 0;
    int is_host_ok = 0;

    int index;
    int index_host_beginning = 6;
    int index_host_ending = 6;

    //printf("\n %s de taille %lu \n\n",characters,strlen(characters));

    /* Il faut que le host header commence au minimum par Host: */
    if (strlen(characters) >= 5 && characters[0] == 0x48 && characters[1] == 0x6F && characters[2] == 0x73 && characters[3] == 0x74 && characters[4] == 0x3A)
    {
        //printf("\n Début d'en-tête correcte. \n\n");
        while (is_sp(characters[index_host_beginning]) || is_htab(characters[index_host_beginning]))
        {
            index_host_beginning++;
            index_host_ending++;
        }
        if (characters[index_host_beginning] != '\0') index_host_ending++;
        /* Tant que l'on ne rencontre ni le caractère sentinelle, ni un espace, ni un htab, on avance */
        while (characters[index_host_ending] != '\0' && (!is_sp(characters[index_host_ending]) || !is_htab(characters[index_host_ending]))) index_host_ending++;
    
        index = index_host_ending;
        while (is_sp(characters[index]) || is_htab(characters[index])) index++;
        if (characters[index] == '\0') is_end_ok = 1;

        /* D'après le deuxième if (entre les deux while), la différence suivante est au moins égale à 1 */
        char tmp[index_host_ending - index_host_beginning + 1];
        tmp[index_host_ending - index_host_beginning] = '\0';
        /* index peut aller jusqu'à index_host_ending - index_host_opening - 1 */
        for (index = index_host_beginning; index < index_host_ending; index++) tmp[index - index_host_beginning] = characters[index];

        //printf("\n Appel à is_host... \n\n");
        is_host_ok = is_host(tmp);
        to_return = (is_host_ok && is_end_ok);
        if (is_host_ok == 0) printf("\n %s n'est pas un Host valide. \n\n",tmp);
        else if (is_end_ok == 0) printf("\n Fin invalide. \n\n");
    }
    //else printf("\n %d %d %d %d %d %d \n\n\n\n", strlen(characters) >= 5, characters[0] == 0x48, characters[1] == 0x6F, characters[2] == 0x73, characters[3] == 0x74, characters[4] == 0x3A);

    return to_return;
}


/* h16 = 1*4 HEXDIG */
int is_hseize(char *characters)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index;
    if(1 <= strlen(characters) && strlen(characters) <= 4)
    {
        for (index = 0; index < strlen(characters); index++)
        {
            to_return_tmp *= is_hexdig(characters[index]);
        }
        to_return = to_return_tmp;
    }

    return to_return;
}


int is_htab(char character)
{
    return (character == 0x09);
}


/* Les quatre premiers caractères doivent être "HTTP"
   Le caractère suivant doit être un '/'
   Le caractère suivant doit être un DIGIT
   Le caractère suivant doit être un '.'
   Le caractère suivant doit être un DIGIT
*/
int is_http_version(char *characters)
{
    int to_return = 0;
    if (strlen(characters) == 8)
    {
        to_return = (characters[0] == 'H' && characters[1] == 'T' && characters[2] == 'T' && characters[3] == 'P' && characters[4] == '/' && is_digit(characters[5]) && characters[6] == '.' && is_digit(characters[7]));
    }

    if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas une http-version valide. \n\n");
    return to_return;
}


/* IP-literal = "[" ( IPv6address / IPvFuture ) "]" */
int is_ip_literal(char *characters)
{
    int to_return = 0;
    int index = 0;
    int index_tmp;
    char tmp[strlen(characters) - 1];

    /* la première condition certifie que strlen(characters) >= 1 pour pouvoir effectuer la 2eme comparaison */
    /* la deuxième condition certifie que strlen(characters) >= 2 pour pouvoir effectuer la boucle for*/
    if (characters[index] == 0x5B && characters[strlen(characters)-1] == 0x5D)
    {
        for (index_tmp = 1; index_tmp < strlen(characters)-2 ; index_tmp++) tmp[index_tmp-1] = characters[index_tmp];

        /* Si tmp continet un nombre de char strictment inférieur à 2, to_return = 0 instantanément */
        to_return = (is_ipvsix_adress(tmp) || is_ipv_future_adress(tmp));
    }

    if (to_return == 0) printf("\n %s n'est pas une adresse IP littérale valide. \n\n",characters);
    return to_return;
}

/* IPvFuture = "v" 1* HEXDIG "." 1* ( unreserved / sub-delims / ":" ) */
/* Un is_hexdig ou un is_unreserved ou un is_sub-delims ne prend qu'un char en argument */
int is_ipv_future_adress(char *characters)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 1;
    
    /* certifie que l'on a bien au moins un hexdig et au moins un ( unreserved / sub-delims / ":" ) */
    /* certifie également que la chaine n'est pas juste '\0' */
    if(strlen(characters) >= 4 && characters[0] == 'v')
    {
        while (characters[index] != '\0' && characters[index] != '.')
        {
            to_return_tmp *= is_hexdig(characters[index]);
            index++;
        }
        
        if (characters[index] != '\0') index++; /* certifie que characters[index] était égal à '.' */
        
        while (characters[index] != '\0')
        {
            to_return_tmp *= (is_unreserved(characters[index]) || is_sub_delims(characters[index]) || characters[index] == 0x3A);
            index++;
        }

        to_return = to_return_tmp;
    }

    return to_return;
}


/* IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet */
/* Un dec_octet contient entre 1 et 3 characters */
/* Size min : 7, size max : 15 */
int is_ipvquatre_adress(char *characters)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 0;
    int index_tmp;
    int index_dev_octet;
    char tmp[4];
    tmp[3] = '\0';

    //printf("\n Testons si %s est une adresse IPv4 valide. \n\n",characters);

    if (7 <= strlen(characters) && strlen(characters) <= 15)
    {
        for (index_dev_octet = 0; index_dev_octet < 4; index_dev_octet++)
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
            if (characters[index] == '\0' && index_dev_octet != 3) to_return_tmp = 0;
            else if (index_tmp == 3 && characters[index] != '\0' && characters[index] != 0x2E) to_return_tmp = 0;
            else if (index_tmp != 0)
            {
                while (index_tmp != 3)
                {
                    tmp[index_tmp] = '\0';
                    index_tmp++;
                }
                to_return_tmp *= is_dec_octet(tmp);
            }
            else to_return_tmp = 0;

            index++;
        }

        to_return = to_return_tmp;
    }

    if (to_return == 0) printf("\n %s n'est pas une adresse IPv4 valide. \n\n",characters);
    //else printf("\n %s est une adresse IPv4 valide. \n\n",characters);
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
int is_ipvsix_adress_first_case(char *characters, int nb_deux_points_expected, int option)
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

    while (characters[index] != '\0' && nb_deux_points <= nb_deux_points_expected)
    {
        while (characters[index] != '\0' && characters[index] != 0x3A)
        {
            hseize_tmp[index_tmp] = characters[index];
            index++;
            index_tmp++;
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
            if (option == 0) to_return_tmp *= is_lstrentedeux(lstrentedeux_tmp);
            else to_return_tmp *= (lstrentedeux_tmp[0] == ':' && lstrentedeux_tmp[1] == '\0');
        }
        else if (characters[index] == ':' && nb_deux_points != nb_deux_points_expected)
        {
            nb_deux_points++;
            to_return_tmp *= is_hseize(hseize_tmp);
            for (index_reset = 0; index_reset < 4; index_reset++) hseize_tmp[index_reset] = '\0';
            index++;
            index_tmp = 0;
        }
        else /* Si characters[index] == ':' && nb_deux_points == nb_deux_points_expected */
        {
            for (index_tmp = 0; index_tmp < strlen(characters) - index; index_tmp++) lstrentedeux_tmp[index_tmp] = characters[index + index_tmp];
            if (option == 0) to_return_tmp *= is_lstrentedeux(lstrentedeux_tmp);
            else to_return_tmp *= (lstrentedeux_tmp[0] == 0x3A && lstrentedeux_tmp[1] == '\0');
            to_return = to_return_tmp;
        }
    }

    return to_return;
}


/*
        "::" 5 ( h16 ":" ) ls32 /                           ; case 2
*/
int is_ipvsix_adress_second_case(char *characters)
{
    int to_return = 0;
    if (strlen(characters) > 2)
    {
        to_return = (characters[0] == 0x3A && characters[1] == 0x3A && is_ipvsix_adress_first_case((characters+2), 5, 0));
    }

    return to_return;
}


/*
        [ h16 ] "::" 4 ( h16 ":" ) ls32 /                   ; case 3
*/
int is_ipvsix_adress_third_case(char *characters)
{
    int to_return = 0;
    int index;

    char hseize_tmp[5];
    hseize_tmp[4] = '\0';

    while(characters[index] != '\0' && characters[index] != 0x3A && index < 4)
    {
        hseize_tmp[index] = characters[index];
        index++;
    }
    if(characters[index] != '\0' && characters[index] == 0x3A)
    {
        if(characters[index+1] != '\0' && characters[index+1] == 0x3A) to_return = (is_hseize(hseize_tmp)) && (is_ipvsix_adress_first_case((characters + index + 2), 4, 0));
    }

    return to_return;
}


int is_ipvsix_adress_fourth_case(char *characters)
{
    int to_return = 0;
    int index = 0;
    int nb_deux_points = 0;

    int index_hseize_option;
    int is_hseize_option_correct = 0;
    int index_hseize_option_correct = -1;

    for (index_hseize_option = 1; index_hseize_option <= 6; index_hseize_option++)
    {
        is_hseize_option_correct = (is_hseize_option_correct || is_ipvsix_adress_first_case(characters, index_hseize_option+1, 1));
        if (is_hseize_option_correct) index_hseize_option_correct = index_hseize_option;
    }

    if (is_hseize_option_correct) /* certifie qu'il y ait deux ':' successifs quelque part */
    {
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
        if (index_hseize_option_correct == 1) to_return = is_ipvsix_adress_first_case((characters + index + 1), 3, 0);
        if (index_hseize_option_correct == 2) to_return = is_ipvsix_adress_first_case((characters + index + 1), 2, 0);
        if (index_hseize_option_correct == 3) to_return = is_ipvsix_adress_first_case((characters + index + 1), 1, 0);
        if (index_hseize_option_correct == 4) to_return = is_lstrentedeux(characters + index + 1);
        if (index_hseize_option_correct == 5) to_return = is_hseize(characters + index + 1);
        if (index_hseize_option_correct == 6) to_return = 1;
    }
    else if (strlen(characters) >= 2) /* Traiter les cas où il n'y a pas d'option !!!! */
    {
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
            else if (nb_deux_points == 0) to_return = (is_hseize(characters+2) || is_lstrentedeux(characters+2));
            else if (nb_deux_points >= 1) to_return = is_ipvsix_adress_first_case((characters+2), nb_deux_points, 0);
        }
    }

    return to_return;
}



int is_ipvsix_adress(char *characters)
{
    int to_return = 0;

    /* La plus petite adresse IPv6 acceptée selon les règles abnf est (cas 6) "::" */
    if(strlen(characters) >= 2) to_return = (is_ipvsix_adress_first_case(characters, 6, 0) || is_ipvsix_adress_second_case(characters) || is_ipvsix_adress_third_case(characters) || is_ipvsix_adress_fourth_case(characters));

    if (to_return == 0) printf("\n %s n'est pas une adresse IPv6 valide. \n\n",characters);
    return to_return;
}


int is_lf(char character)
{
    return(character == 0x0A);
}


/* ls32 = 
( h16 ":" h16 ) /           size min : 3 ; size max : 9
IPv4address                 size min : 7 ; size max : 15
*/
int is_lstrentedeux(char *characters)
{
    int to_return = 0;
    int to_return_tmp = 1;
    int index = 0;
    int index_deux_points;
    int is_deux_hseize = 1;

    while(characters[index] != '\0' && characters[index] != 0x3A) index++;
    if (characters[index] == '\0') is_deux_hseize = 0;
    else index_deux_points = index;

    if (is_deux_hseize)
    {
        char tmp[index_deux_points+1];
        tmp[index_deux_points] = '\0';
        for (index = 0; index < index_deux_points; index++) tmp[index] = characters[index];

        to_return_tmp *= is_hseize(tmp);
        for (index = index_deux_points; characters[index] != '\0'; index++) tmp[index - index_deux_points] = characters[index];
        while (index - index_deux_points < index_deux_points)
        {
            tmp[index - index_deux_points] = '\0';
            index++;
        }
        to_return_tmp *= is_hseize(tmp);
        to_return = to_return_tmp;
    }
    else to_return = is_ipvquatre_adress(characters);

    return to_return;
}


// LWSP = *(WSP / CRLF WSP)
int is_lwsp(char *characters)
{
    int to_return = 0;
    int size = strlen(characters);
    int index;
    char tmp[3];
    tmp[2] = '\0';

    if (size > 0)
    {
        to_return = 1;
        for (index = 0; index < size; index++)
        {
            if (index + 1 < size && index + 2 < size)
            {
                tmp[0] = characters[index];
                tmp[1] = characters[index+1];
                to_return *= (is_wsp(characters[index]) || (is_crlf(tmp) && is_wsp(characters[index+2])));
                /* problème si la comparaison avec CR LF est effectuée deux fois */
                /* par exemple si CR LF WSP est envoyé, la deuxième comparaison fait passer to_return à 0 */
                if(is_crlf(tmp)) index++;
                /*if(to_return == 0)
                {
                    printf("\n %d || %d && %d\n\n",is_wsp(characters[index]),is_crlf(tmp),is_wsp(characters[index+2]));
                }*/
            }
            /* si il ne reste qu'un ou deux caractère(s), ça ne peut pas être un CR LF WSP (trois caractères) */
            else
            {
                to_return *= is_wsp(characters[index]);
            }
            // if (to_return == 0) printf("\nWARNING to_return a changé à l'indice %d \n",index);
        }
    }
    //else printf("\nTaille Invalide (taille attendue > 0, taille réelle : %lu)\n",strlen(characters));

    if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas un LWSP valide. \n\n");
    return to_return;
}


/* obs-text = %x80-FF */
int is_obs_text(char character)
{
    return (0x80 <= character && character <= 0xFF);
}


int is_octet(char character)
{
    return (0x00 <= character && character <= 0xFF);
}


int is_ows(char *characters)
{
    int to_return = 1;
    int index;

    while(characters[index] != '\0')
    {
        to_return *= (is_wsp(characters[index]));
        index++;
    }

    return to_return;
}


// Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@" 
int is_pchar(char *characters)
{
    //printf("\n Is_pchar %s de taille %lu ?\n\n",characters,strlen(characters));
    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = is_pct_encoded(characters);
    }
    else if(strlen(characters) == 1)
    {
        to_return = (is_unreserved(characters[0]) || is_sub_delims(characters[0]) || characters[0] == ':' || characters[0] == '@');
    }

    //if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas un pchar valide. \n\n");
    return to_return;
}

// pct-encoded = "%" HEXDIG HEXDIG
int is_pct_encoded(char *characters)
{
    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = (characters[0] == '%' && is_hexdig(characters[1]) && is_hexdig(characters[2]));
    }

    return to_return;
}


int is_port(char *characters)
{
    int to_return = 1;
    int index;

    while(characters[index] != '\0')
    {
        to_return *= (is_digit(characters[index]));
        index++;
    }

    return to_return;
}



/* Une query est une combinaison de ( pchar / "/" / "?" )  */
int is_query(char *characters)
{
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
            
            to_return_tmp = is_pchar(tmp);
            if(to_return_tmp == 0)
            {
                for (index_tmp = 0; index_tmp < 2; index_tmp++)
                {
                    test[0] = characters[index + index_tmp];
                    //printf("\n Test : %s de taille %lu\n\n",test,strlen(test));
                    to_return *= (is_pchar(test) || test[0] == '/' || test[0] == '?');
                    //if(to_return == 0) printf("\n Fin de la comparaison. %s n'est pas un pchar valide. \n\n",test);
                }
                /*
                to_return *= is_pchar(tmp[1]);
                if(to_return == 0) printf("\n Fin de la comparaison. %c n'est pas un pchar valide. \n\n",characters[index+1]);
                to_return *= is_pchar(&characters[index + 2]);
                if(to_return == 0) printf("\n Fin de la comparaison. %c n'est pas un pchar valide. \n\n",characters[index+2]);
                */
            }
            index = index + 3;
        }
        else if(characters[index + 1] != '\0') /* il y a deux caractères avant '\0' */
        { 
            to_return *= is_pchar(&characters[index]);
            to_return *= is_pchar(&characters[index + 1]);
            index = index + 2;
        }
        else
        {
            to_return *= is_pchar(&characters[index]);
            index++;
        }
    }
    if (characters[0] != '\0' && characters[1] == '\0')
    {
        to_return = (is_pchar(characters) || characters[0] == '?' || characters[0] == '/');
    }

    if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas une query valide. \n\n");
    return to_return;
}



// Une reason-phrase doit être une combinaison de HTAB, d'espaces, de VCHAR ou d'obs-text (=?? %x80-FF)
int is_reason_phrase(char *characters)
{
    int to_return = 1;
    int index;

    for(index = 0; index < strlen(characters); index++)
    {
        to_return *= (is_htab(characters[index]) || is_sp(characters[index]) || is_vchar(characters[index]) || is_obs_text(characters[index]));
    }

    return to_return;
}


/* reg-name = * ( unreserved / pct-encoded / sub-delims )  */
/* Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@"   */ 
int is_reg_name(char *characters)
{
    int to_return = 1;
    int index;
    char test[2];
    test[1] = '\0';

    for(index = 0; index < strlen(characters); index++)
    {
        test[0] = characters[index];
        to_return *= (test[0] != ':' && test[0] != '@' && is_pchar(test));
    }

    if (to_return == 0) printf("\n %s n'est pas un reg name valide. \n\n",characters);
    else printf("\n %s est un reg name valide !! \n\n",characters);
    return to_return;
}


// Un segment est une combinaison de pchar
// Un pchar fait soit un caractère soit trois caractères
int is_segment(char *characters)
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
            
            to_return_tmp = is_pchar(tmp);
            if(to_return_tmp == 0)
            {
                for (index_tmp = 0; index_tmp < 3; index_tmp++)
                {
                    test[0] = characters[index + index_tmp];
                    //printf("\n Test : %s de taille %lu : %d\n\n",test,strlen(test),is_pchar(test));
                    to_return *= is_pchar(test);
                    //if(to_return == 0) printf("\n Fin de la comparaison. %s n'est pas un pchar valide. \n\n",test);
                }
                /*
                to_return *= is_pchar(tmp[1]);
                if(to_return == 0) printf("\n Fin de la comparaison. %c n'est pas un pchar valide. \n\n",characters[index+1]);
                to_return *= is_pchar(&characters[index + 2]);
                if(to_return == 0) printf("\n Fin de la comparaison. %c n'est pas un pchar valide. \n\n",characters[index+2]);
                */
            }
            index = index + 3;
        }
        else if(characters[index + 1] != '\0') /* il y a deux caractères avant '\0' */
        { 
            to_return *= is_pchar(&characters[index]);
            to_return *= is_pchar(&characters[index + 1]);
            index = index + 2;
        }
        else
        {
            to_return *= is_pchar(&characters[index]);
            index++;
        }
    }
    if (characters[0] != '\0' && characters[1] == '\0')
    {
        to_return = is_pchar(characters);
    }
    

    if(to_return == 0) printf("\n Fin de la comparaison. %s n'est pas un segment valide. \n\n",characters);
    return to_return;
}

int is_sp(char character)
{
    return (character == 0x20);
}


// Un status_code ne doit comporter que trois caractères 
// Chacun de ces caractères doit être un DIGIT
int is_status_code(char *characters)
{
    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = (is_digit(characters[0]) && is_digit(characters[1]) && is_digit(characters[2]));
    }
    
    return to_return;
}


// sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
int is_sub_delims(char character)
{
    return (character == '!' || character == '$' || character == '&' || character == '\'' || character == '(' || character == ')' || character == '*' || character == '+' || character == ',' || character == ';' || character == '=');
}

// TCHAR = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA 
int is_tchar(char character)
{
    return(is_digit(character) || is_alpha(character) || character == '!' || character == '#' || character == '$' || character == '%' || character == '&' || character == '\'' || character == '*' || character == '+' || character == '-' || character == '.' || character == '^' || character == '_' || character == '`' || character == '|' || character == '~');
}

// Un token est composé d'au moins un caractère ET tous ses caractères sont des TCHAR
int is_token(char *characters)
{
    int to_return = 0;
    int is_tchar_okay = 1;
    int token_size = taille_mot(characters); /* fonction définie dans utils.h */

    int index;
    if(token_size >= 1)
    {
        for(index = 0; index < token_size; index++)
        {
            is_tchar_okay *= is_tchar(characters[index]);
        }

        to_return = is_tchar_okay;
    }

    if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas un token valide. \n\n");
    return to_return;
}

// unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" 
int is_unreserved(char character)
{
    return (is_alpha(character) || is_digit(character) || character == '-' || character == '.' || character == '_' || character == '~');
}

int is_vchar(char character)
{
    return (0x21 <= character && character <= 0x7E);
}

int is_wsp(char character)
{
    return (is_sp(character) || is_htab(character));
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

int is_request_line(char *characters, char *mots[], int tailles_des_mots[])
{
    /* On suppose ici que la chaine est correcte (comporte au moins 11 caractères (token = '\0' et segment = '/')) */
    int to_return = 0;

    /* Le token est le premier mot */
    int nb_espaces_1_correct;
    /* Le absolute-path est le deuxième mot */    
    int nb_espaces_2_correct;

    nb_espaces_1_correct = (is_sp(characters[tailles_des_mots[0]]) && !(is_sp(characters[tailles_des_mots[0] + 1])));

    if(is_token(mots[0]) && nb_espaces_1_correct) /* is_token(token) assure que token_size est non nul ; tout calculé :) */
    {
        //printf("\n Token correct + nb_espaces_1 correct \n\n");

        nb_espaces_2_correct = (is_sp(characters[tailles_des_mots[0] + 1 + tailles_des_mots[1]]) && !(is_sp(characters[tailles_des_mots[0] + 1 + tailles_des_mots[1] + 1])));

        if(is_absolute_path(mots[1]) && nb_espaces_2_correct)
        {
            //printf("\n Absolute path correct + nb_espaces_2 correct \n\n");
            /*
            for(index = 0; index < 8; index++) http_version[index] = mots[2][index];
            http_version[index] = '\0';
    
            to_return = is_http_version(http_version);
            */
            to_return = is_http_version(mots[2]);
        }
    }

    if (to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas une request line. \n\n");
    //else printf("\n Fin de la comparaison. C'est une request line!! \n\n");
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

int is_status_line(char *characters, char *mots[], int tailles_des_mots[])
{
    /* On suppose ici que la chaine est correcte (comporte au moins 13 caractères (si reason-phrase = '\0)) */
    int to_return = 0;

    char http_version[9];
    int nb_espaces_1_correct;
    char status_code[4];
    int nb_espaces_2_correct;

    int index;
    for (index = 0; index < 8; index++) http_version[index] = characters[index];
    http_version[index] = '\0';

    nb_espaces_1_correct = (is_sp(characters[8]) && !(is_sp(characters[9])));
    
    if(is_http_version(http_version) && nb_espaces_1_correct)
    {
        for (index = 0; index < 3; index ++) status_code[index] = characters[index + 9];
        status_code[index] = '\0';

        nb_espaces_2_correct = (is_sp(characters[12]) && !(is_sp(characters[13])));

        if(is_status_code(status_code) && nb_espaces_2_correct) to_return = is_reason_phrase((characters+13));
    }

    if (to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas une status line. \n\n");
    //else printf("\n Fin de la comparaison. C'est une status line!! \n\n");
    return to_return;
}