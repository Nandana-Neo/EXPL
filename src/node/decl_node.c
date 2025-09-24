#include "decl_node.h"


decl_node* create_decl_node(char* varname, int size, VarType type){
    decl_node * node = (decl_node *)malloc(sizeof(decl_node));
    node->symbol_table_entry = add_variable(varname, size, type); 
    node->next = NULL;
    return node;
}


decl_node* create_decl_node_arr(char* varname, int size, VarType type, array* arr_sz, FILE* fp){
    decl_node * node = (decl_node *)malloc(sizeof(decl_node));
    node->symbol_table_entry = add_array_to_symbol(fp,varname,arr_sz, type, size); 
    node->next = NULL;
    return node;
}

decl_node* add_to_list(decl_node* ls,decl_node* node){
    node->next = ls;
    return node;
}


void free_decl_node(decl_node * node){
    free(node);
}


void update_type_symbol_tb(decl_node* node, VarType type){
    decl_node* curr = node;
    while(curr != NULL){
        switch(curr->symbol_table_entry->type){
            case TYPE_INT:
                curr->symbol_table_entry->type = type;
                break;
            case TYPE_INT_PTR:
                curr->symbol_table_entry->type = pointer_type(type);
        }
        curr = curr->next;
    }
}