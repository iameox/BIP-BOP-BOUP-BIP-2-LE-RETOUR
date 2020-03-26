#include <stdlib.h>
#include <stdio.h>

typedef enum repetition_type repetition_type;
typedef enum num_val_type num_val_type;
typedef enum opt_or_grp opt_or_grp;
typedef enum bin_or_dec_or_hex bin_or_dec_or_hex;

typedef struct string string;

typedef struct rule rule;
typedef struct concatenation concatenation;
typedef struct repetition repetition;
typedef struct num_val num_val;
typedef struct num_val_set num_val_set;

int is_crlf(char *s);
int is_wsp(char c);
int is_alpha(char c);
int is_digit(char c, int base);
int is_vchar(char c);
int char_to_int(char c);
void create_string(string **head, char *base, int length);
void insert_in_num_val_set(num_val_set **head, int value);
num_val *create_num_val(num_val **head);
void delete_last_repetition(repetition **head);
repetition *create_repetition(repetition **head);
void delete_last_concatenation(concatenation **head);
concatenation *create_concatenation(concatenation **head);
void delete_last_rule(rule **head);
rule *create_rule(rule **head);
int get_bin_or_dec_or_hex_val(bin_or_dec_or_hex type, num_val *num_val, char **abnf);
int get_num_val(num_val **head, char **abnf);
int get_char_val(string **char_val, char **abnf);
int get_group_or_option(opt_or_grp type, concatenation **head, char **abnf);
int get_element(repetition *r, char **abnf);
int get_repeat(repetition *r, char **abnf);
int get_repetition(repetition **head, char **abnf);
int get_concatenation(concatenation **head, char **abnf);
int get_alternation(concatenation **head, char **abnf);
int get_c_nl(char **abnf);
int get_c_wsp(char **abnf);
int get_elements(concatenation **head, char **abnf);
int get_defined_as(char **abnf);
int get_rulename(string **rulename, char **abnf);
int get_rule(rule **head, char **abnf);
int get_rulelist(rule **head, char **abnf);

enum repetition_type {
    RULENAME,
    GROUP,
    OPTION,
    CHAR_VAL,
    NUM_VAL
};

enum num_val_type {
    SET,
    MIN_MAX
};

enum opt_or_grp {
    GRP,
    OPT
};

enum bin_or_dec_or_hex {
    BIN,
    DEC,
    HEX
};

struct string {
    char *base;
    int length;
};

struct rule {
    string *rulename;
    concatenation *concatenations;

    rule *next;
};

struct concatenation {
    repetition *repetitions;

    concatenation *next;
};

struct repetition {
    repetition_type type;
    int min;
    int max;
    union {
        string *rulename;
        concatenation *concatenations;
        string *char_val;
        num_val *num_val;
    } content;

    repetition *next;
};

struct num_val {
    num_val_type type;
    union {
        num_val_set *set;
        int min_max[2];
    } value;
};

struct num_val_set {
    int value;

    num_val_set *next;
};

void print_num_val_set(num_val_set *s);
void print_num_val(num_val *n);
void print_repetitions(repetition *r);
void print_concatenations(concatenation *c);
void print_string(string *s);
void print_rules(rule *r);

void print_num_val_set(num_val_set *s) {
    while (s != NULL) {
        printf("%d ", s->value);
        s = s->next;
    }
    printf("\n");
}

void print_num_val(num_val *n) {
    if (n->type == SET) { printf("Set: "); print_num_val_set(n->value.set); }
    else printf("Min: %d / Max: %d\n", n->value.min_max[0], n->value.min_max[1]);
}

void print_repetitions(repetition *r) {
    while (r != NULL) {
        printf("Min: %d / Max: %d\n", r->min, r->max);
        if (r->type == RULENAME) { printf("Rulename: "); print_string(r->content.rulename); }
        else if (r->type == GROUP) { printf("Group:\n"); print_concatenations(r->content.concatenations); }
        else if (r->type == OPTION) { printf("Option:\n"); print_concatenations(r->content.concatenations); }
        else if (r->type == CHAR_VAL) { printf("Char var: "); print_string(r->content.char_val); }
        else { printf("Num var:\n"); print_num_val(r->content.num_val); }
        r = r->next;
    }
}

void print_concatenations(concatenation *c) {
    while (c != NULL) {
        printf("Repetitions: \n"); print_repetitions(c->repetitions);
        c = c->next;
    }
}

void print_string(string *s) {
    int i;
    for (i = 0; i < s->length; i++) printf("%c", *(s->base+i));
    printf("\n");
}

void print_rules(rule *r) {
    while (r != NULL) {
        printf("Name: "); print_string(r->rulename);
        printf("Concatenations:\n"); print_concatenations(r->concatenations);
        printf("\n");
        r = r->next;
    }
}

int is_crlf(char *s) {
    return *s == '\r' && *(s + 1) == '\n';
}

