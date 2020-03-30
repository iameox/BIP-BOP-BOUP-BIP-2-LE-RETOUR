#ifndef __COMPARE_H__
#define __COMPARE_H__

#include <stdio.h>
#include <stdlib.h>

int is_absolute_path(char *characters);

int is_alpha(char character);

int is_bit(char character);

int is_bws(char *characters);

int is_char(char character);

int is_cr(char character);

int is_crlf(char *characters);

int is_ctl(char character);

int is_dec_octet(char *characters);

int is_digit(char character);

int is_dquote(char character);

int is_hexdig(char character);

int is_host(char *characters);

int is_host_header(char *characters);

int is_hseize(char *characters);

int is_htab(char character);

int is_http_version(char *characters);

int is_ip_literal(char *characters);

int is_ipv_future_adress(char *characters);

int is_ipvquatre_adress(char *characters);

int is_ipvsix_adress_first_case(char *characters, int nb_deux_points_expected, int option);

int is_ipvsix_adress_second_case(char *characters);

int is_ipvsix_adress_third_case(char *characters);

int is_ipvsix_adress_fourth_case(char *characters);

int is_ipvsix_adress(char *characters);

int is_lf(char character);

int is_lstrentedeux(char *characters);

int is_lwsp(char *characters);

int is_message_body(char *characters);

int is_obs_text(char character);

int is_octet(char character);

int is_ows(char *characters);

int is_pchar(char *characters);

int is_pct_encoded(char *characters);

int is_port(char *characters);

int is_qdtext(char character);

int is_query(char *characters);

int is_quoted_pair(char *characters);

int is_quoted_string(char *characters);

int is_reason_phrase(char *characters);

int is_reg_name(char *characters);

int is_segment(char *characters);

int is_sp(char character);

int is_status_code(char *characters);

int is_sub_delims(char character);

int is_tchar(char character);

int is_token(char *characters);

int is_transfer_coding(char *characters);

int is_transfer_encoding(char *characters);

int is_transfer_encoding_header(char *characters);

int is_transfer_extension(char *characters);

int is_transfer_parameter(char *characters);

int is_unreserved(char character);

int is_vchar(char character);

int is_wsp(char character);

int is_request_line(char *characters, char *mots[], int tailles_des_mots[]);

int is_status_line(char *characters, char *mots[], int tailles_des_mots[]);

#endif