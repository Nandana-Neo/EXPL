#ifndef AST_NODE_HEADER_FILE
#define AST_NODE_HEADER_FILE

#include <stdlib.h>
#include <stdio.h>

#include "type_node.h"
#include "../symbol_table/symbol_table.h"

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
 *  Gsymbol* gst_entry - pointer to GST entry in case of variables and functions
 *  tnode * l - left node in AST
 *  tnode * m - middle node in AST
 *  tnode * r - right node in AST
 * 
 * Output:
 *  tnode * pointer to node
*/
tnode* create_tree(node_val val, VarType type, char* varname, NodeType nodetype, Gsymbol* gst_entry, tnode *l, tnode *m, tnode *r);

/*Make a leaf tnode, can be NUM/STR node or ID node */
tnode* make_leaf_node(node_val n, VarType type, char* varname, Gsymbol* gst_entry);

/*Create operator node*/
tnode* make_operator_node(VarType type, NodeType nodetype, tnode* l, tnode* r);

/*Create if else node*/
tnode* make_conditional_node(tnode* l, tnode* m, tnode* r);

/*Prints the prefix notation of the AST*/
void prefix(tnode* node);

tnode* make_break_node(void);
tnode* make_continue_node(void);

#endif