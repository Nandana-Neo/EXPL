#ifndef NODE_TYPE_HEADER_FILE
#define NODE_TYPE_HEADER_FILE

typedef enum {
    TYPE_NONE = -1,   // not ID node
    TYPE_INT  = 0,
    TYPE_CHAR = 1,
    TYPE_BOOL = 2,
    TYPE_STR = 3
} VarType;

typedef enum {
    NODE_LEAF,
    NODE_CONN,
    NODE_ADD,   // "+"
    NODE_SUB,   // "-"
    NODE_MUL,   // "*"
    NODE_DIV,   // "/"
    NODE_ASGN,  // "="
    NODE_LT,    // "<"
    NODE_GT,    // ">"
    NODE_LE,    // "<="
    NODE_GE,    // ">="
    NODE_EQ,    // "=="
    NODE_NE,    // "!="
    NODE_IF,        // if (l) then m
    NODE_IFELSE,    // if (l) then m else r ternary node
    NODE_WHILE,      // while
    NODE_REPEAT,     // repeat-until
    NODE_DOWHILE,    // do-while
    NODE_WRITE, 
    NODE_READ,
    NODE_BREAK,
    NODE_CONTINUE
} NodeType;

typedef union node_val {
    int int_val;
    char * str_val;
} node_val;

typedef struct tnode{
    node_val val;    // value of a number for NUM nodes
    VarType type;   // type of variable
    char* varname;   // name of a variable for ID nodes
    NodeType nodetype;   // information about non-leaf nodes - read/write/connector/+/* etc.
    struct Gsymbol* gst_entry;     // pointer to GST entry for global variables and functions
    struct tnode *left, *middle, *right; //left and right branches
} tnode;

typedef struct Gsymbol {
    char* name;       // name of the variable
    VarType type;         // type of the variable - INT or STR
    int size;         // size of the type of the variable - default(1)
    int binding;      // stores the static memory address allocated to the variable
    struct Gsymbol *next;
} Gsymbol;

#endif