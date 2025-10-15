#ifndef NODE_TYPE_HEADER_FILE
#define NODE_TYPE_HEADER_FILE

#include <stdio.h>
#include "../type_table/type_table.h"

typedef enum {
    NODE_LEAF,
    NODE_CONN,
    NODE_FN,    // for fn call
    NODE_RET,   // return node to fn call
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
    NODE_AND,   // AND
    NODE_OR,    // OR
    NODE_NOT,   // NOT
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

typedef enum {
    SYMBOL_FN,
    SYMBOL_ARR,
    SYMBOL_VAR
} SymbolType;

/**
 * Value of a variable
 * Can be char* or int
 */
typedef union node_val {
    int int_val;
    char * str_val;
} node_val;

/**
 * AST tree node
 */
typedef struct tnode{
    node_val val;    // value of a number for NUM nodes
    Type* type_entryy;  // type of variable
    char* varname;   // name of a variable for ID nodes
    NodeType nodetype;   // information about non-leaf nodes - read/write/connector/+/* etc.
    struct Gsymbol* gst_entry;     // pointer to GST entry for global variables and functions
    struct Lsymbol* lst_entry;      // pointer to LST entry for local variables and functions
    struct tnode *left, *middle, *right; //left, middle and right branches
    struct tnode* next;     // points to the arguments given into the function
} tnode;

/**
 * To store the array size of multi dimensional arrays
 */
typedef struct array{
    int val;
    struct array* nxt;
} array;

/**
 * To store the parameter list for a function definition
 */
typedef struct Param{
    char* name;
    Type* type_entryy;
    struct Param* next;
} parameter;    

/**
 * Global Symbol Table
 */
typedef struct Gsymbol {
    char* name;             // name of the variable
    Type* type_entryy;
    SymbolType symbol_type; // type of the entity - ARR or FN or VARIABLE
    array* size_array;       // stores the length of multidim array
    int size;               // size of the type of the variable - default(1)
    int binding;            // stores the static memory address allocated to the variable
    parameter* param_list;  // stores the type and name of the parameters of the function
    int f_label;            // the label for identifying the start address of fn code -1 for non fn node
    struct Gsymbol *next;
} Gsymbol;

// loc > val so that when called free_reg(), loc is freed first
typedef struct loc_and_val{
    int val;    // reg no holding the val -> -1 if no register
    int loc;    // reg no holding the location -> -1 if no register
} loc_and_val;

/**
 * Local Symbol Table
 */
typedef struct Lsymbol{
    char* varname;
    Type* type_entryy;
    int binding;
    struct Lsymbol* next;
} Lsymbol;

// Current pointer to local symbol table
extern Lsymbol* curr_lsymbol;
extern int lst_binding;
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