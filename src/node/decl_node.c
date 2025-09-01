#include "decl_node.h"


decl_node* create_decl_node(char* varname){
    decl_node * node = (decl_node *)malloc(sizeof(decl_node));
    node->varname = varname;
    node->next = NULL;
    return node;
}


decl_node* add_to_list(decl_node* ls,decl_node* node){
    node->next = ls;
    return node;
}


void free_decl_node(decl_node * node){
    free(node->varname);
    free(node);
}