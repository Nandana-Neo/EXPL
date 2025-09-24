#include "symbol_table.h"

int SP = 4096;
static int f_label = 0;

int get_f_label(){
    return f_label++;
}

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
    node->symbol_type = SYMBOL_VAR;
    node->next = symbol_table;
    node->size_array = NULL;
    node->f_label = -1;
    node->param_list = NULL;
    symbol_table = node;
    return node;
}

Gsymbol* add_fn(char* name, VarType return_type, parameter* param_list){
    if(get_variable(name)!=NULL){
        fprintf(stderr,"Variable redeclared:%s\n",name);
        exit(1);
    }
    Gsymbol* node = (Gsymbol*)malloc(sizeof(Gsymbol));
    node->name = strdup(name);
    node->symbol_type = SYMBOL_FN;
    node->param_list = param_list;
    node->type = return_type;
    node->f_label = get_f_label();
    node->binding = -1;
    node->size_array = NULL;
    node->size = 0;
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

Gsymbol* add_array_to_symbol(FILE* fp, char* varname,array* array_sz, VarType type, int sz){
    type = pointer_type(type);
    sz+=1;  // first mem stores the arr variable which points to the nxt location
    Gsymbol* node = add_variable(varname, sz, type);
    fprintf(fp,"MOV [%d],%d\n",node->binding,node->binding+1);
    node->size_array = array_sz;
    node->symbol_type = SYMBOL_ARR;
    return node;
}

void print_st(){
    Gsymbol * curr = symbol_table;
    while(curr != NULL){
        printf("%s-%d-%d\n",curr->name,curr->size,curr->binding);
        print_array_int(curr->size_array);
        curr = curr->next;
    }
}

void print_array_int(array * arr){
    array* node = arr;
    while(node!=NULL){
        printf("%d->",node->val);
        node = node->nxt;
    }
    printf("\n");
}

array* add_array_node(array* arr,int val){
    array* node = (array *)malloc(sizeof(array));
    node->val = val;
    //insert at the beginning
    node->nxt = arr;
    return node;
}