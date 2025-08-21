#ifndef EXPTREE_HEADER_FILE
#define EXPTREE_HEADER_FILE

#include <stdlib.h>
#include <stdio.h>

typedef enum {
    TYPE_NONE = -1,   // not ID node
    TYPE_INT  = 0,
    TYPE_CHAR = 1
} VarType;

typedef enum {
    NODE_LEAF,
    NODE_CONN,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_EQ,
    NODE_WRITE,
    NODE_READ
} NodeType;

typedef struct tnode{
    int val;    // value of a number for NUM nodes
    VarType type;   // type of variable
    char* varname;   // name of a variable for ID nodes
    NodeType nodetype;   // information about non-leaf nodes - read/write/connector/+/* etc.
    struct tnode *left, *right; //left and right branches
} tnode;


#define YYSTYPE tnode*

/**
 * Function: nodeType
 * --------------------
 * Returns the enum for the node
 * 
 * Input: 
 *  char * type - 'R', 'W', 'C', '+', '-', etc
 * 
 * Output:
 *  NodeType corresponding to the type
 */
NodeType nodeType(char* type);

/**
 * Funtion : createTree
 * ---------------------
 * Create a node tnode
 * 
 * Input:
 *  int val - value of NUM node
 *  VarType type - type of ID node (TYPE_NONE for non leaf node)
 *  char  * c - ID name for ID node or 'READ','WRITE','CONN','+','-',etc
 *  tnode * l - left node in AST
 *  tnode * r - right node in AST
 * 
 * Output:
 *  tnode * pointer to node
*/
tnode* createTree(int val, VarType type, char* c, tnode *l, tnode *r);

/*Make a leaf tnode, can be NUM node or ID node */
tnode* makeLeafNode(int n, VarType type, char* varname);

/*Create operator node*/
tnode* makeOperatorNode(char c,tnode* l, tnode* r);


/**
 * Function : codeGen
 * --------------------
 * Generate assembly code corresponding to AST node
 * Note: It will print newline at the end
*/
int codeGen(tnode* node, FILE * fp);

void prefix(tnode* node);

#endif