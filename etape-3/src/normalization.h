typedef struct int_stack {
    int top;
    struct int_stack *next; 
} int_stack;

void percent_decode(char *string, int *length);
void remove_dot_segments(char *string, int *length);

int is_between(unsigned char c, unsigned char c1, unsigned char c2);

int push_stack(int_stack **stack, int top);
int pop_stack(int_stack **stack);
void clear_stack(int_stack **stack);

int compare_strings(char *s1, char *s2, int len);
int match_prefix(char *s, char *p, int *len);