int is_wsp(char c) {
    return c == ' ' || c == '\t';
}

int is_alpha(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

int is_digit(char c, int base) {
    int return_value, max;

    if (base <= 10) return_value = '0' <= c && c <= '0' + base;
    else if (base <= 36) {
        max = base - 10;
        return_value = ('0' <= c && c <= '9')
            || ('A' <= c && c <= 'A' + max)
            || ('a' <= c && c <= 'a' + max);
    }

    return return_value;
}

int is_vchar(char c) {
    return 32 <= c && c <= 126;
}

int char_to_int(char c) {
    int value = 0;

    if (is_digit(c, 10)) value = c - '0';
    else if (is_alpha(c)) {
        if (c <= 'Z') value = c - 'A';
        else value = c - 'a';
    }

    return value;
}

void create_string(string **head, char *base, int length) {
    string *new = malloc(sizeof(string));
    
    if (new != NULL) {
        new->base = base;
        new->length = length;
        
        *head = new;
    }
}

void insert_in_num_val_set(num_val_set **head, int value) {
    num_val_set *new = malloc(sizeof(num_val_set)), *element = *head;
    if (new != NULL) {
        new->value = value;
        new->next = NULL;

        if (element == NULL) *head = new;
        else {
            while (element->next != NULL) element = element->next;
            element->next = new;
        }
    }
}

num_val *create_num_val(num_val **head) {
    num_val *new = malloc(sizeof(num_val));
    if (new == NULL) return NULL;

    *head = new;
    return new;
}

void delete_last_repetition(repetition **head) { // There is at least a repetition
    repetition *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }
    
    free(element);
}

repetition *create_repetition(repetition **head) {
    repetition *new = malloc(sizeof(repetition)), *element = *head;
    if (new == NULL) return NULL;

    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_concatenation(concatenation **head) { // There is at least a concatenation
    concatenation *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }
    
    free(element);
}

