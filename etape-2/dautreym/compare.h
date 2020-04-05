#ifndef __COMPARE_H__
#define __COMPARE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree_node
{
    char *name;
    void *adress;
    char *content;
    int is_correct;
    struct tree_node *next_node;
    struct tree_node *children;
} tree_node;



int is_absolute_path(char *characters, tree_node *tree);

int is_alpha(char character, tree_node *tree);

int is_bit(char character, tree_node *tree);

int is_bws(char *characters, tree_node *tree);

int is_char(char character, tree_node *tree);

int is_cr(char character, tree_node *tree);

int is_crlf(char *characters, tree_node *tree);

int is_ctl(char character, tree_node *tree);

int is_dec_octet(char *characters, tree_node *tree);

int is_digit(char character, tree_node *tree);

int is_dquote(char character, tree_node *tree);

int is_header(char *ligne, int taille_ligne, tree_node *tree);

int is_hexdig(char character, tree_node *tree);

int is_host(char *characters, tree_node *tree);

int is_host_header(char *characters, tree_node *tree);

int is_hseize(char *characters, tree_node *tree);

int is_htab(char character, tree_node *tree);

int is_http_message_preparsage(char *req, tree_node *tree);

int is_http_message(char *requete, tree_node *tree);

int is_http_version(char *characters, tree_node *tree);

int is_ip_literal(char *characters, tree_node *tree);

int is_ipv_future_adress(char *characters, tree_node *tree);

int is_ipvquatre_adress(char *characters, tree_node *tree);

int is_ipvsix_adress_first_case(char *characters, int nb_deux_points_expected, int option, tree_node *tree);

int is_ipvsix_adress_second_case(char *characters, tree_node *tree);

int is_ipvsix_adress_third_case(char *characters, tree_node *tree);

int is_ipvsix_adress_fourth_case(char *characters, tree_node *tree);

int is_ipvsix_adress(char *characters, tree_node *tree);

int is_lf(char character, tree_node *tree);

int is_lstrentedeux(char *characters, tree_node *tree);

int is_lwsp(char *characters, tree_node *tree);

int is_message_body(char *characters, tree_node *tree);

int is_obs_text(char character, tree_node *tree);

int is_octet(char character, tree_node *tree);

int is_ows(char *characters, tree_node *tree);

int is_pchar(char *characters, tree_node *tree);

int is_pct_encoded(char *characters, tree_node *tree);

int is_port(char *characters, tree_node *tree);

int is_qdtext(char character, tree_node *tree);

int is_query(char *characters, tree_node *tree);

int is_quoted_pair(char *characters, tree_node *tree);

int is_quoted_string(char *characters, tree_node *tree);

int is_reason_phrase(char *characters, tree_node *tree);

int is_reg_name(char *characters, tree_node *tree);

int is_segment(char *characters, tree_node *tree);

int is_sp(char character, tree_node *tree);

int is_start_line(char *characters, int taille_ligne, char *mots[], int tailles_des_mots[], tree_node *tree);

int is_status_code(char *characters, tree_node *tree);

int is_sub_delims(char character, tree_node *tree);

int is_tchar(char character, tree_node *tree);

int is_token(char *characters, tree_node *tree);

int is_transfer_coding(char *characters, tree_node *tree);

int is_transfer_encoding(char *characters, tree_node *tree);

int is_transfer_encoding_header(char *characters, tree_node *tree);

int is_transfer_extension(char *characters, tree_node *tree);

int is_transfer_parameter(char *characters, tree_node *tree);

int is_unreserved(char character, tree_node *tree);

int is_vchar(char character, tree_node *tree);

int is_wsp(char character, tree_node *tree);

int is_request_line(char *characters, char *mots[], int tailles_des_mots[], tree_node *tree);

int is_status_line(char *characters, char *mots[], int tailles_des_mots[], tree_node *tree);

#endif