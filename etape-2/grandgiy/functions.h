#include <stdio.h>

FILE *open_file(char *file_name);
int get_file_size(FILE *file);
void read_file(FILE *file, char **buffer, int size);
void close_file(FILE *file);

void *create_element(int size);

int is_between(unsigned char c, unsigned char c1, unsigned char c2);
int to_lowercase(char c);

int compare_insensitive(char c1, char c2);
int char_to_digit(char c, int base);

int is_vchar(char c);
int is_alpha(char c);
int is_digit(char c, int base);
int is_wsp(char c);
int is_crlf(char *s);

void exit_on_error(char *error_message);