#ifndef NODE_TYPE_HEADER_FILE
#define NODE_TYPE_HEADER_FILE

#include <stdio.h>

typedef enum {
    TYPE_NONE = -1,   // not ID node
    TYPE_INT  = 0,
    TYPE_CHAR = 1,
    TYPE_BOOL = 2,
    TYPE_STR = 3,
    TYPE_INT_PTR,
    TYPE_CHAR_PTR
} VarType;

typedef enum {
    NODE_LEAF,
    NODE_CONN,
    NODE_ARR,   // for array
    NODE_INDEX, // for index of array
    NODE_ADDR_OF,    // for ptr = &a
    NODE_VAL_AT,    // for *ptr
    NODE_ADD,   // "+"
    NODE_SUB,   // "-"
    NODE_MUL,   // "*"
    NODE_DIV,   // "/"
    NODE_MOD,   // "%"
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

typedef struct array{
    int val;
    struct array* nxt;
} array;

typedef struct Gsymbol {
    char* name;             // name of the variable
    VarType type;           // type of the variable - INT or STR
    array* size_array;       // stores the length of multidim array
    int size;               // size of the type of the variable - default(1)
    int binding;            // stores the static memory address allocated to the variable
    struct Gsymbol *next;
} Gsymbol;

// loc > val so that when called free_reg(), loc is freed first
typedef struct loc_and_val{
    int val;    // reg no holding the val -> -1 if no register
    int loc;    // reg no holding the location -> -1 if no register
} loc_and_val;

/**
 * Function: pointer_type
 * 
 * Input: type
 * Output: Corresponding type of the pointer
 */
VarType pointer_type(VarType type);

/**
 * Function: variable_type
 * 
 * Input: type of the pointer
 * Output: Corresponding type of the variable which is pointed to by the pointer
 */
VarType variable_type(VarType ptr_type);

/**
 * Function is_pointer_type
 * 
 * Input: type of the node
 * Output:  Returns 1 if the type is pointer
 *          Else returns 0
 */
int is_pointer_type(VarType type);

#endif