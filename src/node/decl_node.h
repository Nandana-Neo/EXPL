#ifndef DECL_NODE_HEADER_FILE
#define DECL_NODE_HEADER_FILE

#include <stdlib.h>
#include "../symbol_table/symbol_table.h"
#include "type_node.h"

/**
 * Node used in declarations section to eventually store into symbol table
 */
typedef struct decl_node{
    Gsymbol* symbol_table_entry;
    struct decl_node* next;
} decl_node;

/**
 * Function: create_decl_node
 * --------------------------
 * Create a decl node with varname as value
 */
decl_node* create_decl_node(char* varname, int size, VarType type);

decl_node* create_decl_node_fn(char* varname, VarType type, parameter* param_list);

decl_node* create_decl_node_arr(char* varname, int size, VarType type, array* arr_sz, FILE* fp);

/**
 * Function: add_to_list
 * --------------------------
 * Add node to the list of decl_nodes specified
 * Returns head of the list
 */
decl_node* add_to_list(decl_node* ls,decl_node* node);

/**
 * Function : free_decl_node
 * ---------------------------
 * Frees the memory assigned for the varname and the node
 */
void free_decl_node(decl_node * node);


void update_type_decl(decl_node* node, VarType type);
#endif