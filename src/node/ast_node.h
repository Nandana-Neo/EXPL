#ifndef AST_NODE_HEADER_FILE
#define AST_NODE_HEADER_FILE

#include <stdlib.h>
#include <stdio.h>

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

typedef struct tnode{
    int val;    // value of a number for NUM nodes
    VarType type;   // type of variable
    char* varname;   // name of a variable for ID nodes
    NodeType nodetype;   // information about non-leaf nodes - read/write/connector/+/* etc.
    struct tnode *left, *middle, *right; //left and right branches
} tnode;


/**
 * Function: node_type
 * --------------------
 * Returns the enum for the node
 * 
 * Input: 
 *  char * type - 'R', 'W', 'C', '+', '-', etc
 * 
 * Output:
 *  NodeType corresponding to the type
 */
NodeType node_type(char* type);

/**
 * Funtion : create_tree
 * ---------------------
 * Create a node tnode
 * 
 * Input:
 *  int val - value of NUM node
 *  VarType type - type of ID node (TYPE_NONE for non leaf node)
 *  char  * c - ID name for ID node or 'READ','WRITE','CONN','+','-',etc
 * 
 *  tnode * l - left node in AST
 *  tnode * m - middle node in AST
 *  tnode * r - right node in AST
 * 
 * Output:
 *  tnode * pointer to node
*/
tnode* create_tree(int val, VarType type, char* varname, NodeType nodetype, tnode *l, tnode *m, tnode *r);

/*Make a leaf tnode, can be NUM node or ID node */
tnode* make_leaf_node(int n, VarType type, char* varname);

/*Create operator node*/
tnode* make_operator_node(VarType type, NodeType nodetype, tnode* l, tnode* r);

/*Create if else node*/
tnode* make_conditional_node(tnode* l, tnode* m, tnode* r);

/*Prints the prefix notation of the AST*/
void prefix(tnode* node);

tnode* make_break_node(void);
tnode* make_continue_node(void);

#endif