concatenation *create_concatenation(concatenation **head) {
    concatenation *new = malloc(sizeof(concatenation)), *element = *head;
    if (new == NULL) return NULL;

    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

void delete_last_rule(rule **head) { // There is at least a rule
    rule *element = *head, *previous = *head;

    if (element->next == NULL) *head = NULL;
    else {
        while (element->next != NULL) {
            previous = element;
            element = element->next;
        }

        previous->next = NULL;
    }

    free(element);
}

rule *create_rule(rule **head) {
    rule *new = malloc(sizeof(rule)), *element = *head;
    if (new == NULL) return NULL;

    new->next = NULL;

    if (element == NULL) *head = new;
    else {
        while (element->next != NULL) element = element->next;
        element->next = new;
    }

    return new;
}

int get_bin_or_dec_or_hex_val(bin_or_dec_or_hex type, num_val *num_val, char **abnf) {
    char *index = *abnf,
         c;
    int value = 0,
        return_value = 0,
        i = 0,
        base;

    if (type == BIN) {
        c = 'b';
        base = 2;

    } else if (type == DEC) {
        c = 'd';
        base = 10;

    } else {
        c = 'x';
        base = 16;
    }

    if (**abnf != c) return_value = -1;
    else {
        (*abnf)++;

        while (is_digit(**abnf, base)) {
            value = value * base + char_to_int(**abnf);
            (*abnf)++;
            i++;
        }

        if (i == 0) return_value = -1;
        else if (**abnf == '.') {
            num_val->type = SET;

            while (**abnf == '.') {
                insert_in_num_val_set(&(num_val->value.set), value);
                value = 0;
                (*abnf)++;

                while (is_digit(**abnf, base)) {
                    value = value * base + char_to_int(**abnf);
                    (*abnf)++;
                }
            }

            insert_in_num_val_set(&(num_val->value.set), value);

        } else if (**abnf == '-') {
            num_val->type = MIN_MAX;
            num_val->value.min_max[0] = value;
            value = 0;
            (*abnf)++;

            while (is_digit(**abnf, base)) {
                value = value * base + char_to_int(**abnf);
                (*abnf)++;
            }

           num_val->value.min_max[1] = value;
        }
    }

    if (return_value == -1) *abnf = index;
    return return_value;
}

int get_num_val(num_val **head, char **abnf) {
    num_val *n = create_num_val(head);
    int return_value = 0;

    if (**abnf != '%') return_value = -1;
    else {
        (*abnf)++;

        if (get_bin_or_dec_or_hex_val(BIN, n, abnf) == -1) {
            if (get_bin_or_dec_or_hex_val(DEC, n, abnf) == -1) {
                if (get_bin_or_dec_or_hex_val(HEX, n, abnf) == -1) return_value = -1;
            }
        }
    }

    return return_value;
}

int get_char_val(string **char_val, char **abnf) {
    char *index = *abnf,
         *base = *abnf + 1;
    int length = 0,
        return_value = 0;

    if (**abnf != '"') return_value = -1;
    else {
        (*abnf)++;

        while (is_vchar(**abnf) && **abnf != '"') {
            (*abnf)++;
            length++;
        }

        if (**abnf != '"') return_value = -1;
        else {
            (*abnf)++;
            create_string(char_val, base, length);
        }
    }

    if (return_value == -1) *abnf = index;
    return return_value;
}

int get_group_or_option(opt_or_grp type, concatenation **head, char **abnf) {
    char *index = *abnf,
         c1 = type == GRP ? '(' : '[',
         c2 = type == GRP ? ')' : ']';
    int return_value = 0;

    if (**abnf != c1) return_value = -1;
    else {
        (*abnf)++;

        while (get_c_wsp(abnf) != -1);
        if (get_alternation(head, abnf) == -1) return_value = -1;
        else {
            while (get_c_wsp(abnf) != -1);

            if (**abnf != c2) return_value = -1;
            else (*abnf)++;
        }
    }

    if (return_value == -1) *abnf = index;
    return return_value;
}

int get_element(repetition *r, char **abnf) {
    int return_value = 0;

    if (get_rulename(&(r->content.rulename), abnf) != -1) r->type = RULENAME;       
    else if (get_group_or_option(GRP, &(r->content.concatenations), abnf) != -1) r->type = GROUP;
    else if (get_group_or_option(OPT, &(r->content.concatenations), abnf) != -1) r->type = OPTION;
    else if (get_char_val(&(r->content.char_val), abnf) != -1) r->type = CHAR_VAL;
    else if (get_num_val(&(r->content.num_val), abnf) != -1) r->type = NUM_VAL;
    else return_value = -1; // No support for prose-val

    return return_value;
}

int get_repeat(repetition *r, char **abnf) {
    int return_value = 0,
        min = 0,
        max = 0,
        i = 0;

    while (is_digit(**abnf, 10)) {
        min = min * 10 + char_to_int(**abnf);
        (*abnf)++;
        i++;
    }

    if (**abnf != '*') {
        if (i == 0) {
            min = 1;
            return_value = -1;
        }

        r->min = min;
        r->max = min;

    } else {
        (*abnf)++;
        i = 0;

        while (is_digit(**abnf, 10)) {
            max = max * 10 + char_to_int(**abnf);
            (*abnf)++;
            i++;
        }

        r->min = min;
        r->max = i == 0 ? -1 : max;
    }

    return return_value;
}

int get_repetition(repetition **head, char **abnf) {
    repetition *r = create_repetition(head);
    int return_value = 0;

    get_repeat(r, abnf);
    if (get_element(r, abnf) == -1) {
        delete_last_repetition(head);
        return_value = -1;
    }

    return return_value;
}

int get_concatenation(concatenation **head, char **abnf) {
    char *index = *abnf;
    concatenation *c = create_concatenation(head);
    int return_value = 0,
        is_valid = 1,
        i;

    if (get_repetition(&(c->repetitions), abnf) == -1) return_value = -1;
    else {
        while (is_valid) {
            i = 0;
            while (get_c_wsp(abnf) != -1) i++;
            if (i == 0 || get_repetition(&(c->repetitions), abnf) == -1) is_valid = 0;
        }
    }

    if (return_value == -1) {
        delete_last_concatenation(head);
        *abnf = index;
    }

    return return_value;
}

int get_alternation(concatenation **head, char **abnf) {
    char *index = *abnf;
    int return_value = 0,
        is_valid = 1;

    if (get_concatenation(head, abnf) == -1) return_value = -1;
    else {
        while (is_valid) {
            while (get_c_wsp(abnf) != -1); 

            if (**abnf != '/') is_valid = 0;
            else {
                (*abnf)++;
                while (get_c_wsp(abnf) != -1);
                if (get_concatenation(head, abnf) == -1) is_valid = 0;
            }
        }
    }

    if (return_value == -1) *abnf = index;
    return return_value;
}

int get_c_nl(char **abnf) {
    int return_value = 0;

    if (!is_crlf(*abnf)) return_value = -1; // No support for comments
    else (*abnf) += 2;

    return return_value;
}

int get_c_wsp(char **abnf) {
    char *index = *abnf;
    int return_value = 0;

    if (!is_wsp(**abnf)) {
        if (get_c_nl(abnf) == -1) return_value = -1;
        else if (!is_wsp(**abnf)) return_value = -1;
    }

    if (return_value == 0) (*abnf)++;
    else *abnf = index;

    return return_value;
}

int get_elements(concatenation **head, char **abnf) {
    int return_value = 0;

    if (get_alternation(head, abnf) == -1) return_value = -1;
    else while (get_c_wsp(abnf) != -1);

    return return_value;
}

int get_defined_as(char **abnf) {
    char *index = *abnf;
    int return_value = 0;

    while (get_c_wsp(abnf) != -1);
    if (**abnf != '=') return_value = -1; // No support for incremental alternatives
    else {
        (*abnf)++;
        while (get_c_wsp(abnf) != -1);
    }

    if (return_value == -1) *abnf = index;
    return return_value;
}

int get_rulename(string **rulename, char **abnf) {
    char *base = *abnf;
    int length = 0,
        return_value = 0;

    if (!is_alpha(**abnf)) return_value = -1;
    else {
        (*abnf)++;
        length++;

        while (is_alpha(**abnf) || is_digit(**abnf, 10) || **abnf == '-') {
            (*abnf)++;
            length++;
        }

        create_string(rulename, base, length);
    }

    return return_value;
}

int get_rule(rule **head, char **abnf) {
    char *index = *abnf;
    rule *r = create_rule(head);
    int return_value = 0;

    if (get_rulename(&(r->rulename), abnf) == -1) return_value = -1;
    else if (get_defined_as(abnf) == -1) return_value = -1;
    else if (get_elements(&(r->concatenations), abnf) == -1) return_value = -1;
    else if (get_c_nl(abnf) == -1) return_value = -1;
    
    if (return_value == -1) {
        delete_last_rule(head);
        *abnf = index;
    }

    return return_value;
}

int get_rulelist(rule **head, char **abnf) {
    int is_valid = 1, i = 0;

    while (is_valid) {
        if (get_rule(head, abnf) == -1) {
            while (get_c_wsp(abnf) != -1);
            if (get_c_nl(abnf) == -1) is_valid = 0;
        }

        i++;
    }

    return i <= 1 ? -1 : 0;
}

int main () {
    char *abnf = "URI = scheme \":\" hier-part [ \"?\" query ] [ \"#\" fragment ]\r\nhier-part = \"//\" authority path-abempty / path-absolute / path-rootless / path-empty\r\nURI-reference = URI / relative-ref\r\nabsolute-URI = scheme \":\" hier-part [ \"?\" query ]\r\nrelative-ref = relative-part [ \"?\" query ] [ \"#\" fragment ]\r\nrelative-part = \"//\" authority path-abempty / path-absolute / path-noscheme / path-empty\r\nscheme = ALPHA *( ALPHA / DIGIT / \"+\" / \"-\" / \".\" )\r\nauthority = [ userinfo \"@\" ] host [ \":\" port ]\r\nuserinfo = *( unreserved / pct-encoded / sub-delims / \":\" )\r\nhost = IP-literal / IPv4address / reg-name\r\nport = *DIGIT\r\nIP-literal = \"[\" ( IPv6address / IPvFuture ) \"]\"\r\nIPvFuture = \"v\" 1*HEXDIG \".\" 1*( unreserved / sub-delims / \":\" )\r\nIPv6address = 6( h16 \":\" ) ls32 / \"::\" 5( h16 \":\" ) ls32 / [ h16 ] \"::\" 4( h16 \":\" ) ls32 / [ h16 *1( \":\" h16 ) ] \"::\" 3( h16 \":\" ) ls32 / [ h16 *2( \":\" h16 ) ] \"::\" 2( h16 \":\" ) ls32 / [ h16 *3( \":\" h16 ) ] \"::\" h16 \":\" ls32 / [ h16 *4( \":\" h16 ) ] \"::\" ls32 / [ h16 *5( \":\" h16 ) ] \"::\" h16 / [ h16 *6( \":\" h16 ) ] \"::\"\r\nh16 = 1*4HEXDIG\r\nls32 = ( h16 \":\" h16 ) / IPv4address\r\nIPv4address = dec-octet \".\" dec-octet \".\" dec-octet \".\" dec-octet\r\ndec-octet = \"25\" %x30-35 / \"2\" %x30-34 DIGIT / \"1\" 2DIGIT / %x31-39 DIGIT / DIGIT\r\nreg-name = *( unreserved / pct-encoded / sub-delims )\r\npath = path-abempty / path-absolute / path-noscheme / path-rootless / path-empty\r\npath-abempty = *( \"/\" segment )\r\npath-absolute = \"/\" [ segment-nz *( \"/\" segment ) ]\r\npath-noscheme = segment-nz-nc *( \"/\" segment )\r\npath-rootless = segment-nz *( \"/\" segment )\r\npath-empty = \"\"\r\nsegment = *pchar\r\nsegment-nz = 1*pchar\r\nsegment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / \"@\" )\r\npchar = unreserved / pct-encoded / sub-delims / \":\" / \"@\"\r\nquery = *( pchar / \"/\" / \"?\" )\r\nfragment = *( pchar / \"/\" / \"?\" )\r\npct-encoded = \"%\" HEXDIG HEXDIG\r\nunreserved = ALPHA / DIGIT / \"-\" / \".\" / \"_\" / \"~\"\r\nreserved = gen-delims / sub-delims\r\ngen-delims = \":\" / \"/\" / \"?\" / \"#\" / \"[\" / \"]\" / \"@\"\r\nsub-delims = \"!\" / \"$\" / \"&\" / \"'\" / \"(\" / \")\" / \"*\" / \"+\" / \",\" / \";\" / \"=\"\r\nlanguage-range = ( 1*8ALPHA *( \"-\" 1*8alphanum ) ) / \"*\"\r\nalphanum = ALPHA / DIGIT\r\nLanguage-Tag = langtag / privateuse / grandfathered\r\nlangtag = language [ \"-\" script ] [ \"-\" region ] *( \"-\" variant ) *( \"-\" extension ) [ \"-\" privateuse ]\r\nlanguage = 2*3ALPHA [ \"-\" extlang ] / 4ALPHA / 5*8ALPHA\r\nextlang = 3ALPHA *2( \"-\" 3ALPHA )\r\nscript = 4ALPHA\r\nregion = 2ALPHA / 3DIGIT\r\nvariant = 5*8alphanum / ( DIGIT 3alphanum )\r\nextension = singleton 1*( \"-\" ( 2*8alphanum ) )\r\nsingleton = DIGIT / %x41-57 / %x59-5A / %x61-77 / %x79-7A\r\nprivateuse = \"x\" 1*( \"-\" ( 1*8alphanum ) )\r\ngrandfathered = irregular / regular\r\nirregular = \"en-GB-oed\" / \"i-ami\" / \"i-bnn\" / \"i-default\" / \"i-enochian\" / \"i-hak\" / \"i-klingon\" / \"i-lux\" / \"i-mingo\" / \"i-navajo\" / \"i-pwn\" / \"i-tao\" / \"i-tay\" / \"i-tsu\" / \"sgn-BE-FR\" / \"sgn-BE-NL\" / \"sgn-CH-DE\"\r\nregular = \"art-lojban\" / \"cel-gaulish\" / \"no-bok\" / \"no-nyn\" / \"zh-guoyu\" / \"zh-hakka\" / \"zh-min\" / \"zh-min-nan\" / \"zh-xiang\"\r\nBWS = OWS\r\nConnection = *( \",\" OWS ) connection-option *( OWS \",\" [ OWS connection-option ] )\r\nContent-Length = 1*DIGIT\r\nHTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]\r\nHTTP-name = %x48.54.54.50\r\nHTTP-version = HTTP-name \"/\" DIGIT \".\" DIGIT\r\nHost = uri-host [ \":\" port ]\r\nOWS = *( SP / HTAB )\r\nRWS = 1*( SP / HTAB )\r\nTE = [ ( \",\" / t-codings ) *( OWS \",\" [ OWS t-codings ] ) ]\r\nTrailer = *( \",\" OWS ) field-name *( OWS \",\" [ OWS field-name ] )\r\nTransfer-Encoding = *( \",\" OWS ) transfer-coding *( OWS \",\" [ OWS transfer-coding ] )\r\nUpgrade = *( \",\" OWS ) protocol *( OWS \",\" [ OWS protocol ] )\r\nVia = *( \",\" OWS ) ( received-protocol RWS received-by [ RWS comment ] ) *( OWS \",\" [ OWS ( received-protocol RWS received-by [ RWS comment ] ) ] )\r\nabsolute-form = absolute-URI\r\nabsolute-path = 1*( \"/\" segment )\r\nasterisk-form = \"*\"\r\nauthority-form = authority\r\nchunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF\r\nchunk-data = 1*OCTET\r\nchunk-ext = *( \";\" chunk-ext-name [ \"=\" chunk-ext-val ] )\r\nchunk-ext-name = token\r\nchunk-ext-val = token / quoted-string\r\nchunk-size = 1*HEXDIG\r\nchunked-body = *chunk last-chunk trailer-part CRLF\r\ncomment = \"(\" *( ctext / quoted-pair / comment ) \")\"\r\nconnection-option = token\r\nctext = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text\r\nfield-content = field-vchar [ 1*( SP / HTAB ) field-vchar ]\r\nfield-name = token\r\nfield-value = *( field-content / obs-fold )\r\nfield-vchar = VCHAR / obs-text\r\nhttp-URI = \"http://\" authority path-abempty [ \"?\" query ] [ \"#\" fragment ]\r\nhttps-URI = \"https://\" authority path-abempty [ \"?\" query ] [ \"#\" fragment ]\r\nlast-chunk = 1*\"0\" [ chunk-ext ] CRLF\r\nmessage-body = *OCTET\r\nmethod = token\r\nobs-fold = CRLF 1*( SP / HTAB )\r\nobs-text = %x80-FF\r\norigin-form = absolute-path [ \"?\" query ]\r\npartial-URI = relative-part [ \"?\" query ]\r\nprotocol = protocol-name [ \"/\" protocol-version ]\r\nprotocol-name = token\r\nprotocol-version = token\r\npseudonym = token\r\nqdtext = HTAB / SP / \"!\" / %x23-5B / %x5D-7E / obs-text\r\nquoted-pair = \"\\\" ( HTAB / SP / VCHAR / obs-text )\r\nquoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE\r\nrank = ( \"0\" [ \".\" *3DIGIT ] ) / ( \"1\" [ \".\" *3\"0\" ] )\r\nreason-phrase = *( HTAB / SP / VCHAR / obs-text )\r\nreceived-by = ( uri-host [ \":\" port ] ) / pseudonym\r\nreceived-protocol = [ protocol-name \"/\" ] protocol-version\r\nrequest-line = method SP request-target SP HTTP-version CRLF\r\nrequest-target = origin-form\r\nstart-line = request-line / status-line\r\nstatus-code = 3DIGIT\r\nstatus-line = HTTP-version SP status-code SP reason-phrase CRLF\r\nt-codings = \"trailers\" / ( transfer-coding [ t-ranking ] )\r\nt-ranking = OWS \";\" OWS \"q=\" rank\r\ntchar = \"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\" / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\" / DIGIT / ALPHA\r\ntoken = 1*tchar\r\ntrailer-part = *( header-field CRLF )\r\ntransfer-coding = \"chunked\" / \"compress\" / \"deflate\" / \"gzip\" / transfer-extension\r\ntransfer-extension = token *( OWS \";\" OWS transfer-parameter )\r\ntransfer-parameter = token BWS \"=\" BWS ( token / quoted-string )\r\nuri-host = host\r\nAccept = [ ( \",\" / ( media-range [ accept-params ] ) ) *( OWS \",\" [ OWS ( media-range [ accept-params ] ) ] ) ]\r\nAccept-Charset = *( \",\" OWS ) ( ( charset / \"*\" ) [ weight ] ) *( OWS \",\" [ OWS ( ( charset / \"*\" ) [ weight ] ) ] )\r\nAccept-Encoding = [ ( \",\" / ( codings [ weight ] ) ) *( OWS \",\" [ OWS ( codings [ weight ] ) ] ) ]\r\nAccept-Language = *( \",\" OWS ) ( language-range [ weight ] ) *( OWS \",\" [ OWS ( language-range [ weight ] ) ] )\r\nAllow = [ ( \",\" / method ) *( OWS \",\" [ OWS method ] ) ]\r\nContent-Encoding = *( \",\" OWS ) content-coding *( OWS \",\" [ OWS content-coding ] )\r\nContent-Language = *( \",\" OWS ) Language-Tag *( OWS \",\" [ OWS Language-Tag ] )\r\nContent-Location = absolute-URI / partial-URI\r\nContent-Type = media-type\r\nDate = HTTP-date\r\nExpect = \"100-continue\"\r\nGMT = %x47.4D.54\r\nHTTP-date = IMF-fixdate / obs-date\r\nIMF-fixdate = day-name \",\" SP date1 SP time-of-day SP GMT\r\nLocation = URI-reference\r\nMax-Forwards = 1*DIGIT\r\nReferer = absolute-URI / partial-URI\r\nRetry-After = HTTP-date / delay-seconds\r\nServer = product *( RWS ( product / comment ) )\r\nUser-Agent = product *( RWS ( product / comment ) )\r\nVary = \"*\" / ( *( \",\" OWS ) field-name *( OWS \",\" [ OWS field-name ] ) )\r\naccept-ext = OWS \";\" OWS token [ \"=\" ( token / quoted-string ) ]\r\naccept-params = weight *accept-ext\r\nasctime-date = day-name SP date3 SP time-of-day SP year\r\ncharset = token\r\ncodings = content-coding / \"identity\" / \"*\"\r\ncontent-coding = token\r\ndate1 = day SP month SP year\r\ndate2 = day \"-\" month \"-\" 2DIGIT\r\ndate3 = month SP ( 2DIGIT / ( SP DIGIT ) )\r\nday = 2DIGIT\r\nday-name = %x4D.6F.6E / %x54.75.65 / %x57.65.64 / %x54.68.75 / %x46.72.69 / %x53.61.74 / %x53.75.6E\r\nday-name-l = %x4D.6F.6E.64.61.79 / %x54.75.65.73.64.61.79 / %x57.65.64.6E.65.73.64.61.79 / %x54.68.75.72.73.64.61.79 / %x46.72.69.64.61.79 / %x53.61.74.75.72.64.61.79 / %x53.75.6E.64.61.79\r\ndelay-seconds = 1*DIGIT\r\nhour = 2DIGIT\r\nmedia-range = ( \"*/*\" / ( type \"/\" subtype ) / ( type \"/*\" ) ) *( OWS \";\" OWS parameter )\r\nmedia-type = type \"/\" subtype *( OWS \";\" OWS parameter )\r\nminute = 2DIGIT\r\nmonth = %x4A.61.6E / %x46.65.62 / %x4D.61.72 / %x41.70.72 / %x4D.61.79 / %x4A.75.6E / %x4A.75.6C / %x41.75.67 / %x53.65.70 / %x4F.63.74 / %x4E.6F.76 / %x44.65.63\r\nobs-date = rfc850-date / asctime-date\r\nparameter = token \"=\" ( token / quoted-string )\r\nproduct = token [ \"/\" product-version ]\r\nproduct-version = token\r\nqvalue = ( \"0\" [ \".\" *3DIGIT ] ) / ( \"1\" [ \".\" *3\"0\" ] )\r\nrfc850-date = day-name-l \",\" SP date2 SP time-of-day SP GMT\r\nsecond = 2DIGIT\r\nsubtype = token\r\ntime-of-day = hour \":\" minute \":\" second\r\ntype = token\r\nweight = OWS \";\" OWS \"q=\" qvalue\r\nyear = 4DIGIT\r\nETag = entity-tag\r\nIf-Match = \"*\" / ( *( \",\" OWS ) entity-tag *( OWS \",\" [ OWS entity-tag ] ) )\r\nIf-Modified-Since = HTTP-date\r\nIf-None-Match = \"*\" / ( *( \",\" OWS ) entity-tag *( OWS \",\" [ OWS entity-tag ] ) )\r\nIf-Unmodified-Since = HTTP-date\r\nLast-Modified = HTTP-date\r\nentity-tag = [ weak ] opaque-tag\r\netagc = \"!\" / %x23-7E\r\nopaque-tag = DQUOTE *etagc DQUOTE\r\nweak = %x57.2F\r\nAccept-Ranges = acceptable-ranges\r\nContent-Range = byte-content-range / other-content-range\r\nIf-Range = entity-tag / HTTP-date\r\nRange = byte-ranges-specifier / other-ranges-specifier\r\nacceptable-ranges = ( *( \",\" OWS ) range-unit *( OWS \",\" [ OWS range-unit ] ) ) / \"none\"\r\nbyte-content-range = bytes-unit SP ( byte-range-resp / unsatisfied-range )\r\nbyte-range = first-byte-pos \"-\" last-byte-pos\r\nbyte-range-resp = byte-range \"/\" ( complete-length / \"*\" )\r\nbyte-range-set = *( \",\" OWS ) ( byte-range-spec / suffix-byte-range-spec ) *( OWS \",\" [ OWS ( byte-range-spec / suffix-byte-range-spec ) ] )\r\nbyte-range-spec = first-byte-pos \"-\" [ last-byte-pos ]\r\nbyte-ranges-specifier = bytes-unit \"=\" byte-range-set\r\nbytes-unit = \"bytes\"\r\ncomplete-length = 1*DIGIT\r\nfirst-byte-pos = 1*DIGIT\r\nlast-byte-pos = 1*DIGIT\r\nother-content-range = other-range-unit SP other-range-resp\r\nother-range-resp = *CHAR\r\nother-range-set = 1*VCHAR\r\nother-range-unit = token\r\nother-ranges-specifier = other-range-unit \"=\" other-range-set\r\nrange-unit = bytes-unit / other-range-unit\r\nsuffix-byte-range-spec = \"-\" suffix-length\r\nsuffix-length = 1*DIGIT\r\nunsatisfied-range = \"*/\" complete-length\r\nAge = delta-seconds\r\nCache-Control = *( \",\" OWS ) cache-directive *( OWS \",\" [ OWS cache-directive ] )\r\nExpires = HTTP-date\r\nPragma = *( \",\" OWS ) pragma-directive *( OWS \",\" [ OWS pragma-directive ] )\r\nWarning = *( \",\" OWS ) warning-value *( OWS \",\" [ OWS warning-value ] )\r\ncache-directive = token [ \"=\" ( token / quoted-string ) ]\r\ndelta-seconds = 1*DIGIT\r\nextension-pragma = token [ \"=\" ( token / quoted-string ) ]\r\npragma-directive = \"no-cache\" / extension-pragma\r\nwarn-agent = ( uri-host [ \":\" port ] ) / pseudonym\r\nwarn-code = 3DIGIT\r\nwarn-date = DQUOTE HTTP-date DQUOTE\r\nwarn-text = quoted-string\r\nwarning-value = warn-code SP warn-agent SP warn-text [ SP warn-date ]\r\nProxy-Authenticate = *( \",\" OWS ) challenge *( OWS \",\" [ OWS challenge ] )\r\nProxy-Authorization = credentials\r\nWWW-Authenticate = *( \",\" OWS ) challenge *( OWS \",\" [ OWS challenge ] )\r\nauth-param = token BWS \"=\" BWS ( token / quoted-string )\r\nauth-scheme = token\r\nchallenge = auth-scheme [ 1*SP ( token68 / [ ( \",\" / auth-param ) *( OWS \",\" [ OWS auth-param ] ) ] ) ]\r\ncredentials = auth-scheme [ 1*SP ( token68 / [ ( \",\" / auth-param ) *( OWS \",\" [ OWS auth-param ] ) ] ) ]\r\nAuthorization = credentials\r\ntoken68 = 1*( ALPHA / DIGIT / \"-\" / \".\" / \"_\" / \"~\" / \"+\" / \"/\" ) *\"=\"\r\nConnection-header = \"Connection\" \":\" OWS Connection OWS\r\nContent-Length-header = \"Content-Length\" \":\" OWS Content-Length OWS\r\nContent-Type-header = \"Content-Type\" \":\" OWS Content-Type OWS\r\nTrailer-header = \"Trailer\" \":\" OWS Trailer OWS\r\nTransfer-Encoding-header = \"Transfer-Encoding\" \":\" OWS Transfer-Encoding OWS\r\nUpgrade-header = \"Upgrade\" \":\" OWS Upgrade OWS\r\nVia-header = \"Via\" \":\" OWS Via OWS\r\nAge-header = \"Age\" \":\" OWS Age OWS\r\nExpires-header = \"Expires\" \":\" OWS Expires OWS\r\nDate-header = \"Date\" \":\" OWS Date OWS\r\nLocation-header = \"Location\" \":\" OWS Location OWS\r\nRetry-After-header = \"Retry-After\" \":\" OWS Retry-After OWS\r\nVary-header = \"Vary\" \":\" OWS Vary OWS\r\nWarning-header = \"Warning\" \":\" OWS Warning OWS\r\nCache-Control-header = \"Cache-Control\" \":\" OWS Cache-Control OWS\r\nExpect-header = \"Expect\" \":\" OWS Expect OWS\r\nHost-header = \"Host\" \":\" OWS Host OWS\r\nMax-Forwards-header = \"Max-Forwards\" \":\" OWS Max-Forwards OWS\r\nPragma-header = \"Pragma\" \":\" OWS Pragma OWS\r\nRange-header = \"Range\" \":\" OWS Range OWS\r\nTE-header = \"TE\" \":\" OWS TE OWS\r\nIf-Match-header = \"If-Match\" \":\" OWS If-Match OWS\r\nIf-None-Match-header = \"If-None-Match\" \":\" OWS If-None-Match OWS\r\nIf-Modified-Since-header = \"If-Modified-Since\" \":\" OWS If-Modified-Since OWS\r\nIf-Unmodified-Since-header = \"If-Unmodified-Since\" \":\" OWS If-Unmodified-Since OWS\r\nIf-Range-header = \"If-Range\" \":\" OWS If-Range OWS\r\nAccept-header = \"Accept\" \":\" OWS Accept OWS\r\nAccept-Charset-header = \"Accept-Charset\" \":\" OWS Accept-Charset OWS\r\nAccept-Encoding-header = \"Accept-Encoding\" \":\" OWS Accept-Encoding OWS\r\nAccept-Language-header = \"Accept-Language\" \":\" OWS Accept-Language OWS\r\nAuthorization-header = \"Authorization\" \":\" OWS Authorization OWS\r\nProxy-Authorization-header = \"Proxy-Authorization\" \":\" OWS Proxy-Authorization OWS\r\nReferer-header = \"Referer\" \":\" OWS Referer OWS\r\nUser-Agent-header = \"User-Agent\" \":\" OWS User-Agent OWS\r\ncookie-pair = cookie-name \"=\" cookie-value\r\ncookie-name = token\r\ncookie-value = ( DQUOTE *cookie-octet DQUOTE ) / *cookie-octet\r\ncookie-octet = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E\r\nCookie-header = \"Cookie:\" OWS cookie-string OWS\r\ncookie-string = cookie-pair *( \";\" SP cookie-pair )\r\nheader-field = Connection-header / Content-Length-header / Content-Type-header / Cookie-header / Transfer-Encoding-header / Expect-header / Host-header / Accept-header / Accept-Charset-header / Accept-Encoding-header / Accept-Language-header / Referer-header / User-Agent-header / ( field-name \":\" OWS field-value OWS )\r\nALPHA = %x41-5A / %x61-7A\r\nBIT = \"0\" / \"1\"\r\nCHAR = %x01-7F\r\nCR = %x0D\r\nCRLF = CR LF\r\nCTL = %x00-1F / %x7F\r\nDIGIT = %x30-39\r\nDQUOTE = %x22\r\nHEXDIG = DIGIT / \"A\" / \"B\" / \"C\" / \"D\" / \"E\" / \"F\"\r\nHTAB = %x09\r\nLF = %x0A\r\nLWSP = *(WSP / CRLF WSP)\r\nOCTET = %x00-FF\r\nSP = %x20\r\nVCHAR = %x21-7E\r\nWSP = SP / HTAB\r\n";
    rule *head = NULL;
    printf("Parsing...\n");
    if (get_rulelist(&head, &abnf) == -1) printf("ABNF incorrect ou syntaxe non supportée.\n");
    else print_rules(head);
}