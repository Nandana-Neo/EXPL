#include "decl_node.h"


decl_node* create_decl_node(char* varname, int size, Type* type){
    decl_node * node = (decl_node *)malloc(sizeof(decl_node));
    node->symbol_table_entry = add_variable_to_symbol(varname, size, type); 
    node->next = NULL;
    return node;
}

decl_node* create_decl_node_fn(char* varname, Type* type, parameter* param_list){
    decl_node* node = (decl_node *)malloc(sizeof(decl_node));
    node->symbol_table_entry = add_fn_to_symbol(varname, type, param_list);
    node->next = NULL;
    return node;
}

decl_node* create_decl_node_arr(char* varname, int size, Type* type, array* arr_sz, FILE* fp){
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


void update_type_decl(decl_node* node, Type* type){
    decl_node* curr = node;
    while(curr != NULL){
        curr->symbol_table_entry->type_entryy->type_table = type->type_table;
        curr->symbol_table_entry->type_entryy->ptr = curr->symbol_table_entry->type_entryy->ptr || type->ptr;
        decl_node* prev = curr;
        curr = curr->next;
        free(prev);
    }
}