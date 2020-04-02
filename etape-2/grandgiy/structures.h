#ifndef STRUCTURES
    #define STRUCTURES

    typedef enum repetition_type repetition_type;
    typedef enum num_val_type num_val_type;
    typedef enum has_group_or_option_type has_group_or_option_type;

    typedef struct rule rule;
    typedef struct concatenation concatenation;
    typedef struct repetition repetition;
    typedef struct num_val num_val;
    typedef struct num_val_set num_val_set;

    typedef struct node node;

    typedef struct string string;

    enum repetition_type {
        RULENAME,
        GROUP,
        OPTION,
        CHAR_VAL,
        NUM_VAL,
        NO_REPETITION_TYPE
    };

    enum num_val_type {
        SET,
        MIN_MAX,
        NO_NUM_VAL_TYPE
    };

    enum has_group_or_option_type {
        HAS_GROUP,
        HAS_OPTION
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

    struct node {
        string *rulename;
        string *content;
        node *children;
        node *next;
    };

    struct string {
        char *base;
        int length;
    };

    rule *insert_rule(rule **head, char *rulename_base, int rulename_length);
    rule *find_rule(rule *head, char *rulename_base, int rulename_length);
    void delete_last_rule(rule **head);
    void delete_rulelist(rule **head);

    concatenation *insert_concatenation(concatenation **head);
    void delete_last_concatenation(concatenation **head);
    void delete_concatenations(concatenation **head);

    repetition *insert_repetition(repetition **head);
    void delete_last_repetition(repetition **head);
    void delete_repetitions(repetition **head);

    num_val *create_num_val();
    void delete_num_val(num_val **head);

    num_val_set *insert_set_value(num_val_set **head, int value);
    void delete_last_set_value(num_val_set **head);
    void delete_set(num_val_set **head);

    node *insert_node(node **head, string *rulename, string *content);
    void delete_last_node(node **head);
    void delete_nodes(node **head);

    string *create_string(char *base, int length);
    int compare_strings(string *s1, string *s2);
    void delete_string(string **head);
#endif