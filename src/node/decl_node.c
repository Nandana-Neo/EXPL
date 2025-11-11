#include "decl_node.h"


decl_node* create_decl_node(char* varname, int size, Type* type, FILE* fp){
    decl_node * node = (decl_node *)malloc(sizeof(decl_node));
    node->symbol_table_entry = add_variable_to_symbol(varname, size, type, fp); 
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
        curr->symbol_table_entry->type_entryy->c_type = type->c_type;
        curr->symbol_table_entry->type_entryy->ptr = curr->symbol_table_entry->type_entryy->ptr || type->ptr;
        decl_node* prev = curr;
        curr = curr->next;
        // free(prev);
    }
}


void update_size_decl(decl_node* node, Type* type, FILE* fp){
    int sz = 0;
    if(type->c_type){
        sz = 2;
    }
    else{
        update_type_size(type->type_table);
        sz = type->type_table->size;
    }
    
    decl_node* curr = node;
    while(curr!=NULL){  // first arg is at the end
        SP = curr->symbol_table_entry->binding;
        curr = curr->next;
    }
    curr = node;
    while(curr != NULL){
        // printf("%s\n",curr->symbol_table_entry->name);
        curr->symbol_table_entry->size = sz;
        if(curr->symbol_table_entry->type_entryy->ptr == 1){
            curr->symbol_table_entry->size = 1;
            curr->symbol_table_entry->binding = SP;
            SP += 1;
        }
        else{
            curr->symbol_table_entry->binding = SP;
            if(curr->symbol_table_entry->type_entryy->c_type){
                curr->symbol_table_entry->cbinding = SP + sz - 1;
                printf("%d\n", curr->symbol_table_entry->cbinding);
                fprintf(fp, "MOV [%d], %d\n", SP+sz-1, curr->symbol_table_entry->type_entryy->c_type->class_id); // POSSIBLE ERROR
            }
            SP += sz;
        }
        decl_node* prev = curr;
        curr = curr->next;
        free(prev);
    }
}