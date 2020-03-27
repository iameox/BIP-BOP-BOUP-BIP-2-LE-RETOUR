#ifndef __COMPARE_H__
#define __COMPARE_H__

#include <stdio.h>
#include <stdlib.h>

int is_absolute_path(char *characters);

int is_alpha(char character);

int is_bit(char character);

int is_char(char character);

int is_cr(char character);

int is_crlf(char *characters);

int is_ctl(char character);

int is_digit(char character);

int is_dquote(char character);

int is_hexdig(char character);

int is_htab(char character);

int is_http_version(char *characters);

int is_lf(char character);

int is_lwsp(char *characters);

int is_obs_text(char character);

int is_octet(char character);

int is_pchar(char *characters);

int is_pct_encoded(char *characters);

int is_reason_phrase(char *characters);

int is_segment(char *characters);

int is_sp(char character);

int is_status_code(char *characters);

int is_sub_delims(char character);

int is_tchar(char character);

int is_token(char *characters);

int is_unreserved(char character);

int is_vchar(char character);

int is_wsp(char character);

int is_request_line(char *characters, char *mots[], int tailles_des_mots[]);

int is_status_line(char *characters, char *mots[], int tailles_des_mots[]);

#endif