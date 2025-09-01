#include "symbol_table.h"

int SP = 4096;

Gsymbol* add_variable(char* name, VarType type){
    if(get_variable(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    node->type = type;
    node->size = 1;
    node->binding = SP;
    SP++;
    node->next = symbol_table;
    symbol_table = node;
    return node;
}

Gsymbol* get_variable(char* name){
    Gsymbol* node = symbol_table;
    while(node!=NULL){
        if(strcmp(node->name,name)==0){
            return node;
        }
        node = node->next;
    }
    return NULL;
}


void create_entries(decl_node * ls, VarType type){
    decl_node * curr = ls;
    while(curr != NULL){
        add_variable(curr->varname, type);
        decl_node * prev = curr;
        curr = curr->next;
        free_decl_node(prev);
    }
}