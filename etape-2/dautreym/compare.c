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
    printf("\n Absolute-path : %s de taille %lu\n\n",characters,strlen(characters));
    int to_return = 0;
    int to_return_tmp = 1;
    int index_slash = 0;
    int index = 1;
    int index_reset;
    char segment[strlen(characters)];
    segment[strlen(characters)-1] = '\0';
    //printf("\n %d \n\n",(segment[0]=='\0'));

    while(characters[index_slash] == '/') /* assure que le premier caractère ne soit pas directement '\0' */
    {
        printf("\n Comparaison du segment suivant l'indice %d \n\n",index_slash);
        while(characters[index] != '\0' && characters[index] != '/')
        {
            segment[index] = characters[index];
            index++;
        }

        if(characters[index] == '\0')
        {
            //printf("\n Comparaison dans le if \n\n");
            /* Si le absolute-path n'est qu'un / ou se termina par un /, il est valide */
            if (segment[0] == '\0') to_return = 1;
            /* Sinon on regarde si le segment entre les / est valide */
            else
            {
                to_return_tmp *= is_segment(segment);
                to_return = to_return_tmp;
            }
            //if (to_return == 0) printf("\n Comparaison en cours... Ce n'est pas un segment valide. \n\n");
            //else printf("\n Comparaison en cours... C'est un segment valide!! \n\n");
        }
        else
        {
            to_return_tmp *= is_segment(segment);
            to_return = to_return_tmp;
            //if (to_return == 0) printf("\n Segment invalide dans le else \n\n");
            index++;
            for(index_reset = 0; index_reset < strlen(segment); index_reset++) segment[index_reset] = '\0';
        }
        index_slash = index_slash + index - 1;
        //printf("\n Index slash : %d \n\n",index_slash);
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


int is_lf(char character)
{
    return(character == 0x0A);
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

// Un pchar est unreserved / pct-encoded / sub-delims / ":" / "@" 
int is_pchar(char *characters)
{
    printf("\n Is_pchar %s de taille %lu ?\n\n",characters,strlen(characters));
    int to_return = 0;
    if(strlen(characters) == 3)
    {
        to_return = is_pct_encoded(characters);
    }
    else if(strlen(characters) == 1)
    {
        to_return = (is_unreserved(characters[0]) || is_sub_delims(characters[0]) || characters[0] == ':' || characters[0] == '@');
    }

    if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas un pchar valide. \n\n");
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


// Un segment est une combinaison de pchar
// Un pchar fait soit un caractère soit trois caractères
int is_segment(char *characters)
{
    //printf("\n Segment : %s de taille %lu\n\n",characters,strlen(characters));
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
        break;
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

    if(to_return == 0) printf("\n Fin de la comparaison. Ce n'est pas un segment valide. \n\n");
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