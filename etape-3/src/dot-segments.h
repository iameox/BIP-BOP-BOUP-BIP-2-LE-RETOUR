typedef struct int_stack {
    int top;
    struct int_stack *next; 
} int_stack;

void remove_dot_segments(char *string, int *length);

int push_stack(int_stack **stack, int top);
int pop_stack(int_stack **stack);
void clear_stack(int_stack **stack);

int compare_strings(char *s1, char *s2, int len);
int match_prefix(char *s, char *p, int *len);