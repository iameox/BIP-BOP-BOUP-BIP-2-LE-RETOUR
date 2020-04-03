#ifndef STRUCTURES
    #define STRUCTURES

    typedef enum repetition_type Repetition_type;
    typedef enum num_val_type Num_val_type;
    typedef enum has_group_or_option_type Has_group_or_option_type;

    typedef struct rule Rule;
    typedef struct concatenation Concatenation;
    typedef struct repetition Repetition;
    typedef struct num_val Num_val;
    typedef struct num_val_set Num_val_set;

    typedef struct node Node;

    typedef struct string String;

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
        String *rulename;
        Concatenation *concatenations;

        Rule *next;
    };

    struct concatenation {
        Repetition *repetitions;

        Concatenation *next;
    };

    struct repetition {
        Repetition_type type;
        int min;
        int max;
        union {
            String *rulename;
            Concatenation *concatenations;
            String *char_val;
            Num_val *num_val;
        } content;

        Repetition *next;
    };

    struct num_val {
        Num_val_type type;
        union {
            Num_val_set *set;
            int min_max[2];
        } value;
    };

    struct num_val_set {
        int value;

        Num_val_set *next;
    };

    struct node {
        String *rulename;
        String *content;
        Node *children;
        Node *next;
    };

    struct string {
        char *base;
        int length;
    };

    Rule *insert_rule(Rule **head, String *rulename);
    Rule *find_rule(Rule *head, String *rulename);
    void delete_last_rule(Rule **head);
    void delete_rulelist(Rule **head);

    Concatenation *insert_concatenation(Concatenation **head);
    void delete_last_concatenation(Concatenation **head);
    void delete_concatenations(Concatenation **head);

    Repetition *insert_repetition(Repetition **head);
    void delete_last_repetition(Repetition **head);
    void delete_repetitions(Repetition **head);

    Num_val *create_num_val();
    void delete_num_val(Num_val **head);

    Num_val_set *insert_set_value(Num_val_set **head, int value);
    void delete_last_set_value(Num_val_set **head);
    void delete_set(Num_val_set **head);

    Node *insert_node(Node **head, String *rulename, String *content);
    Node *find_node(Node *head, String *rulename, int *index);
    void delete_last_node(Node **head);
    void delete_nodes(Node **head);

    String *create_string(char *base, int length);
    int compare_strings(String *s1, String *s2);
    void delete_string(String **head);
#endif