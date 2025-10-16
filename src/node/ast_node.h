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
 * ------------------------------
 * - Can safely delete type because it is reassigned
*/
tnode* create_tree(node_val val, Type* type, char* varname, NodeType nodetype, Gsymbol* gst_entry, Lsymbol* lst_entry, tnode *l, tnode *m, tnode *r, tnode* nxt);

/*Make a leaf tnode, can be NUM/STR node or ID node */
tnode* make_leaf_node(node_val n, Type* type, char* varname, Gsymbol* gst_entry, Lsymbol* lst_entry);

/*Create operator node*/
tnode* make_operator_node(Type* type, NodeType nodetype, tnode* l, tnode* r);

/*Create if else node*/
tnode* make_conditional_node(tnode* l, tnode* m, tnode* r);

/*Prints the prefix notation of the AST*/
void prefix(tnode* node);

tnode* make_break_node(void);
tnode* make_continue_node(void);
tnode* make_array_node(Type* type, tnode* l, tnode* r);
tnode* make_index_node(tnode* l, tnode* r);


// unary operator
tnode* make_address_of_node(tnode* e);
tnode* make_value_at_node(tnode* e);

// represents parent.child with type given (is type of child)
tnode* make_member_of_node(tnode* parent, Type* type, tnode* child);

tnode* make_fn_node(char* name, tnode* arg);
tnode* make_return_node(tnode* e);
tnode* add_node_to_arglist(tnode* arg, tnode* node);

/** Function: int compare_arg_param 
 * ---------------------------------
 * Returns 1 if type of arg_ls and param_ls are matching
 * Else 0
 * 
*/
int compare_arg_param(tnode* arg_ls, parameter* param_ls);

void print_arg_ls(tnode* node);

/* Fress the tree node and all its children and values allocated */
void free_tree(tnode* n);
#endif