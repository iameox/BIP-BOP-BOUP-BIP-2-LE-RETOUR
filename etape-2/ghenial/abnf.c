#include <stdlib.h>
#include <string.h>
#include "abnf.h"


/* 
* Cette fonction initialise toutes les règles ABNF liées au projet HTTP, afin qu'il puisse les utiliser
* Retourne un pointeur vers la règle "HTTP-message"
* D'autres règles peuvent éventuellement être ajoutées, il faut simplement préciser les règles dont elles dépendent.
*/
abnf_rule *init_rules() {
	//Règles terminales
	abnf_rule *ALPHA = create_rule("ALPHA", strlen("ALPHA"), "", 0, 1, NULL);
	abnf_rule *DIGIT = create_rule("DIGIT", strlen("DIGIT"), "", 0, 1, NULL);
	abnf_rule *SP = create_rule("SP", strlen("SP"), "", 0, 1, NULL);
	abnf_rule *CRLF = create_rule("CRLF", strlen("CRLF"), "", 0, 1, NULL);
	abnf_rule *HTAB = create_rule("HTAB", strlen("HTAB"), "", 0, 1, NULL);

	//OWS = *( SP / HTAB )
	char *OWS_name = "OWS";
	char *OWS_exp = "*( SP / HTAB )";
	rule_list *OWS_list = NULL;
	insert_rule(&OWS_list, SP);
	insert_rule(&OWS_list, HTAB);
	abnf_rule *OWS = create_rule(OWS_name, strlen(OWS_name), OWS_exp, strlen(OWS_exp), 0, OWS_list);

	//tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
	char *tchar_name = "tchar";
	char *tchar_exp = "\"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\" / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\" / DIGIT / ALPHA";
	rule_list *liste_tchar = NULL;
	insert_rule(&liste_tchar, ALPHA);
	insert_rule(&liste_tchar, DIGIT);
	abnf_rule *tchar = create_rule(tchar_name, strlen(tchar_name), tchar_exp, strlen(tchar_exp), 0, liste_tchar);

	//token = 1*tchar
	char *token_name = "token";
	char *token_expr = "1*tchar";
	rule_list *liste_token = NULL;
	insert_rule(&liste_token, tchar);
	abnf_rule *token = create_rule(token_name, strlen(token_name), token_expr,strlen(token_expr), 0, liste_token);

	//method = token
	char *method_name = "method";
	char *method_expr = "token";
	rule_list *liste_method = NULL;
	insert_rule(&liste_method, token);
	abnf_rule *method = create_rule(method_name, strlen(method_name), method_expr, strlen(method_expr), 0, liste_method);

	//HTTP-name = "H" "T" "T" "P" (simplifié)
	char *HTTP_name_name = "HTTP-name";
	char *HTTP_name_expr = "\"H\" \"T\" \"T\" \"P\""; // ATTENTION CASE INSENTIVE/SENSITIVE A GERER
	rule_list *liste_HTTP_name = NULL;
	abnf_rule *HTTP_name = create_rule(HTTP_name_name, strlen(HTTP_name_name), HTTP_name_expr, strlen(HTTP_name_expr), 0, liste_HTTP_name);

	//HTTP-version = HTTP-name "/" DIGIT "." DIGIT 
	char *HTTP_version_name = "HTTP-version";
	char *HTTP_version_expr = "HTTP-name \"/\" DIGIT \".\" DIGIT";
	rule_list *liste_HTTP_version = NULL;
	insert_rule(&liste_HTTP_version, HTTP_name);
	insert_rule(&liste_HTTP_version, DIGIT);
	abnf_rule *HTTP_version = create_rule(HTTP_version_name, strlen(HTTP_version_name), HTTP_version_expr, strlen(HTTP_version_expr), 0, liste_HTTP_version);

	//HEXDIG = DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
	char *HEXDIG_name = "HEXDIG";
	char *HEXDIG_expr = "DIGIT / \"A\" / \"B\" / \"C\" / \"D\" / \"E\" / \"F\"";
	rule_list *liste_HEXDIG = NULL;
	insert_rule(&liste_HEXDIG, DIGIT);
	abnf_rule *HEXDIG = create_rule(HEXDIG_name, strlen(HEXDIG_name), HEXDIG_expr, strlen(HEXDIG_expr), 0, liste_HEXDIG);

	//unreserved = ALPHA / DIGIT / "-" / "." / "_" / "~" 
	char *unreserved_name = "unreserved";
	char *unreserved_expr = "ALPHA / DIGIT / \"-\" / \".\" / \"_\" / \"~\"";
	rule_list *liste_unreserved = NULL;
	insert_rule(&liste_unreserved, ALPHA);
	insert_rule(&liste_unreserved, DIGIT);
	abnf_rule *unreserved = create_rule(unreserved_name, strlen(unreserved_name), unreserved_expr, strlen(unreserved_expr), 0, liste_unreserved);

	//pct-encoded = "%" HEXDIG HEXDIG
	char *pct_encoded_name = "pct-encoded";
	char *pct_encoded_expr = "\"%\" HEXDIG HEXDIG";
	rule_list *liste_pct_encoded = NULL;
	insert_rule(&liste_pct_encoded, HEXDIG);
	abnf_rule *pct_encoded = create_rule(pct_encoded_name, strlen(pct_encoded_name), pct_encoded_expr, strlen(pct_encoded_expr), 0, liste_pct_encoded);

	//sub-delims = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
	char *sub_delims_name = "sub-delims";
	char *sub_delims_expr = "\"!\" / \"$\" / \"&\" / \"'\" / \"(\" / \")\" / \"*\" / \"+\" / \",\" / \";\" / \"=\"";
	rule_list *liste_sub_delims = NULL;
	abnf_rule *sub_delims = create_rule(sub_delims_name, strlen(sub_delims_name), sub_delims_expr, strlen(sub_delims_expr), 0, liste_sub_delims);

	//pchar = unreserved / pct-encoded / sub-delims / ":" / "@" 
	char *pchar_name = "pchar";
	char *pchar_expr = "unreserved / pct-encoded / sub-delims / \":\" / \"@\"";
	rule_list *pchar_list = NULL;
	insert_rule(&pchar_list, unreserved);
	insert_rule(&pchar_list, pct_encoded);
	insert_rule(&pchar_list, sub_delims);
	abnf_rule *pchar = create_rule(pchar_name, strlen(pchar_name), pchar_expr, strlen(pchar_expr), 0, pchar_list);

	//segment = *pchar
	char *segment_name = "segment";
	char *segment_expr = "*pchar";
	rule_list *segment_list = NULL;
	insert_rule(&segment_list, pchar);
	abnf_rule *segment = create_rule(segment_name, strlen(segment_name), segment_expr, strlen(segment_expr), 0, segment_list);

	//absolute-path = 1*( "/" segment )
	char *absolute_path_name = "absolute-path";
	char *absolute_path_expr = "1*( \"/\" segment )";
	rule_list *absolute_path_list = NULL;
	insert_rule(&absolute_path_list, segment);
	abnf_rule *absolute_path = create_rule(absolute_path_name, strlen(absolute_path_name), absolute_path_expr, strlen(absolute_path_expr), 0, absolute_path_list);

	//query = *( pchar / "/" / "?" )
	char *query_name = "query";
	char *query_expr = "*( pchar / \"/\" / \"?\" )";
	rule_list *query_list = NULL;
	insert_rule(&query_list, pchar);
	abnf_rule *query = create_rule(query_name, strlen(query_name), query_expr, strlen(query_expr), 0, query_list);

	//origin-form = absolute-path [ "?" query ]
	char *origin_form_name = "origin-form";
	char *origin_form_expr = "absolute-path [ \"?\" query ]";
	rule_list *origin_form_list = NULL;
	insert_rule(&origin_form_list, absolute_path);
	insert_rule(&origin_form_list, query);
	abnf_rule *origin_form = create_rule(origin_form_name, strlen(origin_form_name), origin_form_expr, strlen(origin_form_expr), 0, origin_form_list);

	//request-target = origin-form
	char *request_target_name = "request-target";
	char *request_target_expr = "origin-form";
	rule_list *request_target_list = NULL;
	insert_rule(&request_target_list, origin_form);
	abnf_rule *request_target = create_rule(request_target_name, strlen(request_target_name), request_target_expr, strlen(request_target_expr), 0, request_target_list);

	//request-line = method SP request-target SP HTTP-version CRLF
	char *request_line_name = "request-line";
	char *request_line_expr = "method SP request-target SP HTTP-version CRLF";
	rule_list *request_line_list = NULL;
	insert_rule(&request_line_list, method);
	insert_rule(&request_line_list, SP);
	insert_rule(&request_line_list, request_target);
	insert_rule(&request_line_list, HTTP_version);
	insert_rule(&request_line_list, CRLF);
	abnf_rule *request_line = create_rule(request_line_name, strlen(request_line_name), request_line_expr, strlen(request_line_expr), 0, request_line_list);

	//start-line = request-line
	char *start_line_name = "start-line";
	char *start_line_expr = "request-line";
	rule_list *start_line_list = NULL;
	insert_rule(&start_line_list, request_line);
	abnf_rule *start_line = create_rule(start_line_name, strlen(start_line_name), start_line_expr, strlen(start_line_expr), 0, start_line_list);

	//charset = token
	char *charset_name = "charset";
	char *charset_expr = "token";
	rule_list *charset_list = NULL;
	insert_rule(&charset_list, token);
	abnf_rule *charset = create_rule(charset_name, strlen(charset_name), charset_expr, strlen(charset_expr), 0, charset_list);


	//qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
	char *qvalue_name = "qvalue";
	char *qvalue_expr = "( \"0\" [ \".\" *3DIGIT ] ) / ( \"1\" [ \".\" *3( \"0\" ) ] )";
	rule_list *qvalue_list = NULL;
	insert_rule(&qvalue_list, DIGIT);
	abnf_rule *qvalue = create_rule(qvalue_name, strlen(qvalue_name), qvalue_expr, strlen(qvalue_expr), 0, qvalue_list);

	//weight = OWS ";" OWS "q=" qvalue
	char *weight_name = "weight";
	char *weight_expr = "OWS \";\" OWS \"q=\" qvalue";
	rule_list *weight_list = NULL;
	insert_rule(&weight_list, OWS);
	insert_rule(&weight_list, qvalue);
	abnf_rule *weight = create_rule(weight_name, strlen(weight_name), weight_expr, strlen(weight_expr), 0, weight_list);

	//Accept-Charset = *( "," OWS ) ( ( charset / "*" ) [ weight ] ) *( OWS "," [ OWS ( ( charset / "*" ) [ weight ] ) ] )
	char *Accept_Charset_name = "Accept-Charset";
	char *Accept_Charset_expr = "*( \",\" OWS ) ( ( charset / \"*\" ) [ weight ] ) *( OWS \",\" [ OWS ( ( charset / \"*\" ) [ weight ] ) ] )";
	rule_list *Accept_Charset_list = NULL;
	insert_rule(&Accept_Charset_list, OWS);
	insert_rule(&Accept_Charset_list, charset);
	insert_rule(&Accept_Charset_list, weight);
	abnf_rule *Accept_Charset = create_rule(Accept_Charset_name, strlen(Accept_Charset_name), Accept_Charset_expr, strlen(Accept_Charset_expr), 0, Accept_Charset_list);

	//Accept-Charset-header = "Accept-Charset" ":" OWS Accept-Charset OWS
	char *Accept_Charset_header_name = "Accept-Charset-header";
	char *Accept_Charset_header_expr = "\"Accept-Charset\" \":\" OWS Accept-Charset OWS";
	rule_list *Accept_Charset_header_list = NULL;
	insert_rule(&Accept_Charset_header_list, OWS);
	insert_rule(&Accept_Charset_header_list, Accept_Charset);
	abnf_rule *Accept_Charset_header = create_rule(Accept_Charset_header_name, strlen(Accept_Charset_header_name), Accept_Charset_header_expr, strlen(Accept_Charset_header_expr), 0, Accept_Charset_header_list);

	//scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
	char *scheme_name = "scheme";
	char *scheme_expr = "ALPHA *( ALPHA / DIGIT / \"+\" / \"-\" / \".\" )";
	rule_list *scheme_list = NULL;
	insert_rule(&scheme_list, ALPHA);
	insert_rule(&scheme_list, DIGIT);
	abnf_rule *scheme = create_rule(scheme_name, strlen(scheme_name), scheme_expr, strlen(scheme_expr), 0, scheme_list);

	//userinfo = *( unreserved / pct-encoded / sub-delims / ":" )
	char *userinfo_name = "userinfo";
	char *userinfo_expr = "*( unreserved / pct-encoded / sub-delims / \":\" )";
	rule_list *userinfo_list = NULL;
	insert_rule(&userinfo_list, unreserved);
	insert_rule(&userinfo_list, pct_encoded);
	insert_rule(&userinfo_list, sub_delims);
	abnf_rule *userinfo = create_rule(userinfo_name, strlen(userinfo_name), userinfo_expr, strlen(userinfo_expr), 0, userinfo_list);

	//IPvFuture = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
	char *IPvFuture_name = "IPvFuture";
	char *IPvFuture_expr = "\"v\" 1*HEXDIG \".\" 1*( unreserved / sub-delims / \":\" )";
	rule_list *IPvFuture_list = NULL;
	insert_rule(&IPvFuture_list, HEXDIG);
	insert_rule(&IPvFuture_list, unreserved);
	insert_rule(&IPvFuture_list, sub_delims);
	abnf_rule *IPvFuture = create_rule(IPvFuture_name, strlen(IPvFuture_name), IPvFuture_expr, strlen(IPvFuture_expr), 0, IPvFuture_list);

	//dec-octet = "25" %x30-35 / "2" %x30-34 DIGIT / "1" 2DIGIT / %x31-39 DIGIT / DIGIT
	char *dec_octet_name = "dec-octet";
	char *dec_octet_expr = "\"25\" ( \"0\" / \"1\" / \"2\" / \"3\" / \"4\" / \"5\" ) / \"2\" ( \"0\" / \"1\" / \"2\" / \"3\" / \"4\" ) DIGIT / \"1\" 2*2( DIGIT ) / ( \"1\" / \"2\" / \"3\" / \"4\" / \"5\" / \"6\" / \"7\" / \"8\" / \"9\" ) DIGIT / DIGIT";
	rule_list *dec_octet_list = NULL;
	insert_rule(&dec_octet_list, DIGIT);
	abnf_rule *dec_octet = create_rule(dec_octet_name, strlen(dec_octet_name), dec_octet_expr, strlen(dec_octet_expr), 0, dec_octet_list);

	//IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
	char *IPv4address_name = "IPv4address";
	char *IPv4address_expr = "dec-octet \".\" dec-octet \".\" dec-octet \".\" dec-octet";
	rule_list *IPv4address_list = NULL;
	insert_rule(&IPv4address_list, dec_octet);
	abnf_rule *IPv4address = create_rule(IPv4address_name, strlen(IPv4address_name), IPv4address_expr, strlen(IPv4address_expr), 0, IPv4address_list);

	//h16 = 1*4HEXDIG
	char *h16_name = "h16";
	char *h16_expr = "1*4HEXDIG";
	rule_list *h16_list = NULL;
	insert_rule(&h16_list, HEXDIG);
	abnf_rule *h16 = create_rule(h16_name, strlen(h16_name), h16_expr, strlen(h16_expr), 0, h16_list);

	//ls32 = ( h16 ":" h16 ) / IPv4address
	char *ls32_name = "ls32";
	char *ls32_expr = "( h16 \":\" h16 ) / IPv4address";
	rule_list *ls32_list = NULL;
	insert_rule(&ls32_list, h16);
	insert_rule(&ls32_list, IPv4address);
	abnf_rule *ls32 = create_rule(ls32_name, strlen(ls32_name), ls32_expr, strlen(ls32_expr), 0, ls32_list);

	//IPv6address = 6( h16 ":" ) ls32 / "::" 5( h16 ":" ) ls32 / [ h16 ] "::" 4( h16 ":" ) ls32 / [ h16 *1( ":" h16 ) ] "::" 3( h16 ":" ) ls32 / [ h16 *2( ":" h16 ) ] "::" 2( h16 ":" ) ls32 / [ h16 *3( ":" h16 ) ] "::" h16 ":" ls32 / [ h16 *4( ":" h16 ) ] "::" ls32 / [ h16 *5( ":" h16 ) ] "::" h16 / [ h16 *6( ":" h16 ) ] "::"
	char *IPv6address_name = "IPv6address";
	char *IPv6address_expr = "6*6( h16 \":\" ) ls32 / \"::\" 5*5( h16 \":\" ) ls32 / [ h16 ] \"::\" 4*4( h16 \":\" ) ls32 / [ h16 *1( \":\" h16 ) ] \"::\" 3*3( h16 \":\" ) ls32 / [ h16 *2( \":\" h16 ) ] \"::\" 2*2( h16 \":\" ) ls32 / [ h16 *3( \":\" h16 ) ] \"::\" h16 \":\" ls32 / [ h16 *4( \":\" h16 ) ] \"::\" ls32 / [ h16 *5( \":\" h16 ) ] \"::\" h16 / [ h16 *6( \":\" h16 ) ] \"::\"";
	rule_list *IPv6address_list = NULL;
	insert_rule(&IPv6address_list, h16);
	insert_rule(&IPv6address_list, ls32);
	abnf_rule *IPv6address = create_rule(IPv6address_name, strlen(IPv6address_name), IPv6address_expr, strlen(IPv6address_expr), 0, IPv6address_list);

	//segment-nz = 1*pchar
	char *segment_nz_name = "segment-nz";
	char *segment_nz_expr = "1*pchar";
	rule_list *segment_nz_list = NULL;
	insert_rule(&segment_nz_list, HEXDIG);
	abnf_rule *segment_nz = create_rule(segment_nz_name, strlen(segment_nz_name), segment_nz_expr, strlen(segment_nz_expr), 0, segment_nz_list);

	//segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
	char *segment_nz_nc_name = "segment-nz-nc";
	char *segment_nz_nc_expr = "1*( unreserved / pct-encoded / sub-delims / \"@\" )";
	rule_list *segment_nz_nc_list = NULL;
	insert_rule(&segment_nz_nc_list, unreserved);
	insert_rule(&segment_nz_nc_list, pct_encoded);
	insert_rule(&segment_nz_nc_list, sub_delims);
	abnf_rule *segment_nz_nc = create_rule(segment_nz_nc_name, strlen(segment_nz_nc_name), segment_nz_nc_expr, strlen(segment_nz_nc_expr), 0, segment_nz_nc_list);

	//path-abempty = *( "/" segment )
	char *path_abempty_name = "path-abempty";
	char *path_abempty_expr = "*( \"/\" segment )";
	rule_list *path_abempty_list = NULL;
	insert_rule(&path_abempty_list, segment);
	abnf_rule *path_abempty = create_rule(path_abempty_name, strlen(path_abempty_name), path_abempty_expr, strlen(path_abempty_expr), 0, path_abempty_list);

	//path-absolute = "/" [ segment-nz *( "/" segment ) ]
	char *path_absolute_name = "path-absolute";
	char *path_absolute_expr = "\"/\" [ segment-nz *( \"/\" segment ) ]";
	rule_list *path_absolute_list = NULL;
	insert_rule(&path_absolute_list, segment_nz);
	insert_rule(&path_absolute_list, segment);
	abnf_rule *path_absolute = create_rule(path_absolute_name, strlen(path_absolute_name), path_absolute_expr, strlen(path_absolute_expr), 0, path_absolute_list);

	//path-noscheme = segment-nz-nc *( "/" segment )
	char *path_noscheme_name = "path-noscheme";
	char *path_noscheme_expr = "segment-nz-nc *( \"/\" segment )";
	rule_list *path_noscheme_list = NULL;
	insert_rule(&path_noscheme_list, segment_nz_nc);
	insert_rule(&path_noscheme_list, segment);
	abnf_rule *path_noscheme = create_rule(path_noscheme_name, strlen(path_noscheme_name), path_noscheme_expr, strlen(path_noscheme_expr), 0, path_noscheme_list);

	//path-rootless = segment-nz *( "/" segment )
	char *path_rootless_name = "path-rootless";
	char *path_rootless_expr = "segment-nz *( \"/\" segment )";
	rule_list *path_rootless_list = NULL;
	insert_rule(&path_rootless_list, segment_nz);
	insert_rule(&path_rootless_list, segment);
	abnf_rule *path_rootless = create_rule(path_rootless_name, strlen(path_rootless_name), path_rootless_expr, strlen(path_rootless_expr), 0, path_rootless_list);

	//path-empty = ""
	char *path_empty_name = "path_empty";
	char *path_empty_expr = "";
	rule_list *path_empty_list = NULL;
	abnf_rule *path_empty = create_rule(path_empty_name, strlen(path_empty_name), path_empty_expr, strlen(path_empty_expr), 0, path_empty_list);

	//IP-literal = "[" ( IPv6address / IPvFuture ) "]"
	char *IP_literal_name = "IP-literal";
	char *IP_literal_expr = "\"[\" ( IPvFuture ) \"]\"";
	rule_list *IP_literal_list = NULL;
	insert_rule(&IP_literal_list, IPv6address);
	insert_rule(&IP_literal_list, IPvFuture);
	abnf_rule *IP_literal = create_rule(IP_literal_name, strlen(IP_literal_name), IP_literal_expr, strlen(IP_literal_expr), 0, IP_literal_list);

	//reg-name = *( unreserved / pct-encoded / sub-delims )
	char *reg_name_name = "reg-name";
	char *reg_name_expr = "*( unreserved / pct-encoded / sub-delims )";
	rule_list *reg_name_list = NULL;
	insert_rule(&reg_name_list, unreserved);
	insert_rule(&reg_name_list, pct_encoded);
	insert_rule(&reg_name_list, sub_delims);
	abnf_rule *reg_name = create_rule(reg_name_name, strlen(reg_name_name), reg_name_expr, strlen(reg_name_expr), 0, reg_name_list);

	//host = IP-literal / IPv4address / reg-name
	char *host_name = "host";
	char *host_expr = "IP-literal / IPv4address / reg-name";
	rule_list *host_list = NULL;
	insert_rule(&host_list, IP_literal);
	insert_rule(&host_list, IPv4address);
	insert_rule(&host_list, reg_name);
	abnf_rule *host = create_rule(host_name, strlen(host_name), host_expr, strlen(host_expr), 0, host_list);

	//port = *DIGIT
	char *port_name = "port";
	char *port_expr = "*DIGIT";
	rule_list *port_list = NULL;
	insert_rule(&port_list, IP_literal);
	abnf_rule *port = create_rule(port_name, strlen(port_name), port_expr, strlen(port_expr), 0, port_list);

	//authority = [ userinfo "@" ] host [ ":" port ]
	char *authority_name = "authority";
	char *authority_expr = "[ userinfo \"@\" ] host [ \":\" port ]";
	rule_list *authority_list = NULL;
	insert_rule(&authority_list, userinfo);
	insert_rule(&authority_list, host);
	insert_rule(&authority_list, port);
	abnf_rule *authority = create_rule(authority_name, strlen(authority_name), authority_expr, strlen(authority_expr), 0, authority_list);

	//hier-part = "//" authority path-abempty / path-absolute / path-rootless / path-empty
	char *hier_part_name = "hier-part";
	char *hier_part_expr = "\"//\" authority path-abempty / path-absolute / path-rootless / path-empty";
	rule_list *hier_part_list = NULL;
	insert_rule(&hier_part_list, authority);
	insert_rule(&hier_part_list, path_abempty);
	insert_rule(&hier_part_list, path_absolute);
	insert_rule(&hier_part_list, path_rootless);
	insert_rule(&hier_part_list, path_empty);
	abnf_rule *hier_part = create_rule(hier_part_name, strlen(hier_part_name), hier_part_expr, strlen(hier_part_expr), 0, hier_part_list);

	//absolute-URI = scheme ":" hier-part [ "?" query ]
	char *absolute_URI_name = "absolute-URI";
	char *absolute_URI_expr = "scheme \":\" hier-part [ \"?\" query ]";
	rule_list *absolute_URI_list = NULL;
	insert_rule(&absolute_URI_list, scheme);
	insert_rule(&absolute_URI_list, hier_part);
	insert_rule(&absolute_URI_list, query);
	abnf_rule *absolute_URI = create_rule(absolute_URI_name, strlen(absolute_URI_name), absolute_URI_expr, strlen(absolute_URI_expr), 0, absolute_URI_list);

	//relative-part = "//" authority path-abempty / path-absolute / path-noscheme / path-empty
	char *relative_part_name = "relative_part";
	char *relative_part_expr = "\"//\" authority path-abempty / path-absolute / path-noscheme / path-empty";
	rule_list *relative_part_list = NULL;
	insert_rule(&relative_part_list, authority);
	insert_rule(&relative_part_list, path_abempty);
	insert_rule(&relative_part_list, path_absolute);
	insert_rule(&relative_part_list, path_noscheme);
	insert_rule(&relative_part_list, path_empty);
	abnf_rule *relative_part = create_rule(relative_part_name, strlen(relative_part_name), relative_part_expr, strlen(relative_part_expr), 0, relative_part_list);

	//partial-URI = relative-part [ "?" query ]
	char *partial_URI_name = "partial_URI";
	char *partial_URI_expr = "relative-part [ \"?\" query ]";
	rule_list *partial_URI_list = NULL;
	insert_rule(&partial_URI_list, relative_part);
	insert_rule(&partial_URI_list, query);
	abnf_rule *partial_URI = create_rule(partial_URI_name, strlen(partial_URI_name), partial_URI_expr, strlen(partial_URI_expr), 0, partial_URI_list);

	//Referer = absolute-URI / partial-URI
	char *Referer_name = "Referer";
	char *Referer_expr = "absolute-URI / partial-URI";
	rule_list *Referer_list = NULL;
	insert_rule(&Referer_list, absolute_URI);
	insert_rule(&Referer_list, partial_URI);
	abnf_rule *Referer = create_rule(Referer_name, strlen(Referer_name), Referer_expr, strlen(Referer_expr), 0, Referer_list);

	//Referer-header = "Referer" ":" OWS Referer OWS
	char *Referer_header_name = "Referer-header";
	char *Referer_header_expr = "\"Referer\" \":\" OWS Referer OWS";
	rule_list *Referer_header_list = NULL;
	insert_rule(&Referer_header_list, OWS);
	insert_rule(&Referer_header_list, Referer);
	abnf_rule *Referer_header = create_rule(Referer_header_name, strlen(Referer_header_name), Referer_header_expr, strlen(Referer_header_expr), 0, Referer_header_list);


	//header-field = Accept-Charset-header / Referer-header
	char *header_field_name = "header-field";
	char *header_field_expr = "Accept-Charset-header / Referer-header";
	rule_list *header_field_list = NULL;
	insert_rule(&header_field_list, Accept_Charset_header);
	insert_rule(&header_field_list, Referer_header);
	abnf_rule *header_field = create_rule(header_field_name, strlen(header_field_name), header_field_expr, strlen(header_field_expr), 0, header_field_list);

	//message-body = ""
	char *message_body_name = "message-body";
	char *message_body_expr = "ALPHA";
	rule_list *message_body_list = NULL;
	abnf_rule *message_body = create_rule(message_body_name, strlen(message_body_name), message_body_expr, strlen(message_body_expr), 0, message_body_list);

	//HTTP-message = start-line *( header-field CRLF ) CRLF [ message-body ]
	char *HTTP_message_name = "HTTP-message";
	char *HTTP_message_expr = "start-line *( header-field CRLF ) CRLF [ message-body ]";
	rule_list *HTTP_message_list = NULL;
	insert_rule(&HTTP_message_list, start_line);
	insert_rule(&HTTP_message_list, header_field);
	insert_rule(&HTTP_message_list, CRLF);
	insert_rule(&HTTP_message_list, message_body);
	abnf_rule *HTTP_message = create_rule(HTTP_message_name, strlen(HTTP_message_name), HTTP_message_expr, strlen(HTTP_message_expr), 0, HTTP_message_list);
	
	return HTTP_message;
}


/*
* Fonction qui libère toutes les règles créées
* Paramètre : la règle principale (i.e. la règle qui n'est sous-règle d'aucune autre)
*/
void delete_all_rules(abnf_rule **main_rule) {
	abnf_rule *r;
	rule_list *l;

	if(*main_rule != NULL) {
		l = (*main_rule)->elements;

		//Suppression des sous-règles
		if(l != NULL) {
			delete_all_rules(&(l->rule));
		}

		if(l != NULL && l->next != NULL) {
			delete_all_rules(&(l->next->rule));
		}

		r = *main_rule;
		*main_rule = NULL;
		if(r->elements != NULL) free(r->elements);
		free(r);
	}
}