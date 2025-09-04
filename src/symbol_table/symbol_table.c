#include "symbol_table.h"

int SP = 4096;

Gsymbol* add_variable(char* name, int size, VarType type){
    if(get_variable(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s\n",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    node->type = type;
    node->size = size;
    node->binding = SP;
    SP+=size;
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
        add_variable(curr->varname, curr->size, type);
        decl_node * prev = curr;
        curr = curr->next;
        free_decl_node(prev);
    }
}

void print_st(){
    Gsymbol * curr = symbol_table;
    while(curr != NULL){
        printf("%s-%d-%d\n",curr->name,curr->size,curr->binding);
        curr = curr->next;
    }